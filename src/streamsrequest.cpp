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

class FormatHash : public QHash<QString, Format>
{

public:
    FormatHash() : 
        QHash<QString, Format>() 
    {
        insert("stream_h264_ld_url", Format("stream_h264_ld_url", "H264 audio/video", "mp4", 400, 240));
        insert("stream_h264_url", Format("stream_h264_url", "H264 audio/video", "mp4", 512, 384));
        insert("stream_h264_hq_url", Format("stream_h264_hq_url", "H264 audio/video", "mp4", 848, 480));
        insert("stream_h264_hd_url", Format("stream_h264_hd_url", "H264 audio/video", "mp4", 1280, 720));
        insert("stream_h264_hd1080_url", Format("stream_h264_hd1080_url", "H264 audio/video", "mp4", 1920, 1080));
        insert("stream_h264_qhd_url", Format("stream_h264_qhd_url", "H264 audio/video", "mp4", 2560, 1440));
        insert("stream_h264_uhd_url", Format("stream_h264_uhd_url", "H264 audio/video", "mp4", 3840, 2160));
        insert("stream_source_url", Format("stream_source_url", "H264 audio/video", "mp4", 0, 0));
    }
};

static const QStringList FORMAT_LIST = QStringList() << "stream_source_url" << "stream_h264_uhd_url"
                                                     << "stream_h264_qhd_url" << "stream_h264_hd1080_url"
                                                     << "stream_h264_hd_url" << "stream_h264_hq_url"
                                                     << "stream_h264_url" << "stream_h264_ld_url";

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
        
        switch (reply->error()) {
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
            setError(Request::Error(reply->error()));
            setErrorString(reply->errorString());
            emit q->finished();
            return;
        }
        
        bool ok;
        QVariantMap info = QtJson::Json::parse(QString(reply->readAll()).section("var info = ", 1, 1)
                                                                        .section(";\n", 0, 0), ok).toMap();
        
        if (ok) {
            if (info.contains("error")) {
                setStatus(Request::Failed);
                setError(Request::UnknownContentError);
                setErrorString(info.value("error").toMap().value("message").toString());
            }
            else {
                QVariantList list;

                foreach (QString f, FORMAT_LIST) {
                    if (info.contains(f)) {
                        QVariant v = info.value(f);

                        if (!v.isNull()) {
                            Format format = formatHash.value(f);
                            format["url"] = v;
                            list << format;
                        }
                    }
                }

                setResult(list);
                setStatus(Request::Ready);
                setError(Request::NoError);
                setErrorString(QString());
            }
        }
        else {
            setStatus(Request::Failed);
            setError(Request::ParseError);
            setErrorString(Request::tr("Unable to parse response"));
        }

        emit q->finished();
    }
    
    static FormatHash formatHash;
                
    Q_DECLARE_PUBLIC(StreamsRequest)
};

FormatHash StreamsRequestPrivate::formatHash;

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
    Q_D(StreamsRequest);
    setUrl(VIDEO_PAGE_URL + "/" + id);
    d->networkAccessManager()->cookieJar()->setCookiesFromUrl(QList<QNetworkCookie>() << QNetworkCookie("ff", "off"),
                                                              url());
    get(false);
}

}

#include "moc_streamsrequest.cpp"
