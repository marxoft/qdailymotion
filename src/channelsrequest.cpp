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

#include "channelsrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class ChannelsRequest
    \brief Handles requests for Dailymotion channel resources.
    
    \ingroup channels
    \ingroup requests
    
    The ChannelsRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion channel resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    ChannelsRequest request;
    request.get(CHANNEL_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == ChannelsRequest::Ready) {            
            qDebug() << request.result.toMap().value("name").toString();
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
    
    ChannelsRequest {
        id: request

        onFinished: {
            if (status == ChannelsRequest.Ready) {
                console.log(result.name);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(CHANNEL_ID)
    }
    \endcode
    
    For more details about Dailymotion channels, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#channel">here</a>.
*/
ChannelsRequest::ChannelsRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion channel resources from \a resourcePath.
    
    For example to retrieve channels to which a video belongs:
    
    \code
    ChannelsRequest request;
    request.list("/video/VIDEO_ID/channels");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#channel">here</a>.
*/
void ChannelsRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("/channels") : resourcePath));
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
    \brief Retrieves the Dailymotion channel resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#channel">here</a>.
*/
void ChannelsRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/channel/" + id);
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

}
