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

#include "strongtagsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class StrongtagsModelPrivate : public ModelPrivate
{

public:
    StrongtagsModelPrivate(StrongtagsModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "name";
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
        qDebug() << "StrongtagsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(StrongtagsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(StrongtagsModel);
    
        if (request->status() == StrongtagsRequest::Ready) {
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
        
        StrongtagsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(StrongtagsModel);
    
        if (request->status() == StrongtagsRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        StrongtagsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(StrongtagsModel);
    
        if (request->status() == StrongtagsRequest::Ready) {
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
        
        StrongtagsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(StrongtagsModel);
    
        if (request->status() == StrongtagsRequest::Ready) {
            QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, delId, 1, Qt::MatchExactly);
            
            if (!indexes.isEmpty()) {
                QModelIndex index = indexes.first();
                q->beginRemoveRows(QModelIndex(), index.row(), index.row());
                items.removeAt(index.row());
                q->endRemoveRows();
                emit q->countChanged();
            }
        }
        
        StrongtagsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    StrongtagsRequest *request;
    
    QString resourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(StrongtagsModel)
};

/*!
    \class StrongtagsModel
    \brief A list model for displaying Dailymotion strongtag resources.
    
    \ingroup strongtags
    \ingroup models
    
    The StrongtagsModel is a list model used for displaying Dailymotion strongtags in a list view. StrongtagsModel 
    provides the same methods that are available in StrongtagsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by StrongtagsModel depends on which fields are specified when calling list(). If fields are 
    specified, StrongtagsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, StrongtagsModel provides the following 
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
    StrongtagsModel *model = new StrongtagsModel(this);
    view->setModel(new StrongtagsModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    model->list("/video/VIDEOS_ID/strongtags", filters);
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: StrongtagsModel {
            id: strongtagsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: name
        }
        
        Component.onCompleted: strongtagsModel.list("/video/VIDEO_ID/strongtags", {limit: 20})
    }
    \endcode
    
    \sa StrongtagsRequest
*/

/*!
    \enum StrongtagsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

StrongtagsModel::StrongtagsModel(QObject *parent) :
    Model(*new StrongtagsModelPrivate(this), parent)
{
    Q_D(StrongtagsModel);
    
    d->request = new StrongtagsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString StrongtagsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa StrongtagsRequest::clientId
*/

/*!
    \fn void StrongtagsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString StrongtagsModel::clientId() const {
    Q_D(const StrongtagsModel);
    
    return d->request->clientId();
}

void StrongtagsModel::setClientId(const QString &id) {
    Q_D(StrongtagsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString StrongtagsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa StrongtagsRequest::clientSecret
*/

/*!
    \fn void StrongtagsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString StrongtagsModel::clientSecret() const {
    Q_D(const StrongtagsModel);
    
    return d->request->clientSecret();
}

void StrongtagsModel::setClientSecret(const QString &secret) {
    Q_D(StrongtagsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString StrongtagsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a strongtag's protected resources.
    
    \as StrongtagsRequest::accessToken
*/

/*!
    \fn void StrongtagsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString StrongtagsModel::accessToken() const {
    Q_D(const StrongtagsModel);
    
    return d->request->accessToken();
}

void StrongtagsModel::setAccessToken(const QString &token) {
    Q_D(StrongtagsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString StrongtagsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa StrongtagsRequest::refreshToken
*/

/*!
    \fn void StrongtagsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString StrongtagsModel::refreshToken() const {
    Q_D(const StrongtagsModel);
    
    return d->request->refreshToken();
}

void StrongtagsModel::setRefreshToken(const QString &token) {
    Q_D(StrongtagsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum StrongtagsModel::status
    \brief The current status of the model.
    
    \sa StrongtagsRequest::status
*/

/*!
    \fn void StrongtagsModel::statusChanged()
    \brief Emitted when the status changes.
*/
StrongtagsRequest::Status StrongtagsModel::status() const {
    Q_D(const StrongtagsModel);
    
    return d->request->status();
}

/*!
    \property enum StrongtagsModel::error
    \brief The error type of the model.
    
    \sa StrongtagsRequest::error
*/
StrongtagsRequest::Error StrongtagsModel::error() const {
    Q_D(const StrongtagsModel);
    
    return d->request->error();
}

/*!
    \property enum StrongtagsModel::errorString
    \brief A description of the error of the model.
    
    \sa StrongtagsRequest::status
*/
QString StrongtagsModel::errorString() const {
    Q_D(const StrongtagsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    StrongtagsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa StrongtagsRequest::setNetworkAccessManager()
*/
void StrongtagsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(StrongtagsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool StrongtagsModel::canFetchMore(const QModelIndex &) const {
    if (status() == StrongtagsRequest::Loading) {
        return false;
    }
    
    Q_D(const StrongtagsModel);
    
    return d->hasMore;
}

void StrongtagsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(StrongtagsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion strongtag resources belonging to \a resourcePath.
        
    \sa StrongtagsRequest::list()
*/
void StrongtagsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != StrongtagsRequest::Loading) {
        Q_D(StrongtagsModel);
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
    \brief Inserts a new Dailymotion strongtag into the current resourcePath.
    
    \sa StrongtagsRequest::insert()
*/
void StrongtagsModel::insert(const QVariantMap &resource) {
    if (status() != StrongtagsRequest::Loading) {
        Q_D(StrongtagsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(resource, d->resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion strongtag at \a row with \a resource.
*/
void StrongtagsModel::update(int row, const QVariantMap &resource) {
    if (status() != StrongtagsRequest::Loading) {
        Q_D(StrongtagsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion strongtag at \a row from \a resourcePath.
*/
void StrongtagsModel::del(int row) {
    if (status() != StrongtagsRequest::Loading) {
        Q_D(StrongtagsModel);
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa StrongtagsRequest::cancel()
*/
void StrongtagsModel::cancel() {
    Q_D(StrongtagsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion strongtag resources 
    using the existing parameters.
*/
void StrongtagsModel::reload() {
    if (status() != StrongtagsRequest::Loading) {
        Q_D(StrongtagsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_strongtagsmodel.cpp"
