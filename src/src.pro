TEMPLATE = lib

#DEFINES += QDAILYMOTION_DEBUG

#unix:INSTALL_SRC_PREFIX = /opt

QT += network
QT -= gui

TARGET = qdailymotion
DESTDIR = ../lib

contains(MEEGO_EDITION,harmattan) {
    CONFIG += staticlib
    DEFINES += QDAILYMOTION_STATIC_LIBRARY
} else {
    CONFIG += create_prl
    DEFINES += QDAILYMOTION_LIBRARY
}

HEADERS += \
    authenticationrequest.h \
    json.h \
    model.h \
    model_p.h \
    request.h \
    request_p.h \
    resourcesmodel.h \
    resourcesrequest.h \
    streamsmodel.h \
    streamsrequest.h \
    urls.h

SOURCES += \
    authenticationrequest.cpp \
    json.cpp \
    model.cpp \
    request.cpp \
    resourcesmodel.cpp \
    resourcesrequest.cpp \
    streamsmodel.cpp \
    streamsrequest.cpp
    
headers.files += \
    authenticationrequest.h \
    model.h \
    request.h \
    resourcesmodel.h \
    resourcesrequest.h \
    streamsmodel.h \
    streamsrequest.h \
    urls.h

!isEmpty(INSTALL_SRC_PREFIX) {
    target.path = $$INSTALL_SRC_PREFIX/lib
    headers.path = $$INSTALL_SRC_PREFIX/include/qdailymotion
} else {
    target.path = /usr/lib
    headers.path = /usr/include/qdailymotion
}

INSTALLS += target headers
