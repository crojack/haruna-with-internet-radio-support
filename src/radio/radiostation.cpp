/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "radiostation.h"
#include <QJsonObject>

RadioStation::RadioStation(const QJsonObject &json)
{
    stationuuid = json.value(QStringLiteral("stationuuid")).toString();
    name        = json.value(QStringLiteral("name")).toString();

    // Use url_resolved from API responses for the actual stream URL
    if (json.contains(QStringLiteral("url_resolved")))
        url = QUrl(json.value(QStringLiteral("url_resolved")).toString());
    else
        url = QUrl(json.value(QStringLiteral("url")).toString()); // fallback for saved favorites

    favicon     = QUrl(json.value(QStringLiteral("favicon")).toString());
    tags        = json.value(QStringLiteral("tags")).toString();
    country     = json.value(QStringLiteral("country")).toString();
    countryCode = json.value(QStringLiteral("countrycode")).toString();
    bitrate     = json.value(QStringLiteral("bitrate")).toInt();
    codec       = json.value(QStringLiteral("codec")).toString();
    homepage    = json.value(QStringLiteral("homepage")).toString();
    votes       = json.value(QStringLiteral("votes")).toInt();

    // API JSON won’t have isFavorite, but saved favorites will
    isFavorite  = json.value(QStringLiteral("isFavorite")).toBool(false);
}

QJsonObject RadioStation::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("stationuuid")] = stationuuid;
    obj[QStringLiteral("name")]        = name;
    obj[QStringLiteral("url")]         = url.toString();        // our own schema for favorites
    obj[QStringLiteral("favicon")]     = favicon.toString();
    obj[QStringLiteral("tags")]        = tags;
    obj[QStringLiteral("country")]     = country;
    obj[QStringLiteral("countrycode")] = countryCode;
    obj[QStringLiteral("bitrate")]     = bitrate;
    obj[QStringLiteral("codec")]       = codec;
    obj[QStringLiteral("homepage")]    = homepage;
    obj[QStringLiteral("votes")]       = votes;
    obj[QStringLiteral("isFavorite")]  = isFavorite;
    return obj;
}

RadioStation RadioStation::fromJson(const QJsonObject &json)
{
    RadioStation station(json);
    if (json.contains(QStringLiteral("isFavorite")))
        station.isFavorite = json.value(QStringLiteral("isFavorite")).toBool();
    return station;
}

