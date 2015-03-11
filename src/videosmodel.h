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

#ifndef VIDEOSMODEL_H
#define VIDEOSMODEL_H

#include "model.h"
#include "videosrequest.h"

namespace QDailymotion {

class VideosModelPrivate;

class QDAILYMOTIONSHARED_EXPORT VideosModel : public Model
{
    Q_OBJECT
    
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString refreshToken READ refreshToken WRITE setRefreshToken NOTIFY refreshTokenChanged)
    Q_PROPERTY(QDailymotion::VideosRequest::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QDailymotion::VideosRequest::Error error READ error NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
                
public:
    explicit VideosModel(QObject *parent = 0);
    
    QString clientId() const;
    void setClientId(const QString &id);
    
    QString clientSecret() const;
    void setClientSecret(const QString &secret);
    
    QString accessToken() const;
    void setAccessToken(const QString &token);
    
    QString refreshToken() const;
    void setRefreshToken(const QString &token);
    
    VideosRequest::Status status() const;
    
    VideosRequest::Error error() const;
    QString errorString() const;
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    void fetchMore(const QModelIndex &parent = QModelIndex());
    
public Q_SLOTS:
    void list(const QString &resourcePath = QString(), const QVariantMap &filters = QVariantMap(),
              const QStringList &fields = QStringList());
            
    void insert(int row, const QString &resourcePath);
    
    void update(int row, const QVariantMap &resource);
    
    void del(int row, const QString &resourcePath = QString());
    
    void cancel();
    void reload();
    
Q_SIGNALS:
    void clientIdChanged();
    void clientSecretChanged();
    void accessTokenChanged();
    void refreshTokenChanged();
    void statusChanged();
    
private:        
    Q_DECLARE_PRIVATE(VideosModel)
    Q_DISABLE_COPY(VideosModel)
    
    Q_PRIVATE_SLOT(d_func(), void _q_onListRequestFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_onInsertRequestFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_onUpdateRequestFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_onDeleteRequestFinished())
};

}

#endif // VIDEOSMODEL_H
