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

#include "playlistsrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class PlaylistsRequest
    \brief Handles requests for Dailymotion playlist resources.
    
    \ingroup playlists
    \ingroup requests
    
    The PlaylistsRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion playlist resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    PlaylistsRequest request;
    request.get(PLAYLIST_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == PlaylistsRequest::Ready) {            
            qDebug() << request.result.toMap().value("message").toString();
        }
        else {
            qDebug() << request.errorString();
        }
    }
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    PlaylistsRequest {
        id: request

        onFinished: {
            if (status == PlaylistsRequest.Ready) {
                console.log(result.message);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(PLAYLIST_ID)
    }
    \endcode
    
    For more details about Dailymotion playlists, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#playlist">here</a>.
*/
PlaylistsRequest::PlaylistsRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion playlist resources from \a resourcePath.
    
    For example to retrieve playlists belonging to a user:
    
    \code
    PlaylistsRequest request;
    request.list("/user/USER_ID/playlists");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#playlist">here</a>.
*/
void PlaylistsRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("playlists") : resourcePath));
#if QT_VERSION >= 0x050000
    QUrlQuery query(u);
    
    if (!filters.isEmpty()) {
        addUrlQueryItems(&query, filters);
    }
    
    if (!fields.isEmpty()) {
        query.addQueryItem("fields", fields.join(","));
    }
    
    u.setQuery(query);
#else    
    if (!filters.isEmpty()) {
        addUrlQueryItems(&u, filters);
    }
    
    if (!fields.isEmpty()) {
        u.addQueryItem("fields", fields.join(","));
    }
#endif
    setUrl(u);
    Request::get();
}

/*!
    \brief Retrieves the Dailymotion playlist resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#playlist">here</a>.
*/
void PlaylistsRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/playlist/" + id);
#if QT_VERSION >= 0x050000
    QUrlQuery query(u);
    
    if (!filters.isEmpty()) {
        addUrlQueryItems(&query, filters);
    }
    
    if (!fields.isEmpty()) {
        query.addQueryItem("fields", fields.join(","));
    }
    
    u.setQuery(query);
#else    
    if (!filters.isEmpty()) {
        addUrlQueryItems(&u, filters);
    }
    
    if (!fields.isEmpty()) {
        u.addQueryItem("fields", fields.join(","));
    }
#endif
    setUrl(u);
    Request::get();
}

/*!
    \brief Inserts a new playlist for the authenticated user
*/

void PlaylistsRequest::insert(const QVariantMap &resource) {
    QUrl u(API_URL + "/me/playlists");
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Updates the playlist with \a id.
*/
void PlaylistsRequest::update(const QString &id, const QVariantMap &resource) {
    QUrl u(API_URL + "/playlist/" + id);
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the playlist with \a id.
*/
void PlaylistsRequest::del(const QString &id) {
    QUrl u(API_URL + "/playlist/" + id);
    setUrl(u);
    deleteResource();
}

}
