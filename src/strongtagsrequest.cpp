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

#include "strongtagsrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class StrongtagsRequest
    \brief Handles requests for Dailymotion strongtag resources.
    
    \ingroup strongtags
    \ingroup requests
    
    The StrongtagsRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion strongtag resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    StrongtagsRequest request;
    request.get(STRONGTAG_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == StrongtagsRequest::Ready) {            
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
    
    StrongtagsRequest {
        id: request

        onFinished: {
            if (status == StrongtagsRequest.Ready) {
                console.log(result.message);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(STRONGTAG_ID)
    }
    \endcode
    
    For more details about Dailymotion strongtags, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#strongtag">here</a>.
*/
StrongtagsRequest::StrongtagsRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion strongtag resources from \a resourcePath.
    
    For example to retrieve strongtags belonging to a video:
    
    \code
    StrongtagsRequest request;
    request.list("/video/VIDEO_ID/strongtags");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#strongtag">here</a>.
*/
void StrongtagsRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("strongtags") : resourcePath));
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
    \brief Retrieves the Dailymotion strongtag resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#strongtag">here</a>.
*/
void StrongtagsRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/strongtag/" + id);
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
    \brief Inserts a new strongtag into \a resourcePath.
*/

void StrongtagsRequest::insert(const QVariantMap &resource, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath));
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Updates the strongtag with \a id.
*/
void StrongtagsRequest::update(const QString &id, const QVariantMap &resource) {
    QUrl u(API_URL + "/strongtag/" + id);
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the strongtag with \a id.
*/
void StrongtagsRequest::del(const QString &id) {
    QUrl u(API_URL + "/strongtag/" + id);
    setUrl(u);
    deleteResource();
}

}
