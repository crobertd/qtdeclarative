CONFIG += testcase
TARGET = tst_qquickflickable
macx:CONFIG -= app_bundle

SOURCES += tst_qquickflickable.cpp

include (../../shared/util.pri)
include (../shared/util.pri)

TESTDATA = data/*

QT += core-private gui-private v8-private qml-private quick-private testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
