﻿#pragma once

#include "sstd_memory.hpp"

#ifndef _7_SSTD_FUNCTION_EXPORT_
#define _7_SSTD_FUNCTION_EXPORT_ _1_SSTD_CORE_EXPORT
#endif

namespace sstd {

    class FunctionStack/*函数栈*/;
    class ForFunction/*for*/;
    class WhileFunction/*while*/;
    class DoWhileFunction/*do while*/;
    class JudgeFunction/*get bool*/;
    class IfElseFunction/*if else*/;
    class YieldAns/*reutrn yield*/;
    class Function/*function*/;
    class FunctionStackState/*函数栈的数据*/;

    namespace private_sstd_function_stack {

        class _7_SSTD_FUNCTION_EXPORT_ DataBasic {
            SSTD_DELETE_COPY_ASSIGN(DataBasic);
        protected:
            inline DataBasic() = default;
            friend class sstd::FunctionStack;
        public:
            virtual ~DataBasic();
            inline virtual bool isFunction() const {
                return false;
            }
            inline virtual bool isError() const {
                return false;
            }
            inline virtual bool isJudgeFunction() const {
                return false;
            }
            inline virtual bool isYield() const {
                return false;
            }
        private:
            SSTD_MEMORY_DEFINE(DataBasic)
        };

    }/*private_sstd_function_stack*/

    using FunctionData = private_sstd_function_stack::DataBasic;

    class _7_SSTD_FUNCTION_EXPORT_ YieldAns :
        public private_sstd_function_stack::DataBasic {
    public:
        inline virtual bool isYield() const override {
            return true;
        }
        static YieldAns * create();
    protected:
        friend class sstd::FunctionStack;
    };

    class _7_SSTD_FUNCTION_EXPORT_ Function :
        public private_sstd_function_stack::DataBasic {
    public:
        virtual ~Function();
        inline virtual bool isFunction() const override {
            return true;
        }
        Function * next{ nullptr }/*下一个执行函数*/;
        FunctionData * ans{ nullptr }/*只有当next==nullptr才应当设置这个值*/;
        virtual void call(const FunctionStack *) = 0;
    protected:
        bool set_to_yield(const FunctionStack *);
    protected:
        friend class sstd::FunctionStack;
    };

    class JudgeFunction :
        public private_sstd_function_stack::DataBasic {
    public:
        inline virtual bool call(const FunctionStack *) {
            return false;
        }
        inline bool isJudgeFunction() const override {
            return true;
        }
    protected:
        friend class sstd::FunctionStack;
    };

    class _7_SSTD_FUNCTION_EXPORT_ FunctionStackState {
        friend class sstd::FunctionStack/*函数栈*/;
        friend class sstd::ForFunction/*for*/;
        friend class sstd::WhileFunction/*while*/;
        friend class sstd::DoWhileFunction/*do while*/;
        friend class sstd::JudgeFunction/*get bool*/;
        friend class sstd::IfElseFunction/*if else*/;
        friend class sstd::YieldAns/*reutrn yield*/;
        friend class sstd::Function/*function*/;
    protected:
        Function * currentFunction{ nullptr };
        bool isYieldInLoop{ false };
        virtual ~FunctionStackState();
    public:
        bool isEndl{ false };
        bool hasError{ false };
        bool isYield{ false };
    protected:
        friend class sstd::FunctionStack;
    };

    class _7_SSTD_FUNCTION_EXPORT_ FunctionStackError final :
    public private_sstd_function_stack::DataBasic{
private:
    sstd::string * mmm_errorString{nullptr};
public:
    FunctionStackError();
    ~FunctionStackError();
    FunctionStackError(const std::string_view & arg);
    inline virtual bool isError() const {
        return true;
    }
    static std::shared_ptr<FunctionStackError> create(const std::string_view & arg);
    inline const sstd::string & what() const {
        return mmm_errorString ? (*mmm_errorString) : unknowError();
    }
    static const sstd::string & unknowError();
    static std::shared_ptr<FunctionStackError> createUnknowError();
    };

    class _7_SSTD_FUNCTION_EXPORT_ FunctionStack :
        public FunctionStackState {
        using memory_pool_type = sstd::MemoryLock<std::unique_ptr<private_sstd_function_stack::DataBasic>>;
        memory_pool_type * mmm_MemoryPool{ nullptr };
        std::shared_ptr<FunctionData> * mmm_ThisError{ nullptr };
    public:
        FunctionStack();
        ~FunctionStack();
        /*在当前内存池中创建对象,对象必须继承自FunctionData*/
        template<typename T, typename ... K>
        inline T * createData(K && ... arg);
    public:
        FunctionData * call(Function * arg);
        void error(std::string_view) const/*throw error!*/;
        FunctionData *resume()/*重启标记为yield的函数*/;
        void yield() const/*将当前函数标记为yield*/;
    protected:
        FunctionData * when_error();
        FunctionData *next_call();
    };
}/*namespace sstd*/


namespace sstd {
    template<typename T, typename ... K>
    inline T * FunctionStack::createData(K && ... arg) {
        static_assert(false == std::is_reference_v<T>);
        static_assert(false == std::is_array_v<T>);
        auto varAnsUnique = sstd::make_unique< std::remove_cv_t<T> >(std::forward<K>(arg)...);
        auto varAns = varAnsUnique.get();
        mmm_MemoryPool->emplace_back(std::move(varAnsUnique)) ;
        return varAns;
    }
}/*namespace sstd*/

/*提供一个C++原生实现的，类似于LUA的动态函数运行环境*/

