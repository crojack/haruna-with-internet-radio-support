/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "radiostationsmodel.h"
#include "pathutils.h"
#include "../playlist/playlistfilterproxymodel.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QUrlQuery>

RadioStationsModel::RadioStationsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    loadFavorites();
}

int RadioStationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_stations.count();
}

QVariant RadioStationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_stations.count()) {
        return QVariant();
    }

    const RadioStation &station = m_stations.at(index.row());

    switch (role) {
    case StationUuidRole:
        return station.stationuuid;
    case NameRole:
        return station.name;
    case UrlRole:
        return station.url.toString();
    case FaviconRole:
        return station.favicon.toString();
    case TagsRole:
        return station.tags;
    case CountryRole:
        return station.country;
    case CountryCodeRole:
        return station.countryCode;
    case BitrateRole:
        return station.bitrate;
    case CodecRole:
        return station.codec;
    case IsFavoriteRole:
        return station.isFavorite;
    case VotesRole:
        return station.votes;
    }

    return QVariant();
}

QHash<int, QByteArray> RadioStationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[StationUuidRole] = "stationuuid";
    roles[NameRole] = "name";
    roles[UrlRole] = "url";
    roles[FaviconRole] = "favicon";
    roles[TagsRole] = "tags";
    roles[CountryRole] = "country";
    roles[CountryCodeRole] = "countryCode";
    roles[BitrateRole] = "bitrate";
    roles[CodecRole] = "codec";
    roles[IsFavoriteRole] = "isFavorite";
    roles[VotesRole] = "votes";
    return roles;
}

int RadioStationsModel::favoriteCount() const
{
    return m_favoriteStations.count();
}

void RadioStationsModel::searchStations(const QString &query)
{
    if (query.isEmpty()) {
        showFavorites();
        return;
    }

    // Check if user wants favorites
    QString lowerQuery = query.toLower().trimmed();
    if (lowerQuery == QStringLiteral("fav") || lowerQuery == QStringLiteral("favorite") || 
        lowerQuery == QStringLiteral("favorites") || lowerQuery == QStringLiteral("favourite") || 
        lowerQuery == QStringLiteral("favourites")) {
        showFavorites();
        return;
    }

    // Check if it's a genre search
    if (lowerQuery.startsWith(QStringLiteral("genre:"))) {
        QString genre = lowerQuery.mid(6).trimmed();
        searchByTag(genre);
        return;
    }

    // Check if it's a country code (2 letters)
    if (query.length() == 2 && query.toUpper() == query) {
        searchByCountry(query.toUpper());
        return;
    }

    // Default: search by name
    searchByName(query);
}

void RadioStationsModel::searchByName(const QString &query)
{
    m_isSearching = true;
    Q_EMIT isSearchingChanged();

    QString endpoint = m_apiEndpoints[m_currentEndpointIndex];
    QUrl url(endpoint + QStringLiteral("/json/stations/byname/") + QString::fromUtf8(QUrl::toPercentEncoding(query)));
    
    qDebug() << "Searching stations by name:" << query << "at" << url.toString();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Haruna/1.0"));
    
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSearchReply(reply);
    });
}

void RadioStationsModel::searchByCountry(const QString &countryCode)
{
    m_isSearching = true;
    Q_EMIT isSearchingChanged();

    QString endpoint = m_apiEndpoints[m_currentEndpointIndex];
    QUrl url(endpoint + QStringLiteral("/json/stations/bycountrycodeexact/") + countryCode);
    
    qDebug() << "Searching stations by country:" << countryCode << "at" << url.toString();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Haruna/1.0"));
    
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSearchReply(reply);
    });
}

void RadioStationsModel::searchByTag(const QString &tag)
{
    m_isSearching = true;
    Q_EMIT isSearchingChanged();

    QString endpoint = m_apiEndpoints[m_currentEndpointIndex];
    QUrl url(endpoint + QStringLiteral("/json/stations/bytagexact/") + QString::fromUtf8(QUrl::toPercentEncoding(tag)));
    
    qDebug() << "Searching stations by tag:" << tag << "at" << url.toString();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Haruna/1.0"));
    
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSearchReply(reply);
    });
}

void RadioStationsModel::handleSearchReply(QNetworkReply *reply)
{
    m_isSearching = false;
    Q_EMIT isSearchingChanged();

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        m_lastError = reply->errorString();
        Q_EMIT lastErrorChanged();
        qWarning() << "Radio station search error:" << m_lastError;
        
        // Try next endpoint if available
        if (m_currentEndpointIndex < m_apiEndpoints.count() - 1) {
            m_currentEndpointIndex++;
            qDebug() << "Trying next endpoint:" << m_apiEndpoints[m_currentEndpointIndex];
        }
        return;
    }

    QByteArray data = reply->readAll();
    processStations(data);
}

