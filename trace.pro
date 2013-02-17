TEMPLATE = app
TARGET = trace
CONFIG -= qt

QT =

PROJECT_DEFINES = FLTK1 SAMPLE_SOLUTION SAMPLE_SOLUTION_557 SAMPLE_SOLUTION_P557 cimg_display_type=0 GLEW_STATIC


HEADERS += src/fileio/imageio.h \
    src/fileio/CImg.h \
    src/fileio/buffer.h \
    src/parser/Tokenizer.h \
    src/parser/Token.h \
    src/parser/ParserException.h \
    src/parser/Parser.h \
    src/scene/scene.h \
    src/scene/ray.h \
    src/scene/material.h \
    src/scene/light.h \
    src/scene/camera.h \
    src/scene/BSPTree.h \
    src/SceneObjects/trimesh.h \
    src/SceneObjects/Square.h \
    src/SceneObjects/Sphere.h \
    src/SceneObjects/Cylinder.h \
    src/SceneObjects/Cone.h \
    src/SceneObjects/Box.h \
    src/threads/ThreadPool.h \
    src/ui/TraceUI.h \
    src/ui/TraceGLWindow.h \
    src/ui/ModelerCamera.h \
    src/ui/GraphicalUI.h \
    src/ui/dialog.h \
    src/ui/debuggingWindow.h \
    src/ui/debuggingView.h \
    src/ui/CommandLineUI.h \
    src/vecmath/vec.h \
    src/vecmath/mat.h \
    src/RayTracer.h \
    src/getopt.h \
    src/general.h
SOURCES += src/fileio/"file dialog"/Fl_Native_File_Chooser.cxx \
    src/fileio/"file dialog"/common.cxx \
    src/fileio/imageio.cpp \
    src/fileio/buffer.cpp \
    src/parser/Tokenizer.cpp \
    src/parser/Token.cpp \
    src/parser/ParserException.cpp \
    src/parser/Parser.cpp \
    src/scene/scene.cpp \
    src/scene/ray.cpp \
    src/scene/material.cpp \
    src/scene/light.cpp \
    src/scene/camera.cpp \
    src/SceneObjects/trimesh.cpp \
    src/SceneObjects/Square.cpp \
    src/SceneObjects/Sphere.cpp \
    src/SceneObjects/Cylinder.cpp \
    src/SceneObjects/Cone.cpp \
    src/SceneObjects/Box.cpp \
    src/threads/ThreadPool.cpp \
    src/ui/TraceGLWindow.cpp \
    src/ui/ModelerCamera.cpp \
    src/ui/GraphicalUI.cpp \
    src/ui/glObjects.cpp \
    src/ui/debuggingWindow.cxx \
    src/ui/debuggingView.cpp \
    src/ui/CommandLineUI.cpp \
    src/RayTracer.cpp \
    src/main.cpp

    
    
win32 {

SOURCES += src/getopt.cpp

INCLUDEPATH += ../FLTK \
    ../FLTK/png \
    ../FLTK/zlib \
    ../FLTK/jpeg \
    src/fileio/"file dialog"

LIBPATH += ../FLTK/lib

LIBS = opengl32.lib \
    glu32.lib \
    -lfltk \
    -lfltkgl \
    -lfltkjpeg \
    -lfltkpng \
    -lfltkz \
    Comctl32.lib \
    wsock32.lib \
    kernel32.lib \
    user32.lib \
    gdi32.lib \
    comdlg32.lib \
    shell32.lib \
    ole32.lib \
    oleaut32.lib
 
# set DEFINES = to get rid of UNICODE
#
DEFINES = _CRT_SECURE_NO_WARNINGS WIN32 $$PROJECT_DEFINES
    

QMAKE_CXXFLAGS = -Oi \
    -EHsc \
    -nologo \
    -Zm200

QMAKE_CFLAGS = -nologo \
    -Zm200

}

mac { 

INCLUDEPATH += ../FLTK \
    ../FLTK/png \
    ../FLTK/zlib \
    ../FLTK/jpeg \
    src/fileio/"file dialog"

CONFIG += x86

CONFIG += console
CONFIG -= app_bundle

DEFINES += $$PROJECT_DEFINES
     
LIBS += -L../FLTK/lib \
        -lfltk \
        -lfltk_gl \
        -lfltk_jpeg \
        -lfltk_png \
        -lfltk_images \
        -lfltk_z \
        -framework \
        Carbon \
        -framework \
        ApplicationServices \
        -framework \
        AGL \
        -framework \
        OpenGL \
        -framework \
        AppKit
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
}

linux-g++ {

INCLUDEPATH += ../FLTK/include/FL/images src/fileio/"file dialog" ../glew/include
DEFINES +=  __LINUX__ $$PROJECT_DEFINES
LIBS += -L../FLTK/lib -lfltk_images -lfltk -lfltk_gl -lfltk_jpeg -lfltk_png  -lfltk_forms -lfltk_z -lXext -ldl -lX11 -lm -lGL -lGLU -L../glew -lglews
}

OTHER_FILES += src/ui/debuggingWindow.fl
