﻿#include <sstd_glew.hpp>
#include <QtGui/qcolor.h>
#include "RootWindow.hpp"
#include "RenderSize.hpp"
#include "RenderThread.hpp"
#include <ConstructQSurface.hpp>
#include <QtCore/qdebug.h>

RootWindow::RootWindow() {
    /*用于析构的时候与Render同步*/
    mmm_Mutex = sstd::make_shared< MutexPointer::element_type >();
    /*创建OpenGL渲染环境*/
    this->setSurfaceType(QSurface::OpenGLSurface);
    this->setFormat(sstd::getDefaultOpenGLFormat());
    /**/
    this->setTitle(trUtf8(u8R"(Root Window)"));
    /*固定窗口大小*/
    this->setMinimumSize({ getRenderWidth(),getRenderHeight() });
    this->setMaximumSize({ getRenderWidth(),getRenderHeight() });

}

void RootWindow::openglDraw() {
    if (this->mmm_IsRending) {
        qDebug() << QStringLiteral( "last render is not finished , this ,rend ignored!" );
        return;
    }
    /*the thread will delete itself when finished*/
    auto varThread = sstdNew<sstd::RenderThread>(this) ;
    connect(varThread, &QThread::finished, this, [this]() { 
        this->mmm_IsRending = false; 
    }, Qt::QueuedConnection);
    this->mmm_IsRending = true;
    varThread->start();
}

void RootWindow::initialize() {
    if (getContex()) {
        return openglDraw();
    }
    mmm_Contex = sstdNew< QOpenGLContext >();
    mmm_Contex->setFormat(this->requestedFormat());
    mmm_Contex->create();
    mmm_Contex->makeCurrent(this);
    glewInitialize();
    return openglDraw();
}

void RootWindow::exposeEvent(QExposeEvent *event)   {
    this->initialize();
    (void)event;
}

RootWindow::~RootWindow() {

    auto varMutex = std::move(mmm_Mutex);
    varMutex->setDestory();

    /*rending need this data , so wait for rending finished*/
    for (;;) {
        if ( varMutex->renderCount()>0 ) {
            std::this_thread::sleep_for(10ns);
        } else {
            break;
        }
    }

    /*destory the contex*/
    if (this->getContex()) {
        this->getContex()->deleteLater();
    }

}

bool RootWindow::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::UpdateRequest:
            this->initialize();
            return true;
        default:
            return QWindow::event(event);
    }
}

/**
http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
**/
