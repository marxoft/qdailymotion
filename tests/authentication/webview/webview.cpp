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

#include "webview.h"
#include "urls.h"
#include <QSettings>
#include <QMessageBox>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

WebView::WebView(QWidget *parent) :
    QWebView(parent)
{
    QSettings settings;
    request.setClientId(settings.value("Authentication/clientId").toString());
    request.setClientSecret(settings.value("Authentication/clientSecret").toString());
    request.setRedirectUri(settings.value("Authentication/redirectUri").toString());
    
    connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        
    QUrl u(QDailymotion::AUTH_URL);
#if QT_VERSION >= 0x050000
    QUrlQuery query(u);
    query.addQueryItem("client_id", request.clientId());
    query.addQueryItem("redirect_uri", settings.value("Authentication/redirectUri").toString());
    query.addQueryItem("response_type", "code");
    query.addQueryItem("display", "popup");
    u.setQuery(query);
#else
    u.addQueryItem("client_id", request.clientId());
    u.addQueryItem("redirect_uri", settings.value("Authentication/redirectUri").toString());
    u.addQueryItem("response_type", "code");
    u.addQueryItem("display", "popup");
#endif
#ifdef QDAILYMOTION_DEBUG
    qDebug() << "WebView::setUrl" << u;
#endif
    setWindowTitle(tr("Dailymotion authentication"));
    setUrl(u);
}

void WebView::onUrlChanged(const QUrl &u) {
#ifdef QDAILYMOTION_DEBUG
    qDebug() << "WebView::onUrlChanged" << u;
#endif
#if QT_VERSION >= 0x050000
    QUrlQuery query(u);
    
    if (query.hasQueryItem("code")) {
        request.exchangeCodeForAccessToken(query.queryItemValue("code"));
    }
#else
    if (u.hasQueryItem("code")) {
        request.exchangeCodeForAccessToken(u.queryItemValue("code"));
    }
#endif
}

void WebView::onRequestFinished() {
    if (request.status() == QDailymotion::AuthenticationRequest::Ready) {
        QVariantMap result = request.result().toMap();
        QString message = tr("Access token: %1\n\nExpires in: %2\n\nRefresh token: %3")
                            .arg(result.value("access_token").toString())
                            .arg(result.value("expires_in").toString())
                            .arg(result.value("refresh_token").toString());
                                                                
        QMessageBox::information(this, tr("Authentication sucessful"), message);
    }
    else {
        QMessageBox::information(this, tr("Authentication failed"), request.errorString());
    }
}
