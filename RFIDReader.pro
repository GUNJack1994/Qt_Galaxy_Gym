#-------------------------------------------------
#
# Project created by QtCreator 2015-01-12T18:20:19
#
#-------------------------------------------------

QT += core gui sql serialport multimedia multimediawidgets quick

#CONFIG += console

#DEFINES += SIMULATOR

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RFIDReader
TEMPLATE = app

QTPLUGIN += QSQLMYSQL

SOURCES += main.cpp \
        mainwindow.cpp \
    rfidmanager.cpp \
    userform.cpp \
    basemanager.cpp \
    databasesettingswidget.cpp \
    userobject.cpp \
    serialportsettingswidget.cpp \
    camerasettingswidget.cpp \
    takephotowidget.cpp \
    findwidget.cpp \
    lastentrancewidget.cpp \
    rowfilter.cpp \
    centerwidget.cpp \
    payment.cpp \
    worker.cpp \
    carnettype.cpp \
    simulatorrfid.cpp \
    userswidget.cpp \
    statuscontrolswidget.cpp \
    entrancedelegate.cpp \
    lastentrancemodel.cpp \
    paymentdelegate.cpp \
    worktimewidget.cpp \
    photodelegator.cpp \
    saleswidget.cpp \
    querythread.cpp \
    photodownloader.cpp \
    finduserdelegate.cpp \
    socketserver.cpp \
    groupedentrancewidget.cpp \
    application.cpp \
    keyform.cpp


HEADERS  += mainwindow.h \
    rfidmanager.h \
    userform.h \
    basemanager.h \
    databasesettingswidget.h \
    userobject.h \
    serialportsettingswidget.h \
    camerasettingswidget.h \
    takephotowidget.h \
    findwidget.h \
    lastentrancewidget.h \
    rowfilter.h \
    centerwidget.h \
    payment.h \
    worker.h \
    carnettype.h \
    simulatorrfid.h \
    userswidget.h \
    statuscontrolswidget.h \
    entrancedelegate.h \
    lastentrancemodel.h \
    paymentdelegate.h \
    defines.h \
    worktimewidget.h \
    photodelegator.h \
    saleswidget.h \
    querythread.h \
    photodownloader.h \
    finduserdelegate.h \
    socketserver.h \
    groupedentrancewidget.h \
    application.h \
    keyform.h

FORMS    += mainwindow.ui \
    userform.ui \
    databasesettingswidget.ui \
    serialportsettingswidget.ui \
    camerasettingswidget.ui \
    takephotowidget.ui \
    findwidget.ui \
    lastentrancewidget.ui \
    centerwidget.ui \
    simulatorrfid.ui \
    userswidget.ui \
    statuscontrolswidget.ui \
    worktimewidget.ui \
    saleswidget.ui \
    groupedentrancewidget.ui \
    keyform.ui

RESOURCES += \
    resources.qrc \
    guiIconsResources.qrc

OTHER_FILES += \
    CenterView.qml

TRANSLATIONS = lang_pl.ts
