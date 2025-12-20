/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef RADIOSTATION_H
#define RADIOSTATION_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QJsonObject>

class RadioStation
{
public:
    RadioStation() = default;
    RadioStation(const QJsonObject &json);
    
    QJsonObject toJson() const;
    static RadioStation fromJson(const QJsonObject &json);

    QString stationuuid;
    QString name;
    QUrl url;
    QUrl favicon;
    QString tags;
    QString country;
    QString countryCode;
    int bitrate{0};
    bool isFavorite{false};
    
    QString codec;
    QString homepage;
    int votes{0};
    
    bool isValid() const {
    return !name.isEmpty() && url.isValid();
    }
};

Q_DECLARE_METATYPE(RadioStation)

#endif // RADIOSTATION_H
