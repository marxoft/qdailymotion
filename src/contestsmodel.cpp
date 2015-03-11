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

#include "contestsmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class ContestsModelPrivate : public ModelPrivate
{

public:
    ContestsModelPrivate(ContestsModel *parent) :
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
        qDebug() << "ContestsModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(ContestsModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ContestsModel);
    
        if (request->status() == ContestsRequest::Ready) {
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
        
        ContestsModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    ContestsRequest *request;
    
    QString resourcePath;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(ContestsModel)
};

/*!
    \class ContestsModel
    \brief A list model for displaying Dailymotion contest resources.
    
    \ingroup contests
    \ingroup models
    
    The ContestsModel is a list model used for displaying Dailymotion contests in a list view. ContestsModel 
    provides the same methods that are available in ContestsRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by ContestsModel depends on which fields are specified when calling list(). If fields are 
    specified, ContestsModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, ContestsModel provides the following 
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
    ContestsModel *model = new ContestsModel(this);
    view->setModel(new ContestsModel(this));
    
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
        model: ContestsModel {
            id: contestsModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: name
        }
        
        Component.onCompleted: contestsModel.list("", {limit: 20, family_filter: true})
    }
    \endcode
    
    \sa ContestsRequest
*/

/*!
    \enum ContestsModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

ContestsModel::ContestsModel(QObject *parent) :
    Model(*new ContestsModelPrivate(this), parent)
{
    Q_D(ContestsModel);
    
    d->request = new ContestsRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString ContestsModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa ContestsRequest::clientId
*/

/*!
    \fn void ContestsModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString ContestsModel::clientId() const {
    Q_D(const ContestsModel);
    
    return d->request->clientId();
}

void ContestsModel::setClientId(const QString &id) {
    Q_D(ContestsModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString ContestsModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa ContestsRequest::clientSecret
*/

/*!
    \fn void ContestsModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString ContestsModel::clientSecret() const {
    Q_D(const ContestsModel);
    
    return d->request->clientSecret();
}

void ContestsModel::setClientSecret(const QString &secret) {
    Q_D(ContestsModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString ContestsModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a contest's protected resources.
    
    \as ContestsRequest::accessToken
*/

/*!
    \fn void ContestsModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString ContestsModel::accessToken() const {
    Q_D(const ContestsModel);
    
    return d->request->accessToken();
}

void ContestsModel::setAccessToken(const QString &token) {
    Q_D(ContestsModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString ContestsModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa ContestsRequest::refreshToken
*/

/*!
    \fn void ContestsModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString ContestsModel::refreshToken() const {
    Q_D(const ContestsModel);
    
    return d->request->refreshToken();
}

void ContestsModel::setRefreshToken(const QString &token) {
    Q_D(ContestsModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum ContestsModel::status
    \brief The current status of the model.
    
    \sa ContestsRequest::status
*/

/*!
    \fn void ContestsModel::statusChanged()
    \brief Emitted when the status changes.
*/
ContestsRequest::Status ContestsModel::status() const {
    Q_D(const ContestsModel);
    
    return d->request->status();
}

/*!
    \property enum ContestsModel::error
    \brief The error type of the model.
    
    \sa ContestsRequest::error
*/
ContestsRequest::Error ContestsModel::error() const {
    Q_D(const ContestsModel);
    
    return d->request->error();
}

/*!
    \property enum ContestsModel::errorString
    \brief A description of the error of the model.
    
    \sa ContestsRequest::status
*/
QString ContestsModel::errorString() const {
    Q_D(const ContestsModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    ContestsModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa ContestsRequest::setNetworkAccessManager()
*/
void ContestsModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(ContestsModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool ContestsModel::canFetchMore(const QModelIndex &) const {
    if (status() == ContestsRequest::Loading) {
        return false;
    }
    
    Q_D(const ContestsModel);
    
    return d->hasMore;
}

void ContestsModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(ContestsModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion contest resources belonging to \a resourcePath.
        
    \sa ContestsRequest::list()
*/
void ContestsModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != ContestsRequest::Loading) {
        Q_D(ContestsModel);
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
    
    \sa ContestsRequest::cancel()
*/
void ContestsModel::cancel() {
    Q_D(ContestsModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion contest resources 
    using the existing parameters.
*/
void ContestsModel::reload() {
    if (status() != ContestsRequest::Loading) {
        Q_D(ContestsModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_contestsmodel.cpp"
