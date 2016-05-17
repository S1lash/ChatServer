#-------------------------------------------------
#
# Project created by QtCreator 2016-05-14T01:29:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatServer
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    server.cpp \
    client.cpp

HEADERS  += dialog.h \
    server.h \
    client.h

FORMS    += dialog.ui
