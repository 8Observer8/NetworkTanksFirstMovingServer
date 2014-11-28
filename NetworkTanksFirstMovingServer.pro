#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T14:38:49
#
#-------------------------------------------------

QT       += core network

QT       += gui

TARGET = NetworkTanksFirstMovingServer
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    TankServer.cpp \
    TankClient.cpp

HEADERS += \
    TankServer.h \
    TankClient.h
