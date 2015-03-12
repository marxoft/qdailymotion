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

#include "subtitlesrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class SubtitlesRequest
    \brief Handles requests for Dailymotion subtitle resources.
    
    \ingroup subtitles
    \ingroup requests
    
    The SubtitlesRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion subtitle resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    SubtitlesRequest request;
    request.get(SUBTITLE_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == SubtitlesRequest::Ready) {            
            qDebug() << request.result.toMap().value("language").toString();
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
    
    SubtitlesRequest {
        id: request

        onFinished: {
            if (status == SubtitlesRequest.Ready) {
                console.log(result.language);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(SUBTITLE_ID)
    }
    \endcode
    
    For more details about Dailymotion subtitles, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#subtitle">here</a>.
*/
SubtitlesRequest::SubtitlesRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion subtitle resources from \a resourcePath.
    
    For example to retrieve subtitles belonging to a video:
    
    \code
    SubtitlesRequest request;
    request.list("/video/VIDEO_ID/subtitles");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#subtitle">here</a>.
*/
void SubtitlesRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("subtitles") : resourcePath));
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
    \brief Retrieves the Dailymotion subtitle resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#subtitle">here</a>.
*/
void SubtitlesRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/subtitle/" + id);
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
    \brief Inserts a new subtitle into \a resourcePath.
*/

void SubtitlesRequest::insert(const QVariantMap &resource, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath));
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Updates the subtitle with \a id.
*/
void SubtitlesRequest::update(const QString &id, const QVariantMap &resource) {
    QUrl u(API_URL + "/subtitle/" + id);
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the subtitle with \a id.
*/
void SubtitlesRequest::del(const QString &id) {
    QUrl u(API_URL + "/subtitle/" + id);
    setUrl(u);
    deleteResource();
}

}
