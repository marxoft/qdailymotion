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

#include "videosrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class VideosRequest
    \brief Handles requests for Dailymotion video resources.
    
    \ingroup videos
    \ingroup requests
    
    The VideosRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion video resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    VideosRequest request;
    request.get(VIDEO_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == VideosRequest::Ready) {            
            qDebug() << request.result.toMap().value("title").toString();
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
    
    VideosRequest {
        id: request

        onFinished: {
            if (status == VideosRequest.Ready) {
                console.log(result.title);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(VIDEO_ID)
    }
    \endcode
    
    For more details about Dailymotion videos, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#video">here</a>.
*/
VideosRequest::VideosRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion video resources from \a resourcePath.
    
    For example to retrieve videos belonging to a playlist:
    
    \code
    VideosRequest request;
    request.list("/playlist/PLAYLIST_ID/videos");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#video">here</a>.
*/
void VideosRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("videos") : resourcePath));
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
    \brief Retrieves the Dailymotion video resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#video">here</a>.
*/
void VideosRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/video/" + id);
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
    \brief Inserts the video with \a id into the \a resourcePath.
    
    For example, to add a video to the authenticated user's favorites:
    
    \code
    VideoRequest request;
    request.insert(VIDEO_ID, "/user/me/favorites");
    \endcode
*/

void VideosRequest::insert(const QString &id, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath).arg(resourcePath.endsWith("/") ? QString() : QString("/"))
                                .arg(id));
    setUrl(u);
    post();
}

/*!
    \brief Updates the video with \a id.
*/
void VideosRequest::update(const QString &id, const QVariantMap &resource) {
    QUrl u(API_URL + "/video/" + id);
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the video with \a id from the \a resourcePath.
    
    For example, to delete a video from the authenticated user's favorites:
    
    \code
    VideoRequest request;
    request.del(VIDEO_ID, "/user/me/favorites");
    \endcode
*/
void VideosRequest::del(const QString &id, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath.isEmpty() ? QString("video") : resourcePath)
                                .arg(resourcePath.endsWith("/") ? QString() : QString("/"))
                                .arg(id));
    setUrl(u);
    deleteResource();
}

}
