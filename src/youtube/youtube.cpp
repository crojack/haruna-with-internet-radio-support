/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "youtube.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QProcess>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QUrlQuery>

#include <KLocalizedString>

using namespace Qt::StringLiterals;

YouTube::YouTube()
{
}

bool YouTube::hasYoutubeDl()
{
    return !youtubeDlExecutable().isEmpty();
}

QString YouTube::youtubeDlExecutable()
{
    auto ytDlp = QStandardPaths::findExecutable(QStringLiteral("yt-dlp"));
    if (!ytDlp.isEmpty()) {
        return ytDlp;
    }

    auto youtubeDl = QStandardPaths::findExecutable(QStringLiteral("youtube-dl"));
    if (!youtubeDl.isEmpty()) {
        return youtubeDl;
    }

    return QString{};
}

bool YouTube::isPlaylist(const QUrl &url)
{
    if (!isYoutubeUrl(url)) {
        return false;
    }

    QUrlQuery query{url.query()};
    return query.hasQueryItem(QStringLiteral("list"));
}

void YouTube::getPlaylist(const QUrl &url)
{
    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(QStringLiteral("list"))};
    QString videoId{query.queryItemValue(QStringLiteral("v"))};
    QString playlistPath = QStringLiteral("https://www.youtube.com/playlist?list=%1").arg(playlistId);

    // use youtube-dl to get the required playlist info as json
    auto ytdlProcess = std::make_shared<QProcess>();
    auto args = QStringList() << QStringLiteral("-J") << QStringLiteral("--flat-playlist") << playlistId;
    ytdlProcess->setProgram(youtubeDlExecutable());
    ytdlProcess->setArguments(args);
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        const auto output = ytdlProcess->readAllStandardOutput();
        const auto errorString = ytdlProcess->readAllStandardError();
        const QString json = QString::fromUtf8(output);
        const QJsonValue entries = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("entries")];
        // QString playlistTitle = QJsonDocument::fromJson(json.toUtf8())[u"title")].toString();
        const auto playlist = entries.toArray();

        if (output.contains("null\n")) {
            Q_EMIT error(i18nc("@info:tooltip; error when yt-dlp fails to get playlist",
                        "Could not retrieve playlist with id: %1\n\n%2",
                        playlistId,
                        QString::fromUtf8(errorString)));
        }

        if (playlist.isEmpty()) {
            return;
        }

        Q_EMIT playlistRetrieved(playlist, videoId, playlistId);
    });
}

QUrl YouTube::normalizeUrl(const QUrl &url)
{
    if (!isYoutubeUrl(url)) {
        return url;
    }

    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(QStringLiteral("list"))};
    QString videoId{query.queryItemValue(QStringLiteral("v"))};

    if (videoId.isEmpty() && url.host().contains(QStringLiteral("youtu.be"))) {
        videoId = url.fileName();
    }

    QUrl normalizedUrl;
    if (playlistId.isEmpty()) {
        normalizedUrl = QUrl{QStringLiteral("https://www.youtube.com/watch?v=%1").arg(videoId)};
    } else {
        normalizedUrl = QUrl{QStringLiteral("https://www.youtube.com/watch?v=%1&list=%2").arg(videoId, playlistId)};
    }

    return normalizedUrl;
}

bool YouTube::isYoutubeUrl(const QUrl &url)
{
    if (url.host().contains(QStringLiteral("youtube.com")) || url.host().contains(QStringLiteral("youtu.be"))) {
        return true;
    }
    return false;
}

void YouTube::getVideoInfo(const QUrl &url, QVariantMap data)
{
    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(QStringLiteral("list"))};
    QString videoId{query.queryItemValue(QStringLiteral("v"))};

    QUrl urlWithoutPlaylist = url;
    if (!playlistId.isEmpty()) {
        urlWithoutPlaylist = QUrl{QStringLiteral("https://www.youtube.com/watch?v=%1").arg(videoId)};
    }

    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << QStringLiteral("-j") << urlWithoutPlaylist.toString());
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QString title = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("title")].toString();
        auto duration = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("duration")].toDouble();
        YTVideoInfo info;
        info.url = url;
        info.mediaTitle = title;
        info.duration = duration;
        Q_EMIT videoInfoRetrieved(info, data);
    });
}
