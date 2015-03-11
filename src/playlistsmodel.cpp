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

#include "playlistsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class PlaylistsModelPrivate : public ModelPrivate
{

public:
    PlaylistsModelPrivate(PlaylistsModel *parent) :
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
            roles[Qt::UserRole + 3] = "owner";
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
        qDebug() << "PlaylistsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(PlaylistsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(PlaylistsModel);
    
        if (request->status() == PlaylistsRequest::Ready) {
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
        
        PlaylistsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(PlaylistsModel);
    
        if (request->status() == PlaylistsRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        PlaylistsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(PlaylistsModel);
    
        if (request->status() == PlaylistsRequest::Ready) {
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
        
        PlaylistsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(PlaylistsModel);
    
        if (request->status() == PlaylistsRequest::Ready) {
            QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, delId, 1, Qt::MatchExactly);
            
            if (!indexes.isEmpty()) {
                QModelIndex index = indexes.first();
                q->beginRemoveRows(QModelIndex(), index.row(), index.row());
                items.removeAt(index.row());
                q->endRemoveRows();
                emit q->countChanged();
            }
        }
        
        PlaylistsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    PlaylistsRequest *request;
    
    QString resourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(PlaylistsModel)
};

/*!
    \class PlaylistsModel
    \brief A list model for displaying Dailymotion playlist resources.
    
    \ingroup playlists
    \ingroup models
    
    The PlaylistsModel is a list model used for displaying Dailymotion playlists in a list view. PlaylistsModel 
    provides the same methods that are available in PlaylistsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by PlaylistsModel depends on which fields are specified when calling list(). If fields are 
    specified, PlaylistsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, PlaylistsModel provides the following 
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
        <tr>
            <td>Qt::UserRole + 3</td>
            <td>owner</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    PlaylistsModel *model = new PlaylistsModel(this);
    view->setModel(new PlaylistsModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    filters["family_filter"] = true;
    filters["search"] = "Qt";
    model->list("", filters, QStringList() << "id" << "owner.screenname" << "name", "thumbnail_120_url");
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: PlaylistsModel {
            id: playlistsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: title
        }
        
        Component.onCompleted: playlistsModel.list("", {limit: 20, family_filter: true, search: "Qt"},
                                                   ["id", "owner.screenname", "name", "thumbnail_120_url"])
    }
    \endcode
    
    \sa PlaylistsRequest
*/

/*!
    \enum PlaylistsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

PlaylistsModel::PlaylistsModel(QObject *parent) :
    Model(*new PlaylistsModelPrivate(this), parent)
{
    Q_D(PlaylistsModel);
    
    d->request = new PlaylistsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString PlaylistsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa PlaylistsRequest::clientId
*/

/*!
    \fn void PlaylistsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString PlaylistsModel::clientId() const {
    Q_D(const PlaylistsModel);
    
    return d->request->clientId();
}

void PlaylistsModel::setClientId(const QString &id) {
    Q_D(PlaylistsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString PlaylistsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa PlaylistsRequest::clientSecret
*/

/*!
    \fn void PlaylistsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString PlaylistsModel::clientSecret() const {
    Q_D(const PlaylistsModel);
    
    return d->request->clientSecret();
}

void PlaylistsModel::setClientSecret(const QString &secret) {
    Q_D(PlaylistsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString PlaylistsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a playlist's protected resources.
    
    \as PlaylistsRequest::accessToken
*/

/*!
    \fn void PlaylistsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString PlaylistsModel::accessToken() const {
    Q_D(const PlaylistsModel);
    
    return d->request->accessToken();
}

void PlaylistsModel::setAccessToken(const QString &token) {
    Q_D(PlaylistsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString PlaylistsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa PlaylistsRequest::refreshToken
*/

/*!
    \fn void PlaylistsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString PlaylistsModel::refreshToken() const {
    Q_D(const PlaylistsModel);
    
    return d->request->refreshToken();
}

void PlaylistsModel::setRefreshToken(const QString &token) {
    Q_D(PlaylistsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum PlaylistsModel::status
    \brief The current status of the model.
    
    \sa PlaylistsRequest::status
*/

/*!
    \fn void PlaylistsModel::statusChanged()
    \brief Emitted when the status changes.
*/
PlaylistsRequest::Status PlaylistsModel::status() const {
    Q_D(const PlaylistsModel);
    
    return d->request->status();
}

/*!
    \property enum PlaylistsModel::error
    \brief The error type of the model.
    
    \sa PlaylistsRequest::error
*/
PlaylistsRequest::Error PlaylistsModel::error() const {
    Q_D(const PlaylistsModel);
    
    return d->request->error();
}

/*!
    \property enum PlaylistsModel::errorString
    \brief A description of the error of the model.
    
    \sa PlaylistsRequest::status
*/
QString PlaylistsModel::errorString() const {
    Q_D(const PlaylistsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    PlaylistsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa PlaylistsRequest::setNetworkAccessManager()
*/
void PlaylistsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(PlaylistsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool PlaylistsModel::canFetchMore(const QModelIndex &) const {
    if (status() == PlaylistsRequest::Loading) {
        return false;
    }
    
    Q_D(const PlaylistsModel);
    
    return d->hasMore;
}

void PlaylistsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(PlaylistsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion playlist resources belonging to \a resourcePath.
        
    \sa PlaylistsRequest::list()
*/
void PlaylistsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != PlaylistsRequest::Loading) {
        Q_D(PlaylistsModel);
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
    \brief Inserts a new Dailymotion playlist into the current resourcePath.
    
    \sa PlaylistsRequest::insert()
*/
void PlaylistsModel::insert(const QVariantMap &resource) {
    if (status() != PlaylistsRequest::Loading) {
        Q_D(PlaylistsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(resource);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion playlist at \a row with \a resource.
*/
void PlaylistsModel::update(int row, const QVariantMap &resource) {
    if (status() != PlaylistsRequest::Loading) {
        Q_D(PlaylistsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion playlist at \a row.
*/
void PlaylistsModel::del(int row) {
    if (status() != PlaylistsRequest::Loading) {
        Q_D(PlaylistsModel);
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa PlaylistsRequest::cancel()
*/
void PlaylistsModel::cancel() {
    Q_D(PlaylistsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion playlist resources 
    using the existing parameters.
*/
void PlaylistsModel::reload() {
    if (status() != PlaylistsRequest::Loading) {
        Q_D(PlaylistsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_playlistsmodel.cpp"
