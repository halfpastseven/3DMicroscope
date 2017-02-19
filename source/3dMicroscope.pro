#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T10:34:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3dMicroscope
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagewidget.cpp \
    analyzer.cpp \
    thumbnailseries.cpp

HEADERS  += mainwindow.h \
    imagewidget.h \
    analyzer.h \
    thumbnailseries.h

RESOURCES += icons.qrc
win32:RC_ICONS = icon/3DMicroscope.ico
