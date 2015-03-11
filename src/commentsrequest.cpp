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

#include "commentsrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class CommentsRequest
    \brief Handles requests for Dailymotion comment resources.
    
    \ingroup comments
    \ingroup requests
    
    The CommentsRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion comment resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    CommentsRequest request;
    request.get(COMMENT_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == CommentsRequest::Ready) {            
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
    
    CommentsRequest {
        id: request

        onFinished: {
            if (status == CommentsRequest.Ready) {
                console.log(result.message);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(COMMENT_ID)
    }
    \endcode
    
    For more details about Dailymotion comments, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#comment">here</a>.
*/
CommentsRequest::CommentsRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion comment resources from \a resourcePath.
    
    For example to retrieve comments belonging to a video:
    
    \code
    CommentsRequest request;
    request.list("/video/VIDEO_ID/comments");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#comment">here</a>.
*/
void CommentsRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("/comments") : resourcePath));
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
        addUrlQueryItems(&url, filters);
    }
    
    if (!fields.isEmpty()) {
        url.addQueryItem("fields", fields.join(","));
    }
#endif
    setUrl(u);
    Request::get();
}

/*!
    \brief Retrieves the Dailymotion comment resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#comment">here</a>.
*/
void CommentsRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/comment/" + id);
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
    \brief Inserts a new comment into \a resourcePath.
*/

void CommentsRequest::insert(const QVariantMap &resource, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath));
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Updates the comment with \a id.
*/
void CommentsRequest::update(const QString &id, const QVariantMap &resource) {
    QUrl u(API_URL + "/comment/" + id);
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the comment with \a id.
*/
void CommentsRequest::del(const QString &id) {
    QUrl u(API_URL + "/comment/" + id);
    setUrl(u);
    deleteResource();
}

}
