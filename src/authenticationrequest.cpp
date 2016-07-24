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

#include "authenticationrequest.h"
#include "request_p.h"
#include "urls.h"
#include <QNetworkReply>
#include <QStringList>
#ifdef QDAILYMOTION_DEBUG
#include <QDebug>
#endif

namespace QDailymotion {

class AuthenticationRequestPrivate : public RequestPrivate
{

public:
    enum AuthRequest {
        WebToken = 0,
        DeviceToken,
        RevokeToken
    };
    
    AuthenticationRequestPrivate(AuthenticationRequest *parent) :
        RequestPrivate(parent),
        authRequest(WebToken)
    {
    }
    
    void _q_onReplyFinished() {
        if (!reply) {
            return;
        }
    
        Q_Q(AuthenticationRequest);
    
        bool ok;
        setResult(QtJson::Json::parse(reply->readAll(), ok));
        
        const QNetworkReply::NetworkError e = reply->error();
        const QString es = reply->errorString();
        reply->deleteLater();
        reply = 0;
    
        switch (e) {
        case QNetworkReply::NoError:
            break;
        case QNetworkReply::OperationCanceledError:
            setStatus(Request::Canceled);
            setError(Request::NoError);
            setErrorString(QString());
            emit q->finished();
            return;
        case QNetworkReply::AuthenticationRequiredError:
            if ((authRequest == RevokeToken) && (!refreshToken.isEmpty())) {
                refreshAccessToken();
            }
            else {
                setStatus(Request::Failed);
                setError(Request::Error(e));
                setErrorString(es);
                emit q->finished();
            }
        
            return;
        default:
            setStatus(Request::Failed);
            setError(Request::Error(e));
            setErrorString(es);
            emit q->finished();
            return;
        }
    
        if ((ok) || (authRequest == RevokeToken)) {
            setStatus(Request::Ready);
            setError(Request::NoError);
            setErrorString(QString());
        }
        else {
            setStatus(Request::Failed);
            setError(Request::ParseError);
            setErrorString(Request::tr("Unable to parse response"));
        }
    
        emit q->finished();
    }
    
    AuthRequest authRequest;
    
    QString redirectUri;
    
    QStringList scopes;
    
    Q_DECLARE_PUBLIC(AuthenticationRequest)
};

/*!
    \class AuthenticationRequest
    \brief Handles OAuth 2.0 authentication requests.
    
    \ingroup requests
    
    The AuthenticationRequest class is used for obtaining and revoking access tokens for use with the Dailymotion Data 
    API.
     
    AuthenticationRequest supports the <a target="_blank" 
    href="https://developer.dailymotion.com/documentation#oauth-client-web-application">web application</a>, 
    <a target="_blank" href="https://developer.dailymotion.com/documentation#oauth-client-user-agent">user-agent</a> 
    and <a target="_blank" href="https://developer.dailymotion.com/documentation#oauth-client-native-application">native</a> 
    client profiles.
*/
AuthenticationRequest::AuthenticationRequest(QObject *parent) :
    Request(*new AuthenticationRequestPrivate(this), parent)
{
}

/*!
    \property QString AuthenticationRequest::redirectUri
    \brief The uri that the web view will be redirected to during authentication.
*/
QString AuthenticationRequest::redirectUri() const {
    Q_D(const AuthenticationRequest);
    
    return d->redirectUri;
}

void AuthenticationRequest::setRedirectUri(const QString &uri) {
    Q_D(AuthenticationRequest);
    
    if (uri != d->redirectUri) {
        d->redirectUri = uri;
        emit redirectUriChanged();
    }
#if QDAILYMOTION_DEBUG
    qDebug() << "QDailymotion::AuthenticationRequest::setRedirectUri" << uri;
#endif
}

/*!
    \property QStringList AuthenticationRequest::scopes
    \brief The list of scopes for which to request permission.
*/
QStringList AuthenticationRequest::scopes() const {
    Q_D(const AuthenticationRequest);
    
    return d->scopes;
}

void AuthenticationRequest::setScopes(const QStringList &scopes) {
    Q_D(AuthenticationRequest);
    
    d->scopes = scopes;
    emit scopesChanged();
#if QDAILYMOTION_DEBUG
    qDebug() << "AuthenticationRequest::setScopes" << scopes;
#endif
}

/*!
    \brief Submits code in exchange for a Dailymotion access token.
*/
void AuthenticationRequest::exchangeCodeForAccessToken(const QString &code) {
    if (status() == Loading) {
        return;
    }
    
    Q_D(AuthenticationRequest);
    d->authRequest = AuthenticationRequestPrivate::WebToken;
    setUrl(TOKEN_URL);
    setData(QString("code=" + code + "&client_id=" + clientId() + "&client_secret=" + clientSecret() +
                    "&redirect_uri=" + redirectUri() + "&grant_type=" + GRANT_TYPE_CODE));
    post();
}

/*!
    \brief Submits \a username and \a password in exchange for a Dailymotion access token.
*/
void AuthenticationRequest::exchangeCredentialsForAccessToken(const QString &username, const QString &password) {
    if (status() == Loading) {
        return;
    }
    
    Q_D(AuthenticationRequest);
    d->authRequest = AuthenticationRequestPrivate::DeviceToken;
    setUrl(TOKEN_URL);
    setData(QString("username=" + username + "&password=" + password + "&client_id=" + clientId() + "&client_secret=" 
                    + clientSecret() + "&scope=" + scopes().join("+") + "&grant_type=" + GRANT_TYPE_PASSWORD));
    post();
}


/*!
    \brief Revokes Dailymotion Data API access for the current access token.
*/
void AuthenticationRequest::revokeAccessToken() {
    if (status() == Loading) {
        return;
    }
    
    Q_D(AuthenticationRequest);
    d->authRequest = AuthenticationRequestPrivate::RevokeToken;
    setUrl(REVOKE_TOKEN_URL);
    setData(QVariant());
    get();
}

}

#include "moc_authenticationrequest.cpp"
