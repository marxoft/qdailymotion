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

#include "channelsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class ChannelsModelPrivate : public ModelPrivate
{

public:
    ChannelsModelPrivate(ChannelsModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "description";
            roles[Qt::UserRole + 3] = "name";
        }
        else {
            fields.prepend("id");
            fields.removeDuplicates();
            int i = Qt::UserRole + 1;
            
            foreach (QString field, fields) {
                roles[i] = field.toUtf8();
                i++;
            }
        }
#ifdef QDAILYMOTION_DEBUG
        qDebug() << "ChannelsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(ChannelsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ChannelsModel);
    
        if (request->status() == ChannelsRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                hasMore = result.value("has_more").toBool();
            
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
        
        ChannelsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    ChannelsRequest *request;
    
    QString resourcePath;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(ChannelsModel)
};

/*!
    \class ChannelsModel
    \brief A list model for displaying Dailymotion channel resources.
    
    \ingroup channels
    \ingroup models
    
    The ChannelsModel is a list model used for displaying Dailymotion channels in a list view. ChannelsModel 
    provides the same methods that are available in ChannelsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by ChannelsModel depends on which fields are specified when calling list(). If fields are 
    specified, ChannelsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, ChannelsModel provides the following 
    roles and role names:
    
    <table>
        <tr>
            <th>Role</th>
            <th>Role name</th>
            <th>Type</th>
        </tr>
        <tr>
            <td>Qt::UserRole + 1</td>
            <td>id</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 2</td>
            <td>description</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 3</td>
            <td>name</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    ChannelsModel *model = new ChannelsModel(this);
    view->setModel(new ChannelsModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    filters["family_filter"] = true;
    model->list("", filters);
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: ChannelsModel {
            id: channelsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: name
        }
        
        Component.onCompleted: channelsModel.list("", {limit: 20, family_filter: true})
    }
    \endcode
    
    \sa ChannelsRequest
*/

/*!
    \enum ChannelsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

ChannelsModel::ChannelsModel(QObject *parent) :
    Model(*new ChannelsModelPrivate(this), parent)
{
    Q_D(ChannelsModel);
    
    d->request = new ChannelsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString ChannelsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa ChannelsRequest::clientId
*/

/*!
    \fn void ChannelsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString ChannelsModel::clientId() const {
    Q_D(const ChannelsModel);
    
    return d->request->clientId();
}

void ChannelsModel::setClientId(const QString &id) {
    Q_D(ChannelsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString ChannelsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa ChannelsRequest::clientSecret
*/

/*!
    \fn void ChannelsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString ChannelsModel::clientSecret() const {
    Q_D(const ChannelsModel);
    
    return d->request->clientSecret();
}

void ChannelsModel::setClientSecret(const QString &secret) {
    Q_D(ChannelsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString ChannelsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a channel's protected resources.
    
    \as ChannelsRequest::accessToken
*/

/*!
    \fn void ChannelsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString ChannelsModel::accessToken() const {
    Q_D(const ChannelsModel);
    
    return d->request->accessToken();
}

void ChannelsModel::setAccessToken(const QString &token) {
    Q_D(ChannelsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString ChannelsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa ChannelsRequest::refreshToken
*/

/*!
    \fn void ChannelsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString ChannelsModel::refreshToken() const {
    Q_D(const ChannelsModel);
    
    return d->request->refreshToken();
}

void ChannelsModel::setRefreshToken(const QString &token) {
    Q_D(ChannelsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum ChannelsModel::status
    \brief The current status of the model.
    
    \sa ChannelsRequest::status
*/

/*!
    \fn void ChannelsModel::statusChanged()
    \brief Emitted when the status changes.
*/
ChannelsRequest::Status ChannelsModel::status() const {
    Q_D(const ChannelsModel);
    
    return d->request->status();
}

/*!
    \property enum ChannelsModel::error
    \brief The error type of the model.
    
    \sa ChannelsRequest::error
*/
ChannelsRequest::Error ChannelsModel::error() const {
    Q_D(const ChannelsModel);
    
    return d->request->error();
}

/*!
    \property enum ChannelsModel::errorString
    \brief A description of the error of the model.
    
    \sa ChannelsRequest::status
*/
QString ChannelsModel::errorString() const {
    Q_D(const ChannelsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    ChannelsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa ChannelsRequest::setNetworkAccessManager()
*/
void ChannelsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(ChannelsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool ChannelsModel::canFetchMore(const QModelIndex &) const {
    if (status() == ChannelsRequest::Loading) {
        return false;
    }
    
    Q_D(const ChannelsModel);
    
    return d->hasMore;
}

void ChannelsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(ChannelsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion channel resources belonging to \a resourcePath.
        
    \sa ChannelsRequest::list()
*/
void ChannelsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != ChannelsRequest::Loading) {
        Q_D(ChannelsModel);
        clear();
        d->resourcePath = resourcePath;
        d->filters = filters;
        d->fields = fields;
        d->setRoleNames();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa ChannelsRequest::cancel()
*/
void ChannelsModel::cancel() {
    Q_D(ChannelsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion channel resources 
    using the existing parameters.
*/
void ChannelsModel::reload() {
    if (status() != ChannelsRequest::Loading) {
        Q_D(ChannelsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_channelsmodel.cpp"
