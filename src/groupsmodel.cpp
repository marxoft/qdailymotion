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

#include "groupsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class GroupsModelPrivate : public ModelPrivate
{

public:
    GroupsModelPrivate(GroupsModel *parent) :
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
        qDebug() << "GroupsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(GroupsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(GroupsModel);
    
        if (request->status() == GroupsRequest::Ready) {
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
        
        GroupsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    GroupsRequest *request;
    
    QString resourcePath;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(GroupsModel)
};

/*!
    \class GroupsModel
    \brief A list model for displaying Dailymotion group resources.
    
    \ingroup groups
    \ingroup models
    
    The GroupsModel is a list model used for displaying Dailymotion groups in a list view. GroupsModel 
    provides the same methods that are available in GroupsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by GroupsModel depends on which fields are specified when calling list(). If fields are 
    specified, GroupsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, GroupsModel provides the following 
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
    GroupsModel *model = new GroupsModel(this);
    view->setModel(new GroupsModel(this));
    
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
        model: GroupsModel {
            id: groupsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: name
        }
        
        Component.onCompleted: groupsModel.list("", {limit: 20, family_filter: true})
    }
    \endcode
    
    \sa GroupsRequest
*/

/*!
    \enum GroupsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

GroupsModel::GroupsModel(QObject *parent) :
    Model(*new GroupsModelPrivate(this), parent)
{
    Q_D(GroupsModel);
    
    d->request = new GroupsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString GroupsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa GroupsRequest::clientId
*/

/*!
    \fn void GroupsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString GroupsModel::clientId() const {
    Q_D(const GroupsModel);
    
    return d->request->clientId();
}

void GroupsModel::setClientId(const QString &id) {
    Q_D(GroupsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString GroupsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa GroupsRequest::clientSecret
*/

/*!
    \fn void GroupsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString GroupsModel::clientSecret() const {
    Q_D(const GroupsModel);
    
    return d->request->clientSecret();
}

void GroupsModel::setClientSecret(const QString &secret) {
    Q_D(GroupsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString GroupsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a group's protected resources.
    
    \as GroupsRequest::accessToken
*/

/*!
    \fn void GroupsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString GroupsModel::accessToken() const {
    Q_D(const GroupsModel);
    
    return d->request->accessToken();
}

void GroupsModel::setAccessToken(const QString &token) {
    Q_D(GroupsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString GroupsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa GroupsRequest::refreshToken
*/

/*!
    \fn void GroupsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString GroupsModel::refreshToken() const {
    Q_D(const GroupsModel);
    
    return d->request->refreshToken();
}

void GroupsModel::setRefreshToken(const QString &token) {
    Q_D(GroupsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum GroupsModel::status
    \brief The current status of the model.
    
    \sa GroupsRequest::status
*/

/*!
    \fn void GroupsModel::statusChanged()
    \brief Emitted when the status changes.
*/
GroupsRequest::Status GroupsModel::status() const {
    Q_D(const GroupsModel);
    
    return d->request->status();
}

/*!
    \property enum GroupsModel::error
    \brief The error type of the model.
    
    \sa GroupsRequest::error
*/
GroupsRequest::Error GroupsModel::error() const {
    Q_D(const GroupsModel);
    
    return d->request->error();
}

/*!
    \property enum GroupsModel::errorString
    \brief A description of the error of the model.
    
    \sa GroupsRequest::status
*/
QString GroupsModel::errorString() const {
    Q_D(const GroupsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    GroupsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa GroupsRequest::setNetworkAccessManager()
*/
void GroupsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(GroupsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool GroupsModel::canFetchMore(const QModelIndex &) const {
    if (status() == GroupsRequest::Loading) {
        return false;
    }
    
    Q_D(const GroupsModel);
    
    return d->hasMore;
}

void GroupsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(GroupsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion group resources belonging to \a resourcePath.
        
    \sa GroupsRequest::list()
*/
void GroupsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != GroupsRequest::Loading) {
        Q_D(GroupsModel);
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
    
    \sa GroupsRequest::cancel()
*/
void GroupsModel::cancel() {
    Q_D(GroupsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion group resources 
    using the existing parameters.
*/
void GroupsModel::reload() {
    if (status() != GroupsRequest::Loading) {
        Q_D(GroupsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_groupsmodel.cpp"
