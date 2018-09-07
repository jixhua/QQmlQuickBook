﻿#include <sstd_glew.hpp>
#include <sstd_load_utf8_file.hpp>
#include "Window.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include "ImageView.hpp"
#include <chrono>

using namespace std::chrono_literals;

Window::~Window() {}

Window::Window() {
    $m$ImageView = sstdNew< ImageView >();
    connect($m$ImageView, &QObject::destroyed, this, &QObject::deleteLater);
    connect(this, &QObject::destroyed, $m$ImageView, &QObject::deleteLater);
    $m$WatcherImageView = $m$ImageView;
    $m$ImageView->show();
    const auto varPos = $m$ImageView->geometry().bottomRight();
    this->setPosition(varPos);
}

class Window::DrawData : public QObject {
public:
    GLuint $m$InputImage = 0;
    GLuint $m$OutputImage = 0;
    GLuint $m$Program = 0;
    GLint  $m$ImageWidth = 0;
    GLint  $m$ImageHeight = 0;
    QImage $m$OriginImageInput;
    QImage $m$LastImageInput;

    std::chrono::high_resolution_clock::time_point $m$LastDraw;
    QTimer * $m$Timer = nullptr;
    void deleteThisObject() {
        $m$Timer->stop();
        glDeleteTextures(1, &$m$InputImage);
        glDeleteTextures(1, &$m$OutputImage);
        glDeleteProgram($m$Program);
        delete this;
    }

    DrawData() {
        $m$Timer = sstdNew<QTimer>(this);
    }

    void setProgram() {
        auto varShader = glCreateShader(GL_COMPUTE_SHADER);
        const auto varShaderSource =
            sstd::load_file_remove_utf8(QStringLiteral("myqml/computeshaderfilterimage/main.cps"));

        {
            GLint varSL = static_cast<GLint>(varShaderSource.size());
            const char *varS[]{ varShaderSource.c_str() };
            glShaderSource(varShader, 1, varS, &varSL);
            glCompileShader(varShader);
        }

        if constexpr (false) {
            auto printErrorDetail = [](GLuint e) {
                GLint log_length;
                glGetShaderiv(e, GL_INFO_LOG_LENGTH, &log_length);
                log_length += 16;

                /*获得一段内存，并初始化为0*/
                sstd::string infos_;
                infos_.resize(log_length);

                char * info = infos_.data();
                glGetShaderInfoLog(e, log_length, nullptr, info);
                qDebug() << info;
            };
            printErrorDetail(varShader);
        }

        $m$Program = glCreateProgram();
        glAttachShader($m$Program, varShader);
        glLinkProgram($m$Program);
        glDeleteShader(varShader);
    }

    void setTexture() {

        /*获得图片*/
        $m$OriginImageInput = []() {
            return QImage(QStringLiteral("myqml/computeshaderfilterimage/test.png"))
                .convertToFormat(QImage::Format_RGBA8888);
        }();
        const auto & varImage = $m$OriginImageInput;
        $m$LastImageInput = varImage;

        /*初始化长宽*/
        $m$ImageHeight = varImage.height();
        $m$ImageWidth = varImage.width();

        /*创建Texture*/
        glCreateTextures(GL_TEXTURE_2D, 1, &$m$InputImage);

        /*分配内存*/
        glTextureStorage2D($m$InputImage, 1, GL_RGBA8, $m$ImageWidth, $m$ImageHeight);
        /*上传数据*/
        glTextureSubImage2D($m$InputImage,
            0/*level*/,
            0/*x*/, 0/*y*/,
            $m$ImageWidth, $m$ImageHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            varImage.bits());

        glCreateTextures(GL_TEXTURE_2D, 1, &$m$OutputImage);
        /*分配内存*/
        glTextureStorage2D($m$OutputImage, 1, GL_RGBA8, varImage.width(), varImage.height());

    }

};

void Window::initializeGL() {
    /*************************************************************************/
    //标准开头
    if ($m$DrawData) { return; }
    this->makeCurrent();
    glewInitialize();

    gl_debug_function_lock();

    $m$DrawData = sstdNew<DrawData>();
    connect(context(), &QOpenGLContext::aboutToBeDestroyed,
        $m$DrawData, &DrawData::deleteThisObject, Qt::DirectConnection);
    connect($m$DrawData->$m$Timer, &QTimer::timeout,
        this, [this]() {update(); });
    $m$DrawData->$m$Timer->start(10);
    /*************************************************************************/

    $m$DrawData->setTexture();
    $m$DrawData->setProgram();

}

void Window::paintGL() {

    if (nullptr == $m$DrawData) {
        return;
    }

    if (bool($m$WatcherImageView)==false) {
        return;
    }

    {/*间隔超过100s才执行*/
        auto varCurrentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::abs(varCurrentTime - $m$DrawData->$m$LastDraw) > 500ms) {
            $m$DrawData->$m$LastDraw = varCurrentTime;
        }
        else {
            return;
        }
    }

    /*开启OpenGL调试环境*/
    gl_debug_function_lock();

    {
        const auto & varImage = $m$DrawData->$m$LastImageInput;
        /*上传数据*/
        glTextureSubImage2D($m$DrawData->$m$InputImage,
            0/*level*/,
            0/*x*/, 0/*y*/,
            $m$DrawData->$m$ImageWidth, $m$DrawData->$m$ImageHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            varImage.bits());
    }

    glUseProgram($m$DrawData->$m$Program);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindImageTexture(0, $m$DrawData->$m$InputImage, 0, false, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(1, $m$DrawData->$m$OutputImage, 0, false, 0, GL_WRITE_ONLY, GL_RGBA8UI);
    glDispatchCompute($m$DrawData->$m$ImageWidth, $m$DrawData->$m$ImageHeight, 1);

    /*等待显卡完成*/
    glFinish();

    {/*从显卡获得数据*/
        auto & varImageOutput = $m$DrawData->$m$LastImageInput;
        glGetTextureImage($m$DrawData->$m$OutputImage, 0,
            GL_RGBA, GL_UNSIGNED_BYTE,
            varImageOutput.byteCount(),
            varImageOutput.bits());
        $m$ImageView->showImage($m$DrawData->$m$OriginImageInput, varImageOutput);
    }

}

void Window::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

bool Window::event(QEvent *argEvent)  {

    if (argEvent->type() == QEvent::Close) {
        this->deleteLater();
        argEvent->accept();
        return true;
    }

    return Super::event(argEvent);
}

/***
https://blog.csdn.net/z444_579/article/details/54138504
https://www.cnblogs.com/vertexshader/articles/3022981.html
***/




