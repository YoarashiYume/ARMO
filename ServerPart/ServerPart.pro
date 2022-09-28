QT += network
QT += widgets
QT += core
QT += gui


TARGET = ServerPart
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    worker.cpp

HEADERS += \
    Packet.h \
    server.h \
    worker.h

