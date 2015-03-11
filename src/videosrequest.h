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
 
#ifndef VIDEOSREQUEST_H
#define VIDEOSREQUEST_H

#include "request.h"
#include <QStringList>

namespace QDailymotion {

class QDAILYMOTIONSHARED_EXPORT VideosRequest : public Request
{
    Q_OBJECT
    
public:
    explicit VideosRequest(QObject *parent = 0);
    
public Q_SLOTS:
    void list(const QString &resourcePath = QString(), const QVariantMap &filters = QVariantMap(),
              const QStringList &fields = QStringList());
    
    void get(const QString &id, const QVariantMap &filters = QVariantMap(), const QStringList &fields = QStringList());
        
    void insert(const QString &id, const QString &resourcePath);
    
    void update(const QString &id, const QVariantMap &resource);
    
    void del(const QString &id, const QString &resourcePath = QString());

private:
    Q_DISABLE_COPY(VideosRequest)
};

}

#endif // VIDEOSREQUEST_H
