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

#include "usersrequest.h"
#include "request_p.h"
#include "urls.h"
#include "json.h"

namespace QDailymotion {

/*!
    \class UsersRequest
    \brief Handles requests for Dailymotion user resources.
    
    \ingroup users
    \ingroup requests
    
    The UsersRequest class is used for making requests to the Dailymotion Data API that concern 
    Dailymotion user resources.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    UsersRequest request;
    request.get(USER_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == UsersRequest::Ready) {            
            qDebug() << request.result.toMap().value("username").toString();
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
    
    UsersRequest {
        id: request

        onFinished: {
            if (status == UsersRequest.Ready) {
                console.log(result.username);
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: get(USER_ID)
    }
    \endcode
    
    For more details about Dailymotion users, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#user">here</a>.
*/
UsersRequest::UsersRequest(QObject *parent) :
    Request(parent)
{
}

/*!
    \brief Requests a list of Dailymotion user resources from \a resourcePath.
    
    For example to retrieve users followed by another user:
    
    \code
    UsersRequest request;
    request.list("/user/USER_ID/following");
    \endcode
    
    For more details on the available \a filters and \a fields, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#user">here</a>.
*/
void UsersRequest::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(QString("%1%2%3").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                            .arg(resourcePath.isEmpty() ? QString("users") : resourcePath));
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
    \brief Retrieves the Dailymotion user resource with \a id.
        
    For more details of the acceptable parameter values, see the Dailymotion reference documentation 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#user">here</a>.
*/
void UsersRequest::get(const QString &id, const QVariantMap &filters, const QStringList &fields) {
    QUrl u(API_URL + "/user/" + id);
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
    \brief Inserts the user with \a id into the \a resourcePath.
    
    For example, to add a user to the authenticated user's followings:
    
    \code
    UserRequest request;
    request.insert(USER_ID, "/user/me/following");
    \endcode
*/

void UsersRequest::insert(const QString &id, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath).arg(resourcePath.endsWith("/") ? QString() : QString("/"))
                                .arg(id));
    setUrl(u);
    post();
}

/*!
    \brief Updates the user with \a id.
*/
void UsersRequest::update(const QString &id, const QVariantMap &resource) {
    QUrl u(API_URL + "/user/" + id);
    QString body;
    addPostBody(&body, resource);
    setUrl(u);
    setData(body);
    post();
}

/*!
    \brief Deletes the user with \a id from the \a resourcePath.
    
    For example, to delete a user from the authenticated user's followings:
    
    \code
    UserRequest request;
    request.del(USER_ID, "/user/me/following");
    \endcode
*/
void UsersRequest::del(const QString &id, const QString &resourcePath) {
    QUrl u(QString("%1%2%3%4%5").arg(API_URL).arg(resourcePath.startsWith("/") ? QString() : QString("/"))
                                .arg(resourcePath.isEmpty() ? QString("user") : resourcePath)
                                .arg(resourcePath.endsWith("/") ? QString() : QString("/"))
                                .arg(id));
    setUrl(u);
    deleteResource();
}

}
