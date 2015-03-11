TEMPLATE = lib
TARGET = qdailymotionplugin
CONFIG += qt plugin link_prl
LIBS += -L../lib -lqdailymotion
PKGCONFIG = libqdailymotion
INCLUDEPATH += ../src

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
} lessThan(QT_MAJOR_VERSION, 5) {
    qml.path = $$[QT_INSTALL_IMPORTS]/QDailymotion
    target.path = $$[QT_INSTALL_IMPORTS]/QDailymotion
} else {
    qml.path = $$[QT_INSTALL_QML]/QDailymotion
    target.path = $$[QT_INSTALL_QML]/QDailymotion
}

INSTALLS += qml target
