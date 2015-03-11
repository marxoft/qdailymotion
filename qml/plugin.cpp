#include "plugin.h"
#include "activitiesmodel.h"
#include "authenticationrequest.h"
#include "channelsmodel.h"
#include "commentsmodel.h"
#include "contestsmodel.h"
#include "groupsmodel.h"
#include "localesmodel.h"
#include "playlistsmodel.h"
#include "reportsmodel.h"
#include "streamsmodel.h"
#include "strongtagsmodel.h"
#include "subtitlesmodel.h"
#include "usersmodel.h"
#include "videosmodel.h"
#if QT_VERSION >= 0x050000
#include <qqml.h>
#else
#include <qdeclarative.h>
#endif

namespace QDailymotion {

void Plugin::registerTypes(const char *uri) {
    Q_ASSERT(uri == QLatin1String("QDailymotion"));

    qmlRegisterType<ActivitiesModel>(uri, 1, 0, "ActivitiesModel");
    qmlRegisterType<ActivitiesRequest>(uri, 1, 0, "ActivitiesRequest");
    qmlRegisterType<AuthenticationRequest>(uri, 1, 0, "AuthenticationRequest");
    qmlRegisterType<ChannelsModel>(uri, 1, 0, "ChannelsModel");
    qmlRegisterType<ChannelsRequest>(uri, 1, 0, "ChannelsRequest");
    qmlRegisterType<CommentsModel>(uri, 1, 0, "CommentsModel");
    qmlRegisterType<CommentsRequest>(uri, 1, 0, "CommentsRequest");
    qmlRegisterType<ContestsModel>(uri, 1, 0, "ContestsModel");
    qmlRegisterType<ContestsRequest>(uri, 1, 0, "ContestsRequest");
    qmlRegisterType<GroupsModel>(uri, 1, 0, "GroupsModel");
    qmlRegisterType<GroupsRequest>(uri, 1, 0, "GroupsRequest");
    qmlRegisterType<LocalesModel>(uri, 1, 0, "LocalesModel");
    qmlRegisterType<LocalesRequest>(uri, 1, 0, "LocalesRequest");
    qmlRegisterType<PlaylistsModel>(uri, 1, 0, "PlaylistsModel");
    qmlRegisterType<PlaylistsRequest>(uri, 1, 0, "PlaylistsRequest");
    qmlRegisterType<ReportsModel>(uri, 1, 0, "ReportsModel");
    qmlRegisterType<ReportsRequest>(uri, 1, 0, "ReportsRequest");
    qmlRegisterType<StreamsModel>(uri, 1, 0, "StreamsModel");
    qmlRegisterType<StreamsRequest>(uri, 1, 0, "StreamsRequest");
    qmlRegisterType<StrongtagsModel>(uri, 1, 0, "StrongtagsModel");
    qmlRegisterType<StrongtagsRequest>(uri, 1, 0, "StrongtagsRequest");
    qmlRegisterType<SubtitlesModel>(uri, 1, 0, "SubtitlesModel");
    qmlRegisterType<SubtitlesRequest>(uri, 1, 0, "SubtitlesRequest");
    qmlRegisterType<UsersModel>(uri, 1, 0, "UsersModel");
    qmlRegisterType<UsersRequest>(uri, 1, 0, "UsersRequest");
    qmlRegisterType<VideosModel>(uri, 1, 0, "VideosModel");
    qmlRegisterType<VideosRequest>(uri, 1, 0, "VideosRequest");
}

}

QML_DECLARE_TYPE(QDailymotion::ActivitiesModel)
QML_DECLARE_TYPE(QDailymotion::ActivitiesRequest)
QML_DECLARE_TYPE(QDailymotion::AuthenticationRequest)
QML_DECLARE_TYPE(QDailymotion::ChannelsModel)
QML_DECLARE_TYPE(QDailymotion::ChannelsRequest)
QML_DECLARE_TYPE(QDailymotion::CommentsModel)
QML_DECLARE_TYPE(QDailymotion::CommentsRequest)
QML_DECLARE_TYPE(QDailymotion::ContestsModel)
QML_DECLARE_TYPE(QDailymotion::ContestsRequest)
QML_DECLARE_TYPE(QDailymotion::GroupsModel)
QML_DECLARE_TYPE(QDailymotion::GroupsRequest)
QML_DECLARE_TYPE(QDailymotion::LocalesModel)
QML_DECLARE_TYPE(QDailymotion::LocalesRequest)
QML_DECLARE_TYPE(QDailymotion::PlaylistsModel)
QML_DECLARE_TYPE(QDailymotion::PlaylistsRequest)
QML_DECLARE_TYPE(QDailymotion::ReportsModel)
QML_DECLARE_TYPE(QDailymotion::ReportsRequest)
QML_DECLARE_TYPE(QDailymotion::StreamsModel)
QML_DECLARE_TYPE(QDailymotion::StreamsRequest)
QML_DECLARE_TYPE(QDailymotion::StrongtagsModel)
QML_DECLARE_TYPE(QDailymotion::StrongtagsRequest)
QML_DECLARE_TYPE(QDailymotion::SubtitlesModel)
QML_DECLARE_TYPE(QDailymotion::SubtitlesRequest)
QML_DECLARE_TYPE(QDailymotion::UsersModel)
QML_DECLARE_TYPE(QDailymotion::UsersRequest)
QML_DECLARE_TYPE(QDailymotion::VideosModel)
QML_DECLARE_TYPE(QDailymotion::VideosRequest)
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qdailymotionplugin, QDailymotion::Plugin)
#endif
