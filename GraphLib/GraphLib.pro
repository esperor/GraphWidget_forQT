QT += core gui widgets

TEMPLATE = lib
DEFINES += GRAPHLIB_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abstractpin.cpp \
    canvas.cpp \
    node.cpp \
    pin.cpp \
    pindata.cpp \
    pindragsignal.cpp \
    util_functions.cpp

HEADERS += \
    GraphLib_global.h \
    abstractpin.h \
    canvas.h \
    constants.h \
    node.h \
    pin.h \
    pindata.h \
    pindragsignal.h \
    util_functions.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
