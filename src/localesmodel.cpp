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

#include "localesmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class LocalesModelPrivate : public ModelPrivate
{

public:
    LocalesModelPrivate(LocalesModel *parent) :
        ModelPrivate(parent),
        request(0)
    {
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(LocalesModel);
    
        if (request->status() == LocalesRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {            
                QVariantList list = result.value("list").toList();
            
                if (!list.isEmpty()) {
                    q->beginInsertRows(QModelIndex(), items.size(), items.size() + list.size());
                    
                    foreach (QVariant item, list) {
                        items << item.toMap();
                    }
                    
                    q->endInsertRows();
                    emit q->countChanged();
                }
            }
        }
        
        LocalesModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    LocalesRequest *request;

    Q_DECLARE_PUBLIC(LocalesModel)
};

/*!
    \class LocalesModel
    \brief A list model for displaying Dailymotion locale resources.
    
    \ingroup locales
    \ingroup models
    
    The LocalesModel is a list model used for displaying Dailymotion locales in a list view. LocalesModel 
    provides the same methods that are available in LocalesRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    LocalesModel provides the following 
    roles and role names:
    
    <table>
        <tr>
            <th>Role</th>
            <th>Role name</th>
            <th>Type</th>
        </tr>
        <tr>
            <td>Qt::UserRole + 1</td>
            <td>locale</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 2</td>
            <td>site_code</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 3</td>
            <td>language</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 4</td>
            <td>localized_language</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 5</td>
            <td>locally_localized_language</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 6</td>
            <td>country</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 7</td>
            <td>localized_country</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 8</td>
            <td>locally_localized_country</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 9</td>
            <td>currency</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    LocalesModel *model = new LocalesModel(this);
    view->setModel(new LocalesModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    model->list(filters);
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: LocalesModel {
            id: localesModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: language
        }
        
        Component.onCompleted: localesModel.list({limit: 20})
    }
    \endcode
    
    \sa LocalesRequest
*/

/*!
    \enum LocalesModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

LocalesModel::LocalesModel(QObject *parent) :
    Model(*new LocalesModelPrivate(this), parent)
{
    Q_D(LocalesModel);
    
    d->roles[Qt::UserRole + 1] = "locale";
    d->roles[Qt::UserRole + 2] = "site_code";
    d->roles[Qt::UserRole + 3] = "language";
    d->roles[Qt::UserRole + 4] = "localized_language";
    d->roles[Qt::UserRole + 5] = "locally_localized_language";
    d->roles[Qt::UserRole + 6] = "country";
    d->roles[Qt::UserRole + 7] = "localized_country";
    d->roles[Qt::UserRole + 8] = "locally_localized_country";
    d->roles[Qt::UserRole + 9] = "currency";
    d->request = new LocalesRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString LocalesModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa LocalesRequest::clientId
*/

/*!
    \fn void LocalesModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString LocalesModel::clientId() const {
    Q_D(const LocalesModel);
    
    return d->request->clientId();
}

void LocalesModel::setClientId(const QString &id) {
    Q_D(LocalesModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString LocalesModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa LocalesRequest::clientSecret
*/

/*!
    \fn void LocalesModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString LocalesModel::clientSecret() const {
    Q_D(const LocalesModel);
    
    return d->request->clientSecret();
}

void LocalesModel::setClientSecret(const QString &secret) {
    Q_D(LocalesModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString LocalesModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a locale's protected resources.
    
    \as LocalesRequest::accessToken
*/

/*!
    \fn void LocalesModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString LocalesModel::accessToken() const {
    Q_D(const LocalesModel);
    
    return d->request->accessToken();
}

void LocalesModel::setAccessToken(const QString &token) {
    Q_D(LocalesModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString LocalesModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa LocalesRequest::refreshToken
*/

/*!
    \fn void LocalesModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString LocalesModel::refreshToken() const {
    Q_D(const LocalesModel);
    
    return d->request->refreshToken();
}

void LocalesModel::setRefreshToken(const QString &token) {
    Q_D(LocalesModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum LocalesModel::status
    \brief The current status of the model.
    
    \sa LocalesRequest::status
*/

/*!
    \fn void LocalesModel::statusChanged()
    \brief Emitted when the status changes.
*/
LocalesRequest::Status LocalesModel::status() const {
    Q_D(const LocalesModel);
    
    return d->request->status();
}

/*!
    \property enum LocalesModel::error
    \brief The error type of the model.
    
    \sa LocalesRequest::error
*/
LocalesRequest::Error LocalesModel::error() const {
    Q_D(const LocalesModel);
    
    return d->request->error();
}

/*!
    \property enum LocalesModel::errorString
    \brief A description of the error of the model.
    
    \sa LocalesRequest::status
*/
QString LocalesModel::errorString() const {
    Q_D(const LocalesModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    LocalesModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa LocalesRequest::setNetworkAccessManager()
*/
void LocalesModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(LocalesModel);
    
    d->request->setNetworkAccessManager(manager);
}

/*!
    \brief Retrieves a list of Dailymotion locale resources.
        
    \sa LocalesRequest::list()
*/
void LocalesModel::list() {
    if (status() != LocalesRequest::Loading) {
        Q_D(LocalesModel);
        clear();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list();
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa LocalesRequest::cancel()
*/
void LocalesModel::cancel() {
    Q_D(LocalesModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion locale resources 
    using the existing parameters.
*/
void LocalesModel::reload() {
    if (status() != LocalesRequest::Loading) {
        Q_D(LocalesModel);
        clear();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list();
        emit statusChanged();
    }
}

}

#include "moc_localesmodel.cpp"
