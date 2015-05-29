TEMPLATE = lib
TARGET = qdailymotionplugin
CONFIG += qt plugin
LIBS += -L../lib -lqdailymotion

lessThan(QT_MAJOR_VERSION, 5) {
    QT += declarative
} else {
    QT += qml
}

INCLUDEPATH += ../src

HEADERS += \
    plugin.h

SOURCES += \
    plugin.cpp
    
contains(QDAILYMOTION_STATIC_LIBRARY) {
    CONFIG += link_pkgconfig
} else {
    CONFIG += link_prl
    PKGCONFIG = libqdailymotion
}

qml.files = qmldir

!isEmpty(INSTALL_QML_PREFIX) {
    qml.path = $$INSTALL_QML_PREFIX/lib/qt4/imports/QDailymotion
    target.path = $$INSTALL_QML_PREFIX/lib/qt4/imports/QDailymotion
} else:lessThan(QT_MAJOR_VERSION, 5) {
    qml.path = $$[QT_INSTALL_IMPORTS]/QDailymotion
    target.path = $$[QT_INSTALL_IMPORTS]/QDailymotion
} else {
    qml.path = $$[QT_INSTALL_QML]/QDailymotion
    target.path = $$[QT_INSTALL_QML]/QDailymotion
}

INSTALLS += qml target
