CONFIG += testcase
TARGET = tst_qquickimage
macx:CONFIG -= app_bundle

HEADERS += ../shared/testhttpserver.h
SOURCES += tst_qquickimage.cpp ../shared/testhttpserver.cpp

testDataFiles.files = data
testDataFiles.path = .
DEPLOYMENT += testDataFiles

CONFIG += parallel_test
QT += core-private gui-private declarative-private network testlib