#-------------------------------------------------
#
# Project created by QtCreator 2017-03-13T09:40:24
#
#-------------------------------------------------

QT       += core gui network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server_morpion
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    serv.cpp \
    mythread.cpp

HEADERS  += \
    serv.h \
    mythread.h \
    structure.h

FORMS    += mainwindow.ui

# 76 69 63 74 6f 72 20 70 6c 6f 75 68 69 6e 65 63
# dmljdG9yIHBsb3VoaW5lYw==
