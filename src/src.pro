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
    activitiesmodel.h \
    activitiesrequest.h \
    authenticationrequest.h \
    channelsmodel.h \
    channelsrequest.h \
    commentsmodel.h \
    commentsrequest.h \
    contestsmodel.h \
    contestsrequest.h \
    groupsmodel.h \
    groupsrequest.h \
    json.h \
    localesmodel.h \
    localesrequest.h \
    model.h \
    model_p.h \
    playlistsmodel.h \
    playlistsrequest.h \
    reportsmodel.h \
    reportsrequest.h \
    request.h \
    request_p.h \
    streamsmodel.h \
    streamsrequest.h \
    strongtagsmodel.h \
    strongtagsrequest.h \
    subtitlesmodel.h \
    subtitlesrequest.h \
    usersmodel.h \
    usersrequest.h \
    videosmodel.h \
    videosrequest.h \
    urls.h

SOURCES += \
    activitiesmodel.cpp \
    activitiesrequest.cpp \
    authenticationrequest.cpp \
    channelsmodel.cpp \
    channelsrequest.cpp \
    commentsmodel.cpp \
    commentsrequest.cpp \
    contestsmodel.cpp \
    contestsrequest.cpp \
    groupsmodel.cpp \
    groupsrequest.cpp \
    json.cpp \
    localesmodel.cpp \
    localesrequest.cpp \
    model.cpp \
    playlistsmodel.cpp \
    playlistsrequest.cpp \
    reportsmodel.cpp \
    reportsrequest.cpp \
    request.cpp \
    streamsmodel.cpp \
    streamsrequest.cpp \
    strongtagsmodel.cpp \
    strongtagsrequest.cpp \
    subtitlesmodel.cpp \
    subtitlesrequest.cpp \
    usersmodel.cpp \
    usersrequest.cpp \
    videosmodel.cpp \
    videosrequest.cpp
    
headers.files += \
    activitiesmodel.h \
    activitiesrequest.h \
    authenticationrequest.h \
    channelsmodel.h \
    channelsrequest.h \
    commentsmodel.h \
    commentsrequest.h \
    contestsmodel.h \
    contestsrequest.h \
    groupsmodel.h \
    groupsrequest.h \
    localesmodel.h \
    localesrequest.h \
    model.h \
    playlistsmodel.h \
    playlistsrequest.h \
    reportsmodel.h \
    reportsrequest.h \
    request.h \
    streamsmodel.h \
    streamsrequest.h \
    strongtagsmodel.h \
    strongtagsrequest.h \
    subtitlesmodel.h \
    subtitlesrequest.h \
    usersmodel.h \
    usersrequest.h \
    videosmodel.h \
    videosrequest.h \
    urls.h

!isEmpty(INSTALL_SRC_PREFIX) {
    target.path = $$INSTALL_SRC_PREFIX/lib
    headers.path = $$INSTALL_SRC_PREFIX/include/qdailymotion
} else {
    target.path = /usr/lib
    headers.path = /usr/include/qdailymotion
}

INSTALLS += target headers
