#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T00:28:36
#
#-------------------------------------------------

unix:!macx: LIBS += -L$$PWD/3rdparty/taglib-1.9.1/taglib/ -ltag
INCLUDEPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers
DEPENDPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers


QT       += core gui
QT       += multimedia

CONFIG   += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        playlist.cpp \
        track.cpp

HEADERS  += mainwindow.h \
            playlist.h \
            track.h

FORMS    += mainwindow.ui
