QT += core gui widgets

TEMPLATE = lib
DEFINES += GRAPHLIB_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Abstracts/abstractpin.cpp \
    Abstracts/basenode.cpp \
    NodeFactory/nodefactorywidget.cpp \
    canvas.cpp \
    TypeManagers/nodetypemanager.cpp \
    pin.cpp \
    DataClasses/pindata.cpp \
    DataClasses/pindragsignal.cpp \
    TypeManagers/pintypemanager.cpp \
    typednode.cpp \
    utility.cpp

HEADERS += \
    GraphLib.h \
    GraphLib_global.h \
    Abstracts/abstractpin.h \
    Abstracts/basenode.h \
    NodeFactory/nodefactorywidget.h \
    canvas.h \
    constants.h \
    TypeManagers/nodetypemanager.h \
    pin.h \
    DataClasses/pindata.h \
    DataClasses/pindragsignal.h \
    TypeManagers/pintypemanager.h \
    typednode.h \
    utility.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
