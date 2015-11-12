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

#include "resourcesrequest.h"
#include "request_p.h"
#include "urls.h"

namespace QDailymotion {

/*!
    \class ResourcesRequest
    \brief Handles requests for Dailymotion resources.
    
    \ingroup requests
    
    The ResourcesRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    ResourcesRequest request;
    request.get("/video/VIDEO_ID");
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == ResourcesRequest::Ready) {            
            QMapIterator<QString, QVariant> iterator(request.result().toMap());
            
            while (iterator.hasNext()) {
                iterator.next();
                qDebug() << iterator.key() << "=" << iterator.value();
            }
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
    
    ResourcesRequest {
        id: request

        onFinished: {
            if (status == ResourcesRequest.Ready) {
                for (var k in result) {
                    console.log(att + " = " + result[k]);
                }
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get("/video/VIDEO_ID")
    }
    \endcode
    
    For more details about Dailymotion resources, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#api-reference">here</a>.
*/
ResourcesRequest::ResourcesRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion resources from \a resourcePath.
    
    For example, to search videos:
    
    \code
    ResourcesRequest request;
    QVariantMap filters;
    filters["limit"] = 10;
    filters["sort"] = "date";
    filters["search"] = "Qt";
    request.list("/videos", filters);
    \endcode
*/
void ResourcesRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() == Loading) {
        return;
    }
    
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath));
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
    setData(QVariant());
    Request::get();
}

/*!
    \brief Retrieves the Dailymotion resource from \a resourcePath.
    
    For example, to retrieve a video:
    
    \code
    ResourcesRequest request;
    request.get("/videos/VIDEO_ID");
    \endcode
*/
void ResourcesRequest::get(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() == Loading) {
        return;
    }
    
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath));
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
    setData(QVariant());
    Request::get();
}

/*!
    \brief Inserts a Dailymotion resource into \a resourcePath using a POST request.
    
    For example, to add a video to the authenticated user's favourites:
    
    \code
    ResourcesRequest request;
    request.insert("/me/favorites/VIDEO_ID");
    \endcode
*/
void ResourcesRequest::insert(const QString &resourcePath) {
    if (status() == Loading) {
        return;
    }
    
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath));
    setUrl(u);
    setData(QVariant());
    Request::post();
}

/*!
    \brief Inserts a new Dailymotion resource.
    
    For example, to insert a new playlist on behalf of the authenticated user:
    
    \code
    ResourcesRequest request;
    QVariantMap playlist;
    playlist["name"] = "My playlist";
    playlist["description"] = "Playlist inserted using QDailymotion";
    request.insert(playlist, "/me/playlists");
    \endcode
*/
void ResourcesRequest::insert(const QVariantMap &resource, const QString &resourcePath) {
    if (status() == Loading) {
        return;
    }
    
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath));
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Updates the Dailymotion resource at \a resourcePath.
    
    For example, to update an existing playlist on behalf of the authenticated user:
    
    \code
    ResourcesRequest request;
    QVariantMap playlist;
    playlist["name"] = "My new playlist name";
    playlist["description"] = "My new playlist description";
    request.update("/me/playlists/PLAYLIST_ID", playlist);
    \endcode
*/
void ResourcesRequest::update(const QString &resourcePath, const QVariantMap &resource) {
    if (status() == Loading) {
        return;
    }
    
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath));
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the Dailymotion resource at \a resourcePath.
    
    For example, to remove a video from the authenticated user's favourites:
    
    \code
    ResourcesRequest request;
    request.del("/me/favorites/VIDEO_ID");
    \endcode
*/
void ResourcesRequest::del(const QString &resourcePath) {
    if (status() == Loading) {
        return;
    }
    
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath));
    setUrl(u);
    setData(QVariant());
    deleteResource();
}

}
