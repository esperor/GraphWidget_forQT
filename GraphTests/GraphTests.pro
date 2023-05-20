include(gtest_dependency.pri)

QT += core gui widgets

TEMPLATE = app
CONFIG += console c++latest
CONFIG -= app_bundle
CONFIG += thread

SOURCES += \
        main.cpp  \
        tst_maincase.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GraphLib/release/ -lGraphLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GraphLib/debug/ -lGraphLib
else:unix: LIBS += -L$$OUT_PWD/../GraphLib/ -lGraphLib

INCLUDEPATH += $$PWD/../GraphLib
DEPENDPATH += $$PWD/../GraphLib
