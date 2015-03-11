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

#include "videosmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class VideosModelPrivate : public ModelPrivate
{

public:
    VideosModelPrivate(VideosModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "channel";
            roles[Qt::UserRole + 3] = "owner";
            roles[Qt::UserRole + 4] = "title";
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
        qDebug() << "VideosModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(VideosModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(VideosModel);
    
        if (request->status() == VideosRequest::Ready) {
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
        
        VideosModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(VideosModel);
    
        if ((request->status() == VideosRequest::Ready) && (writeResourcePath == resourcePath)) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        VideosModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(VideosModel);
    
        if (request->status() == VideosRequest::Ready) {
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
        
        VideosModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(VideosModel);
    
        if ((request->status() == VideosRequest::Ready) &&
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
        
        VideosModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    VideosRequest *request;
    
    QString resourcePath;
    QString writeResourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(VideosModel)
};

/*!
    \class VideosModel
    \brief A list model for displaying Dailymotion video resources.
    
    \ingroup videos
    \ingroup models
    
    The VideosModel is a list model used for displaying Dailymotion videos in a list view. VideosModel 
    provides the same methods that are available in VideosRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by VideosModel depends on which fields are specified when calling list(). If fields are 
    specified, VideosModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, VideosModel provides the following 
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
            <td>channel</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 3</td>
            <td>owner</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 4</td>
            <td>title</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    VideosModel *model = new VideosModel(this);
    view->setModel(new VideosModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    filters["family_filter"] = true;
    filters["sort"] = "relevance";
    filters["search"] = "Qt";
    model->list("", filters, QStringList() << "id" << "owner.screenname" << "title", "thumbnail_120_url");
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: VideosModel {
            id: videosModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: title
        }
        
        Component.onCompleted: videosModel.list("", {limit: 20, family_filter: true, sort: "relevance", search: "Qt"},
                                                ["id", "owner.screenname", "title", "thumbnail_120_url"])
    }
    \endcode
    
    \sa VideosRequest
*/

/*!
    \enum VideosModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

VideosModel::VideosModel(QObject *parent) :
    Model(*new VideosModelPrivate(this), parent)
{
    Q_D(VideosModel);
    
    d->request = new VideosRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString VideosModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa VideosRequest::clientId
*/

/*!
    \fn void VideosModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString VideosModel::clientId() const {
    Q_D(const VideosModel);
    
    return d->request->clientId();
}

void VideosModel::setClientId(const QString &id) {
    Q_D(VideosModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString VideosModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa VideosRequest::clientSecret
*/

/*!
    \fn void VideosModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString VideosModel::clientSecret() const {
    Q_D(const VideosModel);
    
    return d->request->clientSecret();
}

void VideosModel::setClientSecret(const QString &secret) {
    Q_D(VideosModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString VideosModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a video's protected resources.
    
    \as VideosRequest::accessToken
*/

/*!
    \fn void VideosModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString VideosModel::accessToken() const {
    Q_D(const VideosModel);
    
    return d->request->accessToken();
}

void VideosModel::setAccessToken(const QString &token) {
    Q_D(VideosModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString VideosModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa VideosRequest::refreshToken
*/

/*!
    \fn void VideosModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString VideosModel::refreshToken() const {
    Q_D(const VideosModel);
    
    return d->request->refreshToken();
}

void VideosModel::setRefreshToken(const QString &token) {
    Q_D(VideosModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum VideosModel::status
    \brief The current status of the model.
    
    \sa VideosRequest::status
*/

/*!
    \fn void VideosModel::statusChanged()
    \brief Emitted when the status changes.
*/
VideosRequest::Status VideosModel::status() const {
    Q_D(const VideosModel);
    
    return d->request->status();
}

/*!
    \property enum VideosModel::error
    \brief The error type of the model.
    
    \sa VideosRequest::error
*/
VideosRequest::Error VideosModel::error() const {
    Q_D(const VideosModel);
    
    return d->request->error();
}

/*!
    \property enum VideosModel::errorString
    \brief A description of the error of the model.
    
    \sa VideosRequest::status
*/
QString VideosModel::errorString() const {
    Q_D(const VideosModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    VideosModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa VideosRequest::setNetworkAccessManager()
*/
void VideosModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(VideosModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool VideosModel::canFetchMore(const QModelIndex &) const {
    if (status() == VideosRequest::Loading) {
        return false;
    }
    
    Q_D(const VideosModel);
    
    return d->hasMore;
}

void VideosModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(VideosModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion video resources belonging to \a resourcePath.
        
    \sa VideosRequest::list()
*/
void VideosModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != VideosRequest::Loading) {
        Q_D(VideosModel);
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
    \brief Inserts the Dailymotion video at \a row into the \a resourcePath.
    
    \sa VideosRequest::insert()
*/
void VideosModel::insert(int row, const QString &resourcePath) {
    if (status() != VideosRequest::Loading) {
        Q_D(VideosModel);
        d->writeResourcePath = resourcePath;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(data(index(row), Qt::UserRole + 1).toString(), resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion video at \a row with \a resource.
*/
void VideosModel::update(int row, const QVariantMap &resource) {
    if (status() != VideosRequest::Loading) {
        Q_D(VideosModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion video at \a row from \a resourcePath.
*/
void VideosModel::del(int row, const QString &resourcePath) {
    if (status() != VideosRequest::Loading) {
        Q_D(VideosModel);
        d->writeResourcePath = resourcePath;
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId, resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa VideosRequest::cancel()
*/
void VideosModel::cancel() {
    Q_D(VideosModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion video resources 
    using the existing parameters.
*/
void VideosModel::reload() {
    if (status() != VideosRequest::Loading) {
        Q_D(VideosModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_videosmodel.cpp"
