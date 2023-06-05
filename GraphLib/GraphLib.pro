QT += core gui widgets

TEMPLATE = lib
DEFINES += GRAPHLIB_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataClasses/nodespawndata.cpp \
    GraphWidgets/Abstracts/abstractpin.cpp \
    GraphWidgets/Abstracts/basenode.cpp \
    NodeFactory/nodefactory.cpp \
    NodeFactory/nodefactorywidget.cpp \
    GraphWidgets/canvas.cpp \
    NodeFactory/typednodeimage.cpp \
    TypeManagers/nodetypemanager.cpp \
    GraphWidgets/pin.cpp \
    DataClasses/pindata.cpp \
    DataClasses/pindragsignal.cpp \
    TypeManagers/pintypemanager.cpp \
    GraphWidgets/typednode.cpp \
    utility.cpp

HEADERS += \
    DataClasses/nodespawndata.h \
    GraphLib.h \
    GraphLib_global.h \
    GraphWidgets/Abstracts/abstractpin.h \
    GraphWidgets/Abstracts/basenode.h \
    NodeFactory/nodefactory.h \
    NodeFactory/nodefactorywidget.h \
    GraphWidgets/canvas.h \
    NodeFactory/typednodeimage.h \
    constants.h \
    TypeManagers/nodetypemanager.h \
    GraphWidgets/pin.h \
    DataClasses/pindata.h \
    DataClasses/pindragsignal.h \
    TypeManagers/pintypemanager.h \
    GraphWidgets/typednode.h \
    utility.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
