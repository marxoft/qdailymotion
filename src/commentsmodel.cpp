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

#include "commentsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class CommentsModelPrivate : public ModelPrivate
{

public:
    CommentsModelPrivate(CommentsModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "message";
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
        qDebug() << "CommentsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(CommentsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(CommentsModel);
    
        if (request->status() == CommentsRequest::Ready) {
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
        
        CommentsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(CommentsModel);
    
        if (request->status() == CommentsRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        CommentsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(CommentsModel);
    
        if (request->status() == CommentsRequest::Ready) {
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
        
        CommentsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(CommentsModel);
    
        if (request->status() == CommentsRequest::Ready) {
            QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, delId, 1, Qt::MatchExactly);
            
            if (!indexes.isEmpty()) {
                QModelIndex index = indexes.first();
                q->beginRemoveRows(QModelIndex(), index.row(), index.row());
                items.removeAt(index.row());
                q->endRemoveRows();
                emit q->countChanged();
            }
        }
        
        CommentsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    CommentsRequest *request;
    
    QString resourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(CommentsModel)
};

/*!
    \class CommentsModel
    \brief A list model for displaying Dailymotion comment resources.
    
    \ingroup comments
    \ingroup models
    
    The CommentsModel is a list model used for displaying Dailymotion comments in a list view. CommentsModel 
    provides the same methods that are available in CommentsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by CommentsModel depends on which fields are specified when calling list(). If fields are 
    specified, CommentsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, CommentsModel provides the following 
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
            <td>message</td>
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
    CommentsModel *model = new CommentsModel(this);
    view->setModel(new CommentsModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    model->list("/video/VIDEO_ID/comments", filters);
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: CommentsModel {
            id: commentsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: message
        }
        
        Component.onCompleted: commentsModel.list("/video/VIDEO_ID/comments", {limit: 20})
    }
    \endcode
    
    \sa CommentsRequest
*/

/*!
    \enum CommentsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

CommentsModel::CommentsModel(QObject *parent) :
    Model(*new CommentsModelPrivate(this), parent)
{
    Q_D(CommentsModel);
    
    d->request = new CommentsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString CommentsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa CommentsRequest::clientId
*/

/*!
    \fn void CommentsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString CommentsModel::clientId() const {
    Q_D(const CommentsModel);
    
    return d->request->clientId();
}

void CommentsModel::setClientId(const QString &id) {
    Q_D(CommentsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString CommentsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa CommentsRequest::clientSecret
*/

/*!
    \fn void CommentsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString CommentsModel::clientSecret() const {
    Q_D(const CommentsModel);
    
    return d->request->clientSecret();
}

void CommentsModel::setClientSecret(const QString &secret) {
    Q_D(CommentsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString CommentsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a comment's protected resources.
    
    \as CommentsRequest::accessToken
*/

/*!
    \fn void CommentsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString CommentsModel::accessToken() const {
    Q_D(const CommentsModel);
    
    return d->request->accessToken();
}

void CommentsModel::setAccessToken(const QString &token) {
    Q_D(CommentsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString CommentsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa CommentsRequest::refreshToken
*/

/*!
    \fn void CommentsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString CommentsModel::refreshToken() const {
    Q_D(const CommentsModel);
    
    return d->request->refreshToken();
}

void CommentsModel::setRefreshToken(const QString &token) {
    Q_D(CommentsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum CommentsModel::status
    \brief The current status of the model.
    
    \sa CommentsRequest::status
*/

/*!
    \fn void CommentsModel::statusChanged()
    \brief Emitted when the status changes.
*/
CommentsRequest::Status CommentsModel::status() const {
    Q_D(const CommentsModel);
    
    return d->request->status();
}

/*!
    \property enum CommentsModel::error
    \brief The error type of the model.
    
    \sa CommentsRequest::error
*/
CommentsRequest::Error CommentsModel::error() const {
    Q_D(const CommentsModel);
    
    return d->request->error();
}

/*!
    \property enum CommentsModel::errorString
    \brief A description of the error of the model.
    
    \sa CommentsRequest::status
*/
QString CommentsModel::errorString() const {
    Q_D(const CommentsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    CommentsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa CommentsRequest::setNetworkAccessManager()
*/
void CommentsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(CommentsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool CommentsModel::canFetchMore(const QModelIndex &) const {
    if (status() == CommentsRequest::Loading) {
        return false;
    }
    
    Q_D(const CommentsModel);
    
    return d->hasMore;
}

void CommentsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(CommentsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion comment resources belonging to \a resourcePath.
        
    \sa CommentsRequest::list()
*/
void CommentsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != CommentsRequest::Loading) {
        Q_D(CommentsModel);
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
    \brief Inserts a new Dailymotion comment into the current resourcePath.
    
    \sa CommentsRequest::insert()
*/
void CommentsModel::insert(const QVariantMap &resource) {
    if (status() != CommentsRequest::Loading) {
        Q_D(CommentsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(resource, d->resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion comment at \a row with \a resource.
*/
void CommentsModel::update(int row, const QVariantMap &resource) {
    if (status() != CommentsRequest::Loading) {
        Q_D(CommentsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion comment at \a row from \a resourcePath.
*/
void CommentsModel::del(int row) {
    if (status() != CommentsRequest::Loading) {
        Q_D(CommentsModel);
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa CommentsRequest::cancel()
*/
void CommentsModel::cancel() {
    Q_D(CommentsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion comment resources 
    using the existing parameters.
*/
void CommentsModel::reload() {
    if (status() != CommentsRequest::Loading) {
        Q_D(CommentsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_commentsmodel.cpp"
