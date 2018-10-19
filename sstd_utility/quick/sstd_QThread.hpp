﻿#pragma once

#include "sstd_quick_library_global.hpp"
#include <sstd_memory.hpp>
#include <QtCore/qthread.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <mutex>
#include <future>
#include <shared_mutex>

namespace sstd {

    namespace private_quick_thread_sstd {

        class _1_SSTD_QUICK_LIBRARY_EXPORT RunEventObject : public QObject {
            Q_OBJECT
        public:
            RunEventObject(std::shared_ptr<std::atomic_bool>);
        protected:
            bool event(QEvent *) override;
            std::shared_ptr<std::atomic_bool> mmm_logical_quit;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(RunEventObject)
        };

    }/*private_sstd*/

    class _1_SSTD_QUICK_LIBRARY_EXPORT QuickThread : public QThread {
        Q_OBJECT
    public:
        ~QuickThread();
        QuickThread();

        template<typename ... Args>
        std::shared_ptr< const sstd::vector< std::future<void> > > runInThisThread(Args && ... args) {

            if constexpr ((sizeof...(Args)) == 0) {
                return {};
            } else {
                if (isLogicalQuit()) {
                    return{};
                }
                sstd::vector<std::packaged_task<void(void)>> varCall;
                varCall.reserve(sizeof...(Args));
                (QuickThread::ppp_push_back(&varCall, std::forward<Args>(args)), ...);
                return ppp_Call(std::move(varCall));
            }

        }

        template<typename Tuple>
        std::shared_ptr< const sstd::vector< std::future<void> > > applyInThisThread(Tuple && arg) {
            const static constexpr auto varTupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            return ppp_apply(this, std::forward < Tuple >(arg), std::make_index_sequence<varTupleSize>{});
        }

        void setLogicalQuit(bool a) {
            mmm_logical_quit->store(a);
        }

        bool isLogicalQuit() const {
            return mmm_logical_quit->load();
        }

    protected:
        void run() override;
    private:
        std::shared_ptr<std::atomic_bool> mmm_logical_quit;
        std::shared_mutex mmm_Mutex;
        private_quick_thread_sstd::RunEventObject * mmm_CallObject{ nullptr };
        Q_SLOT void ppp_on_finished();
    private:
        template<typename T>
        static inline void ppp_push_back(sstd::vector<std::packaged_task<void(void)>> * a, T && v) {
            a->emplace_back(std::forward<T>(v));
        }
        static inline void ppp_push_back(sstd::vector<std::packaged_task<void(void)>> * a, std::packaged_task<void(void)> && v) {
            a->push_back(std::move(v));
        }
        template<typename Tuple, std::size_t... I>
        static inline std::shared_ptr< const sstd::vector< std::future<void> > >
            ppp_apply(QuickThread *d, Tuple && t, const std::index_sequence<I...> &) {
            return d->runInThisThread(std::get<I>(std::forward<Tuple>(t))...);
        }
    private:
        std::shared_ptr< const sstd::vector< std::future<void> > > ppp_Call(sstd::vector<std::packaged_task<void(void)>> &&);
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(QuickThread)
    };

}/*namespace sstd*/











