# 
# otlib - order type library
# (c) Manfred Scheucher 2018
#  

TARGET = test_universal_sets
CONFIG   += console
CONFIG   +=  c++11
CONFIG   -= app_bundle
QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

QT       -= core
QT       -= gui

OTLIBDIR = ../..
INCLUDEPATH += $$OTLIBDIR 

INCLUDEPATH += $$OTLIBDIR/minisat-2.2.0
LIBS += $$OTLIBDIR/minisat-2.2.0/simp/lib_release.a

SOURCES += \
            test_universal_sets.cpp \
            $$OTLIBDIR/SmallLambda.cpp \
            $$OTLIBDIR/BigLambda.cpp \

HEADERS += \
            $$OTLIBDIR/BigLambda.h \
            $$OTLIBDIR/defines.h \
            $$OTLIBDIR/PointSet.h \
            $$OTLIBDIR/PointSetBinaryReader.h \
            $$OTLIBDIR/SmallLambda.h \
            $$OTLIBDIR/SmallLambdaBinaryReader.h \
            $$OTLIBDIR/SmallLambdaBinaryWriter.h \
