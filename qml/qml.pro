TEMPLATE = lib
TARGET = qdailymotionplugin
CONFIG += qt plugin
LIBS += -L../lib -lqdailymotion

contains(MEEGO_EDITION,harmattan) {
    CONFIG += link_pkgconfig
    INSTALL_QML_PREFIX = /usr
} else {
    CONFIG += link_prl
    PKGCONFIG = libqdailymotion
}

lessThan(QT_MAJOR_VERSION, 5) {
    QT += declarative
} else {
    QT += qml
}

HEADERS += \
    plugin.h

SOURCES += \
    plugin.cpp

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
