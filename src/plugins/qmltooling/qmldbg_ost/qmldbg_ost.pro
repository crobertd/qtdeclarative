TARGET = qmldbg_ost
QT       += qml network

PLUGIN_TYPE = qmltooling
load(qt_plugin)

SOURCES += \
    qmlostplugin.cpp \
    qostdevice.cpp

HEADERS += \
    qmlostplugin.h \
    qostdevice.h \
    usbostcomm.h
