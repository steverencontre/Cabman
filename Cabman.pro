#-------------------------------------------------
#
# Project created by QtCreator 2017-04-20T14:56:34
#
#-------------------------------------------------

QT       += core gui widgets webenginewidgets network networkauth

#CONFIG += c++20
QMAKE_CXXFLAGS += -std=c++20

TARGET = Cabman
TEMPLATE = app


SOURCES += main.cpp\
    CixModel.cpp \
    CixNetworkIo.cpp \
    MainWindow.cpp \
    Settings.cpp

HEADERS  += MainWindow.h \
  CixModel.h \
  CixNetworkIo.h \
  Settings.h

