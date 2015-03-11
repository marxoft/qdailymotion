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

#include "activitiesmodel.h"
#include "model_p.h"
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class ActivitiesModelPrivate : public ModelPrivate
{

public:
    ActivitiesModelPrivate(ActivitiesModel *parent) :
        ModelPrivate(parent),
        request(0),
        hasMore(false)
    {
    }
        
    void setRoleNames() {
        roles.clear();
        
        if (fields.isEmpty()) {
            roles[Qt::UserRole + 1] = "id";
            roles[Qt::UserRole + 2] = "from_tile";
            roles[Qt::UserRole + 3] = "object_tile";
            roles[Qt::UserRole + 4] = "type";
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
        qDebug() << "ActivitiesModelPrivate::setRoleNames" << roles;
#endif
#if QT_VERSION < 0x050000
        Q_Q(ActivitiesModel);
        
        q->setRoleNames(roles);
#endif
    }
        
    void _q_onListRequestFinished() {
        if (!request) {
            return;
        }
    
        Q_Q(ActivitiesModel);
    
        if (request->status() == ActivitiesRequest::Ready) {
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
        
        ActivitiesModel::disconnect(request, SIGNAL(finished()), q, SLOT(_q_onListRequestFinished()));
    
        emit q->statusChanged();
    }
    
    ActivitiesRequest *request;
    
    QString resourcePath;
    QVariantMap filters;
    QStringList fields;
        
    bool hasMore;
    
    Q_DECLARE_PUBLIC(ActivitiesModel)
};

/*!
    \class ActivitiesModel
    \brief A list model for displaying Dailymotion activity resources.
    
    \ingroup activities
    \ingroup models
    
    The ActivitiesModel is a list model used for displaying Dailymotion activities in a list view. ActivitiesModel 
    provides the same methods that are available in ActivitiesRequest, so it is better to simply use that class if you 
    do not need the additional features provided by a data model.
    
    The roles provided by ActivitiesModel depends on which fields are specified when calling list(). If fields are 
    specified, ActivitiesModel will provide roles starting from Qt::UserRole + 2 for the first field specified. 
    Qt::UserRole + 1 is reserved for the 'id' field. If no fields are specified, ActivitiesModel provides the following 
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
            <td>from_title</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 3</td>
            <td>object_title</td>
            <td>QString</td>
        </tr>
        <tr>
            <td>Qt::UserRole + 4</td>
            <td>type</td>
            <td>QString</td>
        </tr>
    </table>
        
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    QListView *view = new QListView(this);
    ActivitiesModel *model = new ActivitiesModel(this);
    view->setModel(new ActivitiesModel(this));
    
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
        model: ActivitiesModel {
            id: activitiesModel
        }
        delegate: Text {
            width: view.width
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: type
        }
        
        Component.onCompleted: activitiesModel.list("", {limit: 20, family_filter: true})
    }
    \endcode
    
    \sa ActivitiesRequest
*/

/*!
    \enum ActivitiesModel::Roles
    \brief The data roles available for accessing the model data.
    
    See the detailed description for the list of available roles.
*/

ActivitiesModel::ActivitiesModel(QObject *parent) :
    Model(*new ActivitiesModelPrivate(this), parent)
{
    Q_D(ActivitiesModel);
    
    d->request = new ActivitiesRequest(this);
    connect(d->request, SIGNAL(clientIdChanged()), this, SIGNAL(clientIdChanged()));
    connect(d->request, SIGNAL(clientSecretChanged()), this, SIGNAL(clientSecretChanged()));
    connect(d->request, SIGNAL(accessTokenChanged()), this, SIGNAL(accessTokenChanged()));
    connect(d->request, SIGNAL(refreshTokenChanged()), this, SIGNAL(refreshTokenChanged()));
}

/*!
    \property QString ActivitiesModel::clientId
    \brief The client id to be used when making requests to the Dailymotion Data API.
    
    The client id is used only when the access token needs to be refreshed.
    
    \sa ActivitiesRequest::clientId
*/

/*!
    \fn void ActivitiesModel::clientIdChanged()
    \brief Emitted when the clientId changes.
*/
QString ActivitiesModel::clientId() const {
    Q_D(const ActivitiesModel);
    
    return d->request->clientId();
}

void ActivitiesModel::setClientId(const QString &id) {
    Q_D(ActivitiesModel);
    
    d->request->setClientId(id);
}

/*!
    \property QString ActivitiesModel::clientSecret
    \brief The client secret to be used when making requests to the Dailymotion Data API.
    
    The client secret is used only when the access token needs to be refreshed.
    
    \sa ActivitiesRequest::clientSecret
*/

/*!
    \fn void ActivitiesModel::clientSecretChanged()
    \brief Emitted when the clientSecret changes.
*/
QString ActivitiesModel::clientSecret() const {
    Q_D(const ActivitiesModel);
    
    return d->request->clientSecret();
}

void ActivitiesModel::setClientSecret(const QString &secret) {
    Q_D(ActivitiesModel);
    
    d->request->setClientSecret(secret);
}

/*!
    \property QString ActivitiesModel::accessToken
    \brief The access token to be used when making requests to the Dailymotion Data API.
    
    The access token is required when accessing a activity's protected resources.
    
    \as ActivitiesRequest::accessToken
*/

/*!
    \fn void ActivitiesModel::accessTokenChanged()
    \brief Emitted when the accessToken changes.
*/
QString ActivitiesModel::accessToken() const {
    Q_D(const ActivitiesModel);
    
    return d->request->accessToken();
}

void ActivitiesModel::setAccessToken(const QString &token) {
    Q_D(ActivitiesModel);
    
    d->request->setAccessToken(token);
}

/*!
    \property QString ActivitiesModel::refreshToken
    \brief The refresh token to be used when making requests to the Dailymotion Data API.
    
    The refresh token is used only when the accessToken needs to be refreshed.
    
    \sa ActivitiesRequest::refreshToken
*/

/*!
    \fn void ActivitiesModel::refreshTokenChanged()
    \brief Emitted when the refreshToken changes.
*/
QString ActivitiesModel::refreshToken() const {
    Q_D(const ActivitiesModel);
    
    return d->request->refreshToken();
}

void ActivitiesModel::setRefreshToken(const QString &token) {
    Q_D(ActivitiesModel);
    
    d->request->setRefreshToken(token);
}

/*!
    \property enum ActivitiesModel::status
    \brief The current status of the model.
    
    \sa ActivitiesRequest::status
*/

/*!
    \fn void ActivitiesModel::statusChanged()
    \brief Emitted when the status changes.
*/
ActivitiesRequest::Status ActivitiesModel::status() const {
    Q_D(const ActivitiesModel);
    
    return d->request->status();
}

/*!
    \property enum ActivitiesModel::error
    \brief The error type of the model.
    
    \sa ActivitiesRequest::error
*/
ActivitiesRequest::Error ActivitiesModel::error() const {
    Q_D(const ActivitiesModel);
    
    return d->request->error();
}

/*!
    \property enum ActivitiesModel::errorString
    \brief A description of the error of the model.
    
    \sa ActivitiesRequest::status
*/
QString ActivitiesModel::errorString() const {
    Q_D(const ActivitiesModel);
    
    return d->request->errorString();
}

/*!
    \brief Sets the QNetworkAccessManager instance to be used when making requests to the Dailymotion Data API.
    
    ActivitiesModel does not take ownership of \a manager.
    
    If no QNetworkAccessManager is set, one will be created when required.
    
    \sa ActivitiesRequest::setNetworkAccessManager()
*/
void ActivitiesModel::setNetworkAccessManager(QNetworkAccessManager *manager) {
    Q_D(ActivitiesModel);
    
    d->request->setNetworkAccessManager(manager);
}

bool ActivitiesModel::canFetchMore(const QModelIndex &) const {
    if (status() == ActivitiesRequest::Loading) {
        return false;
    }
    
    Q_D(const ActivitiesModel);
    
    return d->hasMore;
}

void ActivitiesModel::fetchMore(const QModelIndex &) {
    if (canFetchMore()) {
        Q_D(ActivitiesModel);
        
        int page = d->filters.value("page").toInt();
        d->filters["page"] = (page > 0 ? page + 1 : 2);
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

/*!
    \brief Retrieves a list of Dailymotion activity resources belonging to \a resourcePath.
        
    \sa ActivitiesRequest::list()
*/
void ActivitiesModel::list(const QString &resourcePath, const QVariantMap &filters, const QStringList &fields) {
    if (status() != ActivitiesRequest::Loading) {
        Q_D(ActivitiesModel);
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
    
    \sa ActivitiesRequest::cancel()
*/
void ActivitiesModel::cancel() {
    Q_D(ActivitiesModel);
    
    if (d->request) {
        d->request->cancel();
    }
}

/*!
    \brief Clears any existing data and retreives a new list of Dailymotion activity resources 
    using the existing parameters.
*/
void ActivitiesModel::reload() {
    if (status() != ActivitiesRequest::Loading) {
        Q_D(ActivitiesModel);
        clear();
        d->filters["page"] = 1;
        connect(d->request, SIGNAL(finished()), this, SLOT(_q_onListRequestFinished()));
        d->request->list(d->resourcePath, d->filters, d->fields);
        emit statusChanged();
    }
}

}

#include "moc_activitiesmodel.cpp"
