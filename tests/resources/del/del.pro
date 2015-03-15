TEMPLATE = app
TARGET = resources-del
INSTALLS += target

INCLUDEPATH += ../../../src
LIBS += -L../../../lib -lqdailymotion
SOURCES += main.cpp

unix {
    target.path = /opt/qdailymotion/bin
}
