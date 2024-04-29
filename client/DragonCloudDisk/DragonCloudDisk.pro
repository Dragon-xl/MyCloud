QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common.cpp \
    dataprogress.cpp \
    des.c \
    downloadlayout.cpp \
    downloadtask.cpp \
    foldername.cpp \
    form.cpp \
    login.cpp \
    logininfo.cpp \
    logintitle.cpp \
    main.cpp \
    maintitle.cpp \
    mainwindow.cpp \
    myfilewg.cpp \
    mymenu.cpp \
    property.cpp \
    rankinglist.cpp \
    removeconfirm.cpp \
    selectfolder.cpp \
    sharelist.cpp \
    uploadlayout.cpp \
    uploadtask.cpp

HEADERS += \
    common.h \
    dataprogress.h \
    des.h \
    downloadlayout.h \
    downloadtask.h \
    foldername.h \
    form.h \
    login.h \
    logininfo.h \
    logintitle.h \
    maintitle.h \
    mainwindow.h \
    myfilewg.h \
    mymenu.h \
    property.h \
    rankinglist.h \
    removeconfirm.h \
    selectfolder.h \
    sharelist.h \
    uploadlayout.h \
    uploadtask.h

FORMS += \
    dataprogress.ui \
    foldername.ui \
    form.ui \
    login.ui \
    logintitle.ui \
    maintitle.ui \
    mainwindow.ui \
    myfilewg.ui \
    property.ui \
    rankinglist.ui \
    removeconfirm.ui \
    selectfolder.ui \
    sharelist.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    conf/cfg.json
