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

#include "reportsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class ReportsModelPrivate : public ModelPrivate
{

public:
    ReportsModelPrivate(ReportsModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
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
        qDebug() << "ReportsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(ReportsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ReportsModel);
    
        if (request->status() == ReportsRequest::Ready) {
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
        
        ReportsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ReportsModel);
    
        if (request->status() == ReportsRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        ReportsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ReportsModel);
    
        if (request->status() == ReportsRequest::Ready) {
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
        
        ReportsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ReportsModel);
    
        if (request->status() == ReportsRequest::Ready) {
            QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, delId, 1, Qt::MatchExactly);
            
            if (!indexes.isEmpty()) {
                QModelIndex index = indexes.first();
                q->beginRemoveRows(QModelIndex(), index.row(), index.row());
                items.removeAt(index.row());
                q->endRemoveRows();
                emit q->countChanged();
            }
        }
        
        ReportsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    ReportsRequest *request;
    
    QString resourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(ReportsModel)
};

/*!
    \class ReportsModel
    \brief A list model for displaying Dailymotion report resources.
    
    \ingroup reports
    \ingroup models
    
    The ReportsModel is a list model used for displaying Dailymotion reports in a list view. ReportsModel 
    provides the same methods that are available in ReportsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by ReportsModel depends on which fields are specified when calling list(). If fields are 
    specified, ReportsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, ReportsModel provides the following 
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
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    ReportsModel *model = new ReportsModel(this);
    view->setModel(new ReportsModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    model->list("/video/VIDEO_ID/reports", filters, QStringList() << "id");
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: ReportsModel {
            id: reportsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: id
        }
        
        Component.onCompleted: reportsModel.list("/video/VIDEO_ID/reports", {limit: 20}, ["id"])
    }
    \endcode
    
    \sa ReportsRequest
*/

/*!
    \enum ReportsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

ReportsModel::ReportsModel(QObject *parent) :
    Model(*new ReportsModelPrivate(this), parent)
{
    Q_D(ReportsModel);
    
    d->request = new ReportsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString ReportsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa ReportsRequest::clientId
*/

/*!
    \fn void ReportsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString ReportsModel::clientId() const {
    Q_D(const ReportsModel);
    
    return d->request->clientId();
}

void ReportsModel::setClientId(const QString &id) {
    Q_D(ReportsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString ReportsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa ReportsRequest::clientSecret
*/

/*!
    \fn void ReportsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString ReportsModel::clientSecret() const {
    Q_D(const ReportsModel);
    
    return d->request->clientSecret();
}

void ReportsModel::setClientSecret(const QString &secret) {
    Q_D(ReportsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString ReportsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a report's protected resources.
    
    \as ReportsRequest::accessToken
*/

/*!
    \fn void ReportsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString ReportsModel::accessToken() const {
    Q_D(const ReportsModel);
    
    return d->request->accessToken();
}

void ReportsModel::setAccessToken(const QString &token) {
    Q_D(ReportsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString ReportsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa ReportsRequest::refreshToken
*/

/*!
    \fn void ReportsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString ReportsModel::refreshToken() const {
    Q_D(const ReportsModel);
    
    return d->request->refreshToken();
}

void ReportsModel::setRefreshToken(const QString &token) {
    Q_D(ReportsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum ReportsModel::status
    \brief The current status of the model.
    
    \sa ReportsRequest::status
*/

/*!
    \fn void ReportsModel::statusChanged()
    \brief Emitted when the status changes.
*/
ReportsRequest::Status ReportsModel::status() const {
    Q_D(const ReportsModel);
    
    return d->request->status();
}

/*!
    \property enum ReportsModel::error
    \brief The error type of the model.
    
    \sa ReportsRequest::error
*/
ReportsRequest::Error ReportsModel::error() const {
    Q_D(const ReportsModel);
    
    return d->request->error();
}

/*!
    \property enum ReportsModel::errorString
    \brief A description of the error of the model.
    
    \sa ReportsRequest::status
*/
QString ReportsModel::errorString() const {
    Q_D(const ReportsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    ReportsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa ReportsRequest::setNetworkAccessManager()
*/
void ReportsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(ReportsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool ReportsModel::canFetchMore(const QModelIndex &) const {
    if (status() == ReportsRequest::Loading) {
        return false;
    }
    
    Q_D(const ReportsModel);
    
    return d->hasMore;
}

void ReportsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(ReportsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion report resources belonging to \a resourcePath.
        
    \sa ReportsRequest::list()
*/
void ReportsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != ReportsRequest::Loading) {
        Q_D(ReportsModel);
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
    \brief Inserts a new Dailymotion report into current resourcePath.
    
    \sa ReportsRequest::insert()
*/
void ReportsModel::insert(const QVariantMap &resource) {
    if (status() != ReportsRequest::Loading) {
        Q_D(ReportsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(resource, d->resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion report at \a row with \a resource.
*/
void ReportsModel::update(int row, const QVariantMap &resource) {
    if (status() != ReportsRequest::Loading) {
        Q_D(ReportsModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion report at \a row.
*/
void ReportsModel::del(int row) {
    if (status() != ReportsRequest::Loading) {
        Q_D(ReportsModel);
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa ReportsRequest::cancel()
*/
void ReportsModel::cancel() {
    Q_D(ReportsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion report resources 
    using the existing parameters.
*/
void ReportsModel::reload() {
    if (status() != ReportsRequest::Loading) {
        Q_D(ReportsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_reportsmodel.cpp"
