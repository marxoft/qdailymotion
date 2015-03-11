/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef URLS_H
#define URLS_H

#include <QString>

namespace QDailymotion {

// API
static const QString API_URL("https://api.dailymotion.com");

// Authentication
static const QString AUTH_URL("https://www.dailymotion.com/oauth/authorize");
static const QString TOKEN_URL("https://api.dailymotion.com/oauth/token");
static const QString REVOKE_TOKEN_URL("https://api.dailymotion.com/logout");

static const QString GRANT_TYPE_CODE("authorization_code");
static const QString GRANT_TYPE_PASSWORD("password");
static const QString GRANT_TYPE_REFRESH("refresh_token");

static const QString EMAIL_SCOPE("email");
static const QString USER_INFO_SCOPE("userinfo");
static const QString MANAGE_VIDEOS_SCOPE("manage_videos");
static const QString MANAGE_COMMENTS_SCOPE("manage_comments");
static const QString MANAGE_PLAYLISTS_SCOPE("manage_playlists");
static const QString MANAGE_TILES_SCOPE("manage_tiles");
static const QString MANAGE_SUBSCRIPTIONS_SCOPE("manage_subscriptions");
static const QString MANAGE_FRIENDS_SCOPE("manage_friends");
static const QString MANAGE_FAVORITES_SCOPE("manage_favorites");
static const QString MANAGE_GROUPS_SCOPE("manage_groups");

// VideoPage
static const QString VIDEO_PAGE_URL("http://www.dailymotion.com/embed/video");

}

#endif // URLS_H
