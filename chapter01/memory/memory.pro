
CONFIG(debug,debug|release){
    TARGET =   memory_debug
}else{
    TARGET =   memory
}


TEMPLATE = app

QT -= gui
QT -= qml
QT -= core
QT -= quick
QT -= widgets

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/test_intrusive_ptr.cpp

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

include($$PWD/this/this.pri)

CONFIG(debug,debug|release){
    LIBS += -L$$RootDestDir -lsstd_core_libraryd
    QMAKE_POST_LINK += $$DESTDIR/buildinstall_debug $$PWD "myqml"
    export(QMAKE_POST_LINK)
}else{
    LIBS += -L$$RootDestDir -lsstd_core_library
    QMAKE_POST_LINK += $$DESTDIR/buildinstall $$PWD "myqml"
    export(QMAKE_POST_LINK)
}


