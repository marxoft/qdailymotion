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

#include "subtitlesmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class SubtitlesModelPrivate : public ModelPrivate
{

public:
    SubtitlesModelPrivate(SubtitlesModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "language";
            roles[Qt::UserRole + 3] = "url";
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
        qDebug() << "SubtitlesModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(SubtitlesModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(SubtitlesModel);
    
        if (request->status() == SubtitlesRequest::Ready) {
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
        
        SubtitlesModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onInsertRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(SubtitlesModel);
    
        if (request->status() == SubtitlesRequest::Ready) {
            QVariantMap result = request->result().toMap();
        
            if (!result.isEmpty()) {
                q->beginInsertRows(QModelIndex(), 0, 0);
                items.prepend(result);
                q->endInsertRows();
                emit q->countChanged();
            }
        }
        
        SubtitlesModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onInsertRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onUpdateRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(SubtitlesModel);
    
        if (request->status() == SubtitlesRequest::Ready) {
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
        
        SubtitlesModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onUpdateRequestFinished()));
    
        emit q->statusChanged();
    }
    
    void _q_onDeleteRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(SubtitlesModel);
    
        if (request->status() == SubtitlesRequest::Ready) {
            QModelIndexList indexes = q->match(QModelIndex(), Qt::UserRole + 1, delId, 1, Qt::MatchExactly);
            
            if (!indexes.isEmpty()) {
                QModelIndex index = indexes.first();
                q->beginRemoveRows(QModelIndex(), index.row(), index.row());
                items.removeAt(index.row());
                q->endRemoveRows();
                emit q->countChanged();
            }
        }
        
        SubtitlesModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onDeleteRequestFinished()));
    
        emit q->statusChanged();
    }
    
    SubtitlesRequest *request;
    
    QString resourcePath;
    QString delId;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(SubtitlesModel)
};

/*!
    \class SubtitlesModel
    \brief A list model for displaying Dailymotion subtitle resources.
    
    \ingroup subtitles
    \ingroup models
    
    The SubtitlesModel is a list model used for displaying Dailymotion subtitles in a list view. SubtitlesModel 
    provides the same methods that are available in SubtitlesRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by SubtitlesModel depends on which fields are specified when calling list(). If fields are 
    specified, SubtitlesModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, SubtitlesModel provides the following 
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
            <td>language</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 3</td>
            <td>url</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    SubtitlesModel *model = new SubtitlesModel(this);
    view->setModel(new SubtitlesModel(this));
    
    QVariantMap filters;
    filters["limit"] = 20;
    model->list("/video/VIDEO_ID/subtitles", filters);
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    ListView {
        id: view
        
        width: 800
        height: 480
        model: SubtitlesModel {
            id: subtitlesModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: langauge
        }
        
        Component.onCompleted: subtitlesModel.list("/video/VIDEO_ID/subtitles", {limit: 20})
    }
    \endcode
    
    \sa SubtitlesRequest
*/

/*!
    \enum SubtitlesModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

SubtitlesModel::SubtitlesModel(QObject *parent) :
    Model(*new SubtitlesModelPrivate(this), parent)
{
    Q_D(SubtitlesModel);
    
    d->request = new SubtitlesRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString SubtitlesModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa SubtitlesRequest::clientId
*/

/*!
    \fn void SubtitlesModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString SubtitlesModel::clientId() const {
    Q_D(const SubtitlesModel);
    
    return d->request->clientId();
}

void SubtitlesModel::setClientId(const QString &id) {
    Q_D(SubtitlesModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString SubtitlesModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa SubtitlesRequest::clientSecret
*/

/*!
    \fn void SubtitlesModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString SubtitlesModel::clientSecret() const {
    Q_D(const SubtitlesModel);
    
    return d->request->clientSecret();
}

void SubtitlesModel::setClientSecret(const QString &secret) {
    Q_D(SubtitlesModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString SubtitlesModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a subtitle's protected resources.
    
    \as SubtitlesRequest::accessToken
*/

/*!
    \fn void SubtitlesModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString SubtitlesModel::accessToken() const {
    Q_D(const SubtitlesModel);
    
    return d->request->accessToken();
}

void SubtitlesModel::setAccessToken(const QString &token) {
    Q_D(SubtitlesModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString SubtitlesModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa SubtitlesRequest::refreshToken
*/

/*!
    \fn void SubtitlesModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString SubtitlesModel::refreshToken() const {
    Q_D(const SubtitlesModel);
    
    return d->request->refreshToken();
}

void SubtitlesModel::setRefreshToken(const QString &token) {
    Q_D(SubtitlesModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum SubtitlesModel::status
    \brief The current status of the model.
    
    \sa SubtitlesRequest::status
*/

/*!
    \fn void SubtitlesModel::statusChanged()
    \brief Emitted when the status changes.
*/
SubtitlesRequest::Status SubtitlesModel::status() const {
    Q_D(const SubtitlesModel);
    
    return d->request->status();
}

/*!
    \property enum SubtitlesModel::error
    \brief The error type of the model.
    
    \sa SubtitlesRequest::error
*/
SubtitlesRequest::Error SubtitlesModel::error() const {
    Q_D(const SubtitlesModel);
    
    return d->request->error();
}

/*!
    \property enum SubtitlesModel::errorString
    \brief A description of the error of the model.
    
    \sa SubtitlesRequest::status
*/
QString SubtitlesModel::errorString() const {
    Q_D(const SubtitlesModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    SubtitlesModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa SubtitlesRequest::setNetworkAccessManager()
*/
void SubtitlesModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(SubtitlesModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool SubtitlesModel::canFetchMore(const QModelIndex &) const {
    if (status() == SubtitlesRequest::Loading) {
        return false;
    }
    
    Q_D(const SubtitlesModel);
    
    return d->hasMore;
}

void SubtitlesModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(SubtitlesModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion subtitle resources belonging to \a resourcePath.
        
    \sa SubtitlesRequest::list()
*/
void SubtitlesModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != SubtitlesRequest::Loading) {
        Q_D(SubtitlesModel);
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
    \brief Inserts a new Dailymotion subtitle into the current resourcePath.
    
    \sa SubtitlesRequest::insert()
*/
void SubtitlesModel::insert(const QVariantMap &resource) {
    if (status() != SubtitlesRequest::Loading) {
        Q_D(SubtitlesModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onInsertRequestFinished()));
        d->request->insert(resource, d->resourcePath);
        emit statusChanged();
    }
}

/*!
    \brief Updates the Dailymotion subtitle at \a row with \a resource.
*/
void SubtitlesModel::update(int row, const QVariantMap &resource) {
    if (status() != SubtitlesRequest::Loading) {
        Q_D(SubtitlesModel);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onUpdateRequestFinished()));
        d->request->update(data(index(row), Qt::UserRole + 1).toString(), resource);
        emit statusChanged();
    }
}

/*!
    \brief Deletes the Dailymotion subtitle at \a row from \a resourcePath.
*/
void SubtitlesModel::del(int row) {
    if (status() != SubtitlesRequest::Loading) {
        Q_D(SubtitlesModel);
        d->delId = data(index(row), Qt::UserRole + 1).toString();
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onDeleteRequestFinished()));
        d->request->del(d->delId);
        emit statusChanged();
    }
}

/*!
    \brief Cancels the current request.
    
    \sa SubtitlesRequest::cancel()
*/
void SubtitlesModel::cancel() {
    Q_D(SubtitlesModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion subtitle resources 
    using the existing parameters.
*/
void SubtitlesModel::reload() {
    if (status() != SubtitlesRequest::Loading) {
        Q_D(SubtitlesModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_subtitlesmodel.cpp"