void RadioStationsModel::processStations(const QByteArray &data)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        m_lastError = QStringLiteral("JSON parse error: ") + parseError.errorString();
        Q_EMIT lastErrorChanged();
        qWarning() << m_lastError;
        return;
    }

    if (!doc.isArray()) {
        m_lastError = QStringLiteral("Expected JSON array");
        Q_EMIT lastErrorChanged();
        qWarning() << m_lastError;
        return;
    }

    beginResetModel();
    m_stations.clear();

    QJsonArray array = doc.array();
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            RadioStation station(value.toObject());
            if (station.isValid()) {
                // Check if this station is in favorites
                station.isFavorite = isFavoriteStation(station.stationuuid);
                m_stations.append(station);
            }
        }
    }

    endResetModel();

    qDebug() << "Loaded" << m_stations.count() << "radio stations";
    Q_EMIT searchCompleted(m_stations.count());
}

void RadioStationsModel::showFavorites()
{
    beginResetModel();
    m_stations = m_favoriteStations;
    endResetModel();

    qDebug() << "Showing" << m_favoriteStations.count() << "favorite stations";
    Q_EMIT searchCompleted(m_favoriteStations.count());
}

void RadioStationsModel::toggleFavorite(int index)
{
    if (index < 0 || index >= m_stations.count()) {
        return;
    }

    RadioStation &station = m_stations[index];
    station.isFavorite = !station.isFavorite;

    if (station.isFavorite) {
        // Add to favorites if not already there
        bool alreadyFavorite = false;
        for (const RadioStation &fav : m_favoriteStations) {
            if (fav.stationuuid == station.stationuuid) {
                alreadyFavorite = true;
                break;
            }
        }
        if (!alreadyFavorite) {
            m_favoriteStations.append(station);
        }
    } else {
        // Remove from favorites
        for (int i = 0; i < m_favoriteStations.count(); ++i) {
            if (m_favoriteStations[i].stationuuid == station.stationuuid) {
                m_favoriteStations.removeAt(i);
                break;
            }
        }
    }

    saveFavorites();
    
    QModelIndex modelIndex = createIndex(index, 0);
    Q_EMIT dataChanged(modelIndex, modelIndex, {IsFavoriteRole});
    Q_EMIT favoriteCountChanged();
}

void RadioStationsModel::playStation(int index)
{
    if (index < 0 || index >= m_stations.count()) {
        return;
    }

    const RadioStation &station = m_stations[index];
    qDebug() << "Playing radio station:" << station.name << station.url.toString();
    Q_EMIT playStationRequested(station.url.toString(), station.name);
}

void RadioStationsModel::clearResults()
{
    beginResetModel();
    m_stations.clear();
    endResetModel();
}

QString RadioStationsModel::getFavoritesFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }
    return dataPath + QStringLiteral("/favorite_radio_stations.json");
}

void RadioStationsModel::loadFavorites()
{
    QString filePath = getFavoritesFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        qDebug() << "No favorites file found at" << filePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open favorites file:" << file.errorString();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing favorites JSON:" << parseError.errorString();
        return;
    }

    if (!doc.isArray()) {
        qWarning() << "Favorites file is not a JSON array";
        return;
    }

    m_favoriteStations.clear();
    QJsonArray array = doc.array();
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            RadioStation station(value.toObject());
            if (station.isValid()) {
                station.isFavorite = true;
                m_favoriteStations.append(station);
            }
        }
    }

    qDebug() << "Loaded" << m_favoriteStations.count() << "favorite stations";
    Q_EMIT favoriteCountChanged();
}

void RadioStationsModel::saveFavorites()
{
    QString filePath = getFavoritesFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open favorites file for writing:" << file.errorString();
        return;
    }

    QJsonArray array;
    for (const RadioStation &station : m_favoriteStations) {
        array.append(station.toJson());
    }

    QJsonDocument doc(array);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Saved" << m_favoriteStations.count() << "favorite stations";
}

bool RadioStationsModel::isFavoriteStation(const QString &uuid) const
{
    for (const RadioStation &station : m_favoriteStations) {
        if (station.stationuuid == uuid) {
            return true;
        }
    }
    return false;
}
