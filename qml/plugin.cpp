#include "plugin.h"
#include "../src/authenticationrequest.h"
#include "../src/resourcesmodel.h"
#include "../src/streamsmodel.h"
#if QT_VERSION >= 0x050000
#include <qqml.h>
#else
#include <qdeclarative.h>
#endif

namespace QDailymotion {

void Plugin::registerTypes(const char *uri) {
    Q_ASSERT(uri == QLatin1String("QDailymotion"));

    qmlRegisterType<AuthenticationRequest>(uri, 1, 0, "AuthenticationRequest");
    qmlRegisterType<ResourcesModel>(uri, 1, 0, "ResourcesModel");
    qmlRegisterType<ResourcesRequest>(uri, 1, 0, "ResourcesRequest");
    qmlRegisterType<StreamsModel>(uri, 1, 0, "StreamsModel");
    qmlRegisterType<StreamsRequest>(uri, 1, 0, "StreamsRequest");
}

}

QML_DECLARE_TYPE(QDailymotion::AuthenticationRequest)
QML_DECLARE_TYPE(QDailymotion::ResourcesModel)
QML_DECLARE_TYPE(QDailymotion::ResourcesRequest)
QML_DECLARE_TYPE(QDailymotion::StreamsModel)
QML_DECLARE_TYPE(QDailymotion::StreamsRequest)
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qdailymotionplugin, QDailymotion::Plugin)
#endif
