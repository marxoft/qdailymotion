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

#include "streamsrequest.h"
#include "request_p.h"
#include "urls.h"
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QStringList>

namespace QDailymotion {

class Format : public QVariantMap
{

public:
    Format() :
        QVariantMap()
    {
    }
    
    Format(const QString &id, const QString &desc, const QString &ext, int width, int height) :
        QVariantMap()
    {
        insert("id", id);
        insert("description", desc);
        insert("ext", ext);
        insert("width", width);
        insert("height", height);
    }
};

class FormatMap : public QMap<QString, Format>
{

public:
    FormatMap() : 
        QMap<QString, Format>() 
    {
        insert("144", Format("144", "H264 audio/video", "mp4", 176, 144));
        insert("240", Format("240", "H264 audio/video", "mp4", 400, 240));
        insert("380", Format("380", "H264 audio/video", "mp4", 512, 384));
        insert("480", Format("480", "H264 audio/video", "mp4", 848, 480));
        insert("720", Format("720", "H264 audio/video", "mp4", 1280, 720));
        insert("1080", Format("1080", "H264 audio/video", "mp4", 1920, 1080));
        insert("1440", Format("1440", "H264 audio/video", "mp4", 2560, 1440));
        insert("2160", Format("2160", "H264 audio/video", "mp4", 3840, 2160));
    }
};

class StreamsRequestPrivate : public RequestPrivate
{

public:
    StreamsRequestPrivate(StreamsRequest *parent) :
        RequestPrivate(parent)
    {
    }
    
    void _q_onReplyFinished() {
        if (!reply) {
            return;
        }
    
        Q_Q(StreamsRequest);
        
        const QString response = reply->readAll();
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
        default:
            setStatus(Request::Failed);
            setError(Request::Error(e));
            setErrorString(es);
            emit q->finished();
            return;
        }
        
        bool ok;
        const QVariantMap info = QtJson::Json::parse(response
                                 .section("dmp.create(document.getElementById('player'), ", 1, 1)
                                 .section(");\n", 0, 0), ok).toMap();
  
        if (ok) {
            const QVariantMap metadata = info.value("metadata").toMap();
            
            if (metadata.contains("qualities")) {
                const QVariantMap qualities = metadata.value("qualities").toMap();
                QVariantList list;
                QMapIterator<QString, Format> iterator(formatMap);
                
                while (iterator.hasNext()) {
                    iterator.next();
                    
                    if (qualities.contains(iterator.key())) {
                        const QVariantList ql = qualities.value(iterator.key()).toList();

                        if (!ql.isEmpty()) {
                            const QVariant u = ql.first().toMap().value("url");
                            
                            if (!u.isNull()) {
                                Format format = iterator.value();
                                format["url"] = u;
                                list << format;
                            }
                        }
                    }
                }
                
                setResult(list);
                setStatus(Request::Ready);
                setError(Request::NoError);
                setErrorString(QString());
                emit q->finished();
                return;
            }
            else if (metadata.contains("error")) {
                setStatus(Request::Failed);
                setError(Request::UnknownContentError);
                setErrorString(metadata.value("error").toMap().value("message").toString());
                emit q->finished();
                return;
            }
        }
        
        setStatus(Request::Failed);
        setError(Request::UnknownContentError);
        setErrorString(StreamsRequest::tr("No streams found"));
        emit q->finished();
    }
    
    static FormatMap formatMap;
                
    Q_DECLARE_PUBLIC(StreamsRequest)
};

FormatMap StreamsRequestPrivate::formatMap;

/*!
    \class StreamsRequest
    \brief Handles requests for video streams
    
    \ingroup requests
    
    The StreamsRequest class is used for requesting a list of streams for a Dailymotion video.
    
    Example usage:
    
    C++
    
    \code
    using namespace QDailymotion;
    
    ...
    
    StreamsRequest request;
    request.list(VIDEO_ID);
    connect(&request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    
    ...
    
    void MyClass::onRequestFinished() {
        if (request.status() == StreamsRequest::Ready) {
            foreach (QVariant stream, request.result().toList()) {
                qDebug() << "ID:" << stream.value("id").toString();
                qDebug() << "Description:" << stream.value("description").toString();
                qDebug() << "Extension:" << stream.value("ext").toString();
                qDebug() << "Width:" << stream.value("width").toInt();
                qDebug() << "Height:" << stream.value("height").toInt();
                qDebug() << "URL:" << stream.value("url").toString();
            }
        }
        else {
            qDebug() << request.errorString();
        }
    }
    \endcode
    
    QML
    
    \code
    import QtQuick 1.0
    import QDailymotion 1.0
    
    StreamsRequest {
        id: request
        
        onFinished: {
            if (status == StreamsRequest.Ready) {
                for (var i = 0; i < result.length; i++) {
                    console.log("ID: " + result[i].id);
                    console.log("Description: " + result[i].description);
                    console.log("Extension: " + result[i].ext);
                    console.log("Width: " + result[i].width);
                    console.log("Height: " + result[i].height);
                    console.log("URL: " + result[i].url);
                }
            }
            else {
                console.log(errorString);
            }
        }
        
        Component.onCompleted: list(VIDEO_ID)
    }
    \endcode
*/
StreamsRequest::StreamsRequest(QObject *parent) :
    Request(*new StreamsRequestPrivate(this), parent)
{
}

/*!
    \brief Requests a list of streams for the video identified by id.
*/
void StreamsRequest::list(const QString &id) {
    if (status() == Loading) {
        return;
    }
    
    Q_D(StreamsRequest);
    setUrl(VIDEO_PAGE_URL + "/" + id);
    d->networkAccessManager()->cookieJar()->setCookiesFromUrl(QList<QNetworkCookie>() << QNetworkCookie("ff", "off"),
                                                              url());
    get(false);
}

}

#include "moc_streamsrequest.cpp"
