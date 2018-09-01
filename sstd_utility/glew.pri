DEFINES *= GLEW_NO_GLU
DEFINES *= GLEW_STATIC

win32:{
LIBS+= -lopengl32 -lgdi32 -luser32 -lkernel32
}

linux:{
###libglu1-mesa-dev
###libgl1-mesa-dev
LIBS+=-lXmu -lXi -lGL -lXext -lX11
}

SOURCES += $$PWD/glew_source/glew.c
INCLUDEPATH += $$PWD/glew_source/glew
HEADERS += $$PWD/glew_source/glew/GL/glew.h
HEADERS += $$PWD/glew_source/glew/GL/eglew.h
HEADERS += $$PWD/glew_source/glew/GL/glxew.h
HEADERS += $$PWD/glew_source/glew/GL/wglew.h











