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

#include "usersmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class UsersModelPrivate : public ModelPrivate
{

public:
    UsersModelPrivate(UsersModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "screenname";
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
        qDebug() << "UsersModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(UsersModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(UsersModel);
    
        if (request->status() == UsersRequest::Ready) {
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
        
        UsersModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(UsersModel);
    
        if ((request->status() == UsersRequest::Ready) && (writeResourcePath == resourcePath)) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        UsersModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(UsersModel);
    
        if (request->status() == UsersRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                QString id = result.value("id").toString();
                
                if (!id.isEmpty()) {
                    QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, id, 1, Qt::MatchExactly);
                    
                    if (!indexes.isEmpty()) {
                        q->set(indexes.first().row(), result);
                    }
                }
            }
        }
        
        UsersModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(UsersModel);
    
        if ((request->status() == UsersRequest::Ready) &&
            ((writeResourcePath == resourcePath) || (writeResourcePath.isEmpty()))) {
            QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, delId, 1, Qt::MatchExactly);
            
            if (!indexes.isEmpty()) {
                QModelIndex index = indexes.first();
                q->beginRemoveRows(QModelIndex(), index.row(), index.row());
                items.removeAt(index.row());
                q->endRemoveRows();
                emit q->countChanged();
            }
        }
        
        UsersModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    UsersRequest *request;
    
    QString resourcePath;
    QString writeResourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(UsersModel)
};

/*!
    \class UsersModel
    \brief A list model for displaying Dailymotion user resources.
    
    \ingroup users
    \ingroup models
    
    The UsersModel is a list model used for displaying Dailymotion users in a list view. UsersModel 
    provides the same methods that are available in UsersRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by UsersModel depends on which fields are specified when calling list(). If fields are 
    specified, UsersModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, UsersModel provides the following 
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
            <td>screenname</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    UsersModel *model = new UsersModel(this);
    view->setModel(new UsersModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    filters["search"] = "Qt";
    model->list("", filters, QStringList() << "id" << "screenname" << "username", "avatar_120_url");
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: UsersModel {
            id: usersModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: username
        }
        
        Component.onCompleted: usersModel.list("", {limit: 20, search: "Qt"},
                                               ["id", "screenname", "username", "avatar_120_url"])
    }
    \endcode
    
    \sa UsersRequest
*/

/*!
    \enum UsersModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

UsersModel::UsersModel(QObject *parent) :
    Model(*new UsersModelPrivate(this), parent)
{
    Q_D(UsersModel);
    
    d->request = new UsersRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString UsersModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa UsersRequest::clientId
*/

/*!
    \fn void UsersModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString UsersModel::clientId() const {
    Q_D(const UsersModel);
    
    return d->request->clientId();
}

void UsersModel::setClientId(const QString &id) {
    Q_D(UsersModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString UsersModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa UsersRequest::clientSecret
*/

/*!
    \fn void UsersModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString UsersModel::clientSecret() const {
    Q_D(const UsersModel);
    
    return d->request->clientSecret();
}

void UsersModel::setClientSecret(const QString &secret) {
    Q_D(UsersModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString UsersModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a user's protected resources.
    
    \as UsersRequest::accessToken
*/

/*!
    \fn void UsersModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString UsersModel::accessToken() const {
    Q_D(const UsersModel);
    
    return d->request->accessToken();
}

void UsersModel::setAccessToken(const QString &token) {
    Q_D(UsersModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString UsersModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa UsersRequest::refreshToken
*/

/*!
    \fn void UsersModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString UsersModel::refreshToken() const {
    Q_D(const UsersModel);
    
    return d->request->refreshToken();
}

void UsersModel::setRefreshToken(const QString &token) {
    Q_D(UsersModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum UsersModel::status
    \brief The current status of the model.
    
    \sa UsersRequest::status
*/

/*!
    \fn void UsersModel::statusChanged()
    \brief Emitted when the status changes.
*/
UsersRequest::Status UsersModel::status() const {
    Q_D(const UsersModel);
    
    return d->request->status();
}

/*!
    \property enum UsersModel::error
    \brief The error type of the model.
    
    \sa UsersRequest::error
*/
UsersRequest::Error UsersModel::error() const {
    Q_D(const UsersModel);
    
    return d->request->error();
}

/*!
    \property enum UsersModel::errorString
    \brief A description of the error of the model.
    
    \sa UsersRequest::status
*/
QString UsersModel::errorString() const {
    Q_D(const UsersModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    UsersModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa UsersRequest::setNetworkAccessManager()
*/
void UsersModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(UsersModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool UsersModel::canFetchMore(const QModelIndex &) const {
    if (status() == UsersRequest::Loading) {
        return false;
    }
    
    Q_D(const UsersModel);
    
    return d->hasMore;
}

void UsersModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(UsersModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion user resources belonging to \a resourcePath.
        
    \sa UsersRequest::list()
*/
void UsersModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != UsersRequest::Loading) {
        Q_D(UsersModel);
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
    \brief Inserts the Dailymotion user at \a row into the \a resourcePath.
    
    \sa UsersRequest::insert()
*/
void UsersModel::insert(int row, const QString &resourcePath) {
    if (status() != UsersRequest::Loading) {
        Q_D(UsersModel);
        d->writeResourcePath = resourcePath;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(data(index(row), Qt::UserRole + 1).toString(), resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion user at \a row with \a resource.
*/
void UsersModel::update(int row, const QVariantMap &resource) {
    if (status() != UsersRequest::Loading) {
        Q_D(UsersModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion user at \a row from \a resourcePath.
*/
void UsersModel::del(int row, const QString &resourcePath) {
    if (status() != UsersRequest::Loading) {
        Q_D(UsersModel);
        d->writeResourcePath = resourcePath;
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId, resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa UsersRequest::cancel()
*/
void UsersModel::cancel() {
    Q_D(UsersModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion user resources 
    using the existing parameters.
*/
void UsersModel::reload() {
    if (status() != UsersRequest::Loading) {
        Q_D(UsersModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_usersmodel.cpp"
