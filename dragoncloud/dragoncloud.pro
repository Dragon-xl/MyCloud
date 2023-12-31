QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    des.c \
    funtion.cpp \
    login.cpp \
    logintitlewg.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    common/common.h \
    common/des.h \
    common/logininfoinstance.h \
    des.h \
    funtion.h \
    login.h \
    logintitlewg.h \
    mainwindow.h

FORMS += \
    login.ui \
    logintitlewg.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qrc.qrc
