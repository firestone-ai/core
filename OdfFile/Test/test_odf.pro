QT       -= core
QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += core_static_link_libstd

CORE_ROOT_DIR = $$PWD/../../../core
CORE_3DPARTY_DIR = $$CORE_ROOT_DIR/Common/3dParty
PWD_ROOT_DIR = $$PWD

include($$CORE_ROOT_DIR/Common/base.pri)
include($$CORE_3DPARTY_DIR/googletest/googletest.pri)
include($$CORE_3DPARTY_DIR/boost/boost.pri)
include($$CORE_ROOT_DIR/Common/3dParty/icu/icu.pri)

DESTDIR = $$PWD/build

INCLUDEPATH += ../
INCLUDEPATH += $$CORE_ROOT_DIR/OdfFile/Common/

LIBS += -L$$CORE_BUILDS_LIBRARIES_PATH -lCryptoPPLib
LIBS += -L$$CORE_BOOST_LIBS

ADD_DEPENDENCY(kernel)
ADD_DEPENDENCY(graphics)
ADD_DEPENDENCY(DocxFormatLib)
ADD_DEPENDENCY(OdfFormatLib)

core_linux {
    LIBS += -Wl,-unresolved-symbols=ignore-in-shared-libs
    LIBS += -ldl
}

HEADERS += \
    common.h\
    entrance.h

SOURCES += \
    test.cpp\
    common.cpp\
    entrance.cpp\
    $$CORE_ROOT_DIR/Common/OfficeFileFormatChecker2.cpp

SOURCES -= $$CORE_GTEST_PATH/src/gtest_main.cc
