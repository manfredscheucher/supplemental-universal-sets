# 
# otlib - order type library
# (c) Manfred Scheucher 2018
#  

TARGET = extend_order_type
CONFIG   += console
CONFIG   +=  c++11
CONFIG   -= app_bundle
QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

QT       -= core
QT       -= gui

OTLIBDIR = ../..
INCLUDEPATH += $$OTLIBDIR 

SOURCES += \
            extend_order_type.cpp \
            $$OTLIBDIR/SmallLambda.cpp \
            $$OTLIBDIR/BigLambda.cpp \

HEADERS += \
            $$OTLIBDIR/BigLambda.h \
            $$OTLIBDIR/defines.h \
            $$OTLIBDIR/SmallLambda.h \
            $$OTLIBDIR/SmallLambdaBinaryReader.h \
            $$OTLIBDIR/SmallLambdaBinaryWriter.h \
