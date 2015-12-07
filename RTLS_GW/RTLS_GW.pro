#-------------------------------------------------
#
# Project created by QtCreator 2015-08-03T10:14:46
#
#-------------------------------------------------

QT       += core
QT       += network xml


TARGET = RTLS_GW
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    rtls_gw.cpp \
    m_anchor.cpp \
    s_anchor.cpp \
    cs_kalman.cpp \
    multilateration.cpp \
    net_if.cpp \
    tag_tracking.cpp \
    aes.cpp \
    aeshelper.cpp \
    hw_id.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/armadillo-3.930.0/lib/ -lblas_win32_MT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/armadillo-3.930.0/lib/ -lblas_win32_MT
else:unix: LIBS += -L$$PWD/armadillo-3.930.0/lib/ -lblas_win32_MT

INCLUDEPATH += $$PWD/armadillo-3.930.0/include
DEPENDPATH += $$PWD/armadillo-3.930.0/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/armadillo-3.930.0/lib/ -llapack_win32_MT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/armadillo-3.930.0/lib/ -llapack_win32_MT
else:unix: LIBS += -L$$PWD/armadillo-3.930.0/lib/ -llapack_win32_MT

INCLUDEPATH += $$PWD/armadillo-3.930.0/include
DEPENDPATH += $$PWD/armadillo-3.930.0/include

HEADERS += \
    rtls_gw.h \
    m_anchor.h \
    s_anchor.h \
    cs_kalman.h \
    msg_config.h \
    mutilateration.h \
    multilateration.h \
    net_if.h \
    tag_tracking.h \
    hw_rnc.h \
    aes.h \
    aeshelper.h \
    hw_id.h
