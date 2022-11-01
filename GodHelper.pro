#-------------------------------------------------
#
# Project created by QtCreator 2018-04-17T16:02:14
#
#-------------------------------------------------

QT       += core gui network serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GodHelper
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

FORMS += \
    mainwindow.ui

HEADERS += \
    Bin/header.h \
    Bin/param.h \
    Bin/typedef.h \
    Component/imgshowcomponent.h \
    Component/UdpComponent.h \
    Core/consolecenter.h \
    Core/datacenter.h \
    Core/FileHandle.h \
    Core/openglwidget.h \
    Core/recordcenter.hpp \
    Core/recordlist.hpp \
    ImgProc/ImgProc.h \
    ImgProc/ImgProcCore.h \
    ImgProc/imgprocdisplay.h \
    mainwindow.h \
    Library/CheckButton.h \
    Library/ParamLine.h


SOURCES += \
    Component/imgshowcomponent.cpp \
    Component/UdpComponent.cpp \
    Core/consolecenter.cpp \
    Core/datacenter.cpp \
    Core/FileHandle.cpp \
    Core/openglwidget.cpp \
    Core/recordcenter.cpp \
    Core/recordlist.cpp \
    ImgProc/ImgProc.cpp \
    ImgProc/ImgProcCore.cpp \
    ImgProc/imgprocdisplay.cpp \
    main.cpp \
    mainwindow.cpp \
    Library/CheckButton.cpp \
    Library/ParamLine.cpp




