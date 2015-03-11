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

#include "localesrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class LocalesRequest
    \brief Handles requests for Dailymotion locale resources.
    
    \ingroup locales
    \ingroup requests
    
    The LocalesRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion locale resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    LocalesRequest request;
    request.list();
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == LocalesRequest::Ready) {            
            foreach (QVariant locale, request.result().toMap().value("list").toList()) {
                qDebug() << locale.toMap().value("language").toString();
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
    
    LocalesRequest {
        id: request

        onFinished: {
            if (status == LocalesRequest.Ready) {
                for (var i = 0; i < result.list.length; i++) {
                    console.log(result.list[i].language);
                }
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: list()
    }
    \endcode
    
    For more details about Dailymotion locales, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#locale">here</a>.
*/
LocalesRequest::LocalesRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion locale resources.
        
    For more details on the available \a filters, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#locale">here</a>.
*/
void LocalesRequest::list() {
    QUrl u(API_URL + "/locales");
    setUrl(u);
    Request::get();
}

}
