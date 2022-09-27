QT += network
QT += widgets
QT += core
CONFIG += c++11

TARGET = ClientPart
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    client.cpp

HEADERS += \
    client.h \
    Packet.h

