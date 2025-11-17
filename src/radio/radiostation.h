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
    
    // Convert to/from JSON for storage
    QJsonObject toJson() const;
    static RadioStation fromJson(const QJsonObject &json);
    
    // Station properties
    QString stationuuid;
    QString name;
    QUrl url;
    QUrl favicon;
    QString tags;
    QString country;
    QString countryCode;
    int bitrate{0};
    bool isFavorite{false};
    
    // Additional metadata
    QString codec;
    QString homepage;
    int votes{0};
    
    bool isValid() const {
    return !name.isEmpty() && url.isValid();
    }
};

// For use in QVariant
Q_DECLARE_METATYPE(RadioStation)

#endif // RADIOSTATION_H
