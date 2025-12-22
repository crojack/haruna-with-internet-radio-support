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
#include <QRandomGenerator>

RadioStationsModel::RadioStationsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // Use ONLY the verified working server
    m_apiEndpoints.clear();
    m_apiEndpoints << QStringLiteral("https://de1.api.radio-browser.info");
    
    m_serversDiscovered = true; // Mark as ready
    
    qDebug() << "Using radio-browser.info API endpoint:" << m_apiEndpoints.first();
    
    loadFavorites();
}

void RadioStationsModel::discoverServers()
{
    // Not needed - using static list of known working servers
}

void RadioStationsModel::handleServerDiscovery(QHostInfo hostInfo)
{
    Q_UNUSED(hostInfo);
    // Not needed - using static list of known working servers
}

QString RadioStationsModel::getNextEndpoint()
{
    // If servers haven't been discovered yet, use fallback
    if (!m_serversDiscovered || m_apiEndpoints.isEmpty()) {
        return QStringLiteral("https://de1.api.radio-browser.info");
    }
    
    if (m_currentEndpointIndex >= m_apiEndpoints.count()) {
        m_currentEndpointIndex = 0;
    }
    
    return m_apiEndpoints[m_currentEndpointIndex];
}

void RadioStationsModel::abortCurrentRequest()
{
    if (m_currentReply && m_currentReply->isRunning()) {
        qDebug() << "Aborting current request";
        m_currentReply->abort();
        m_currentReply = nullptr;
    }
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

    // Abort any in-progress request
    abortCurrentRequest();

    // Reset retry counter and endpoint index for new search
    m_retryCount = 0;
    m_currentEndpointIndex = 0;

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
        m_currentSearchType = SearchByTag;
        m_currentSearchQuery = genre;
        searchByTag(genre);
        return;
    }

    // Check if it's a country code (2 letters)
    if (query.length() == 2 && query.toUpper() == query) {
        m_currentSearchType = SearchByCountry;
        m_currentSearchQuery = query.toUpper();
        searchByCountry(query.toUpper());
        return;
    }

    // Default: search by name
    m_currentSearchType = SearchByName;
    m_currentSearchQuery = query;
    searchByName(query);
}

void RadioStationsModel::searchByName(const QString &query)
{
    m_isSearching = true;
    Q_EMIT isSearchingChanged();

    QString endpoint = getNextEndpoint();
    QUrl url(endpoint + QStringLiteral("/json/stations/byname/") + QString::fromUtf8(QUrl::toPercentEncoding(query)));
    
    qDebug() << "Searching stations by name:" << query << "at" << url.toString();
    qDebug() << "Endpoint:" << (m_currentEndpointIndex + 1) << "of" << m_apiEndpoints.count() 
             << "| Retry:" << m_retryCount << "of" << MAX_RETRIES;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Haruna/1.0"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(REQUEST_TIMEOUT_MS);
    
    m_currentReply = m_networkManager->get(request);
    
    connect(m_currentReply, &QNetworkReply::finished, this, [this]() {
        handleSearchReply(m_currentReply);
    });
}

void RadioStationsModel::searchByCountry(const QString &countryCode)
{
    m_isSearching = true;
    Q_EMIT isSearchingChanged();

    QString endpoint = getNextEndpoint();
    QUrl url(endpoint + QStringLiteral("/json/stations/bycountrycodeexact/") + countryCode);
    
    qDebug() << "Searching stations by country:" << countryCode << "at" << url.toString();
    qDebug() << "Endpoint:" << (m_currentEndpointIndex + 1) << "of" << m_apiEndpoints.count() 
             << "| Retry:" << m_retryCount << "of" << MAX_RETRIES;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Haruna/1.0"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(REQUEST_TIMEOUT_MS);
    
    m_currentReply = m_networkManager->get(request);
    
    connect(m_currentReply, &QNetworkReply::finished, this, [this]() {
        handleSearchReply(m_currentReply);
    });
}

void RadioStationsModel::searchByTag(const QString &tag)
{
    m_isSearching = true;
    Q_EMIT isSearchingChanged();

    QString endpoint = getNextEndpoint();
    QUrl url(endpoint + QStringLiteral("/json/stations/bytagexact/") + QString::fromUtf8(QUrl::toPercentEncoding(tag)));
    
    qDebug() << "Searching stations by tag:" << tag << "at" << url.toString();
    qDebug() << "Endpoint:" << (m_currentEndpointIndex + 1) << "of" << m_apiEndpoints.count() 
             << "| Retry:" << m_retryCount << "of" << MAX_RETRIES;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Haruna/1.0"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(REQUEST_TIMEOUT_MS);
    
    m_currentReply = m_networkManager->get(request);
    
    connect(m_currentReply, &QNetworkReply::finished, this, [this]() {
        handleSearchReply(m_currentReply);
    });
}

void RadioStationsModel::retrySearch()
{
    qDebug() << "Retrying search with query:" << m_currentSearchQuery 
             << "| Type:" << m_currentSearchType;
    
    switch (m_currentSearchType) {
    case SearchByName:
        searchByName(m_currentSearchQuery);
        break;
    case SearchByCountry:
        searchByCountry(m_currentSearchQuery);
        break;
    case SearchByTag:
        searchByTag(m_currentSearchQuery);
        break;
    }
}

void RadioStationsModel::handleSearchReply(QNetworkReply *reply)
{
    if (!reply) {
        qWarning() << "handleSearchReply called with null reply";
        return;
    }

    reply->deleteLater();
    
    // Clear the current reply pointer since we're done with it
    if (m_currentReply == reply) {
        m_currentReply = nullptr;
    }

    if (reply->error() != QNetworkReply::NoError) {
        m_lastError = reply->errorString();
        Q_EMIT lastErrorChanged();
        qWarning() << "Radio station search error:" << m_lastError;
        qWarning() << "Error code:" << reply->error();
        
        // Increment retry count
        m_retryCount++;
        
        // Try next endpoint if available and haven't exceeded max retries
        if (m_retryCount < MAX_RETRIES && m_currentEndpointIndex < m_apiEndpoints.count() - 1) {
            m_currentEndpointIndex++;
            qDebug() << "Trying next endpoint...";
            
            // Actually retry the search with the new endpoint
            retrySearch();
            return; // Don't set m_isSearching to false yet
        } else {
            // All endpoints failed or max retries reached
            qWarning() << "All endpoints failed or max retries reached";
            qWarning() << "Tried" << m_retryCount << "times across" << (m_currentEndpointIndex + 1) << "endpoints";
            
            m_lastError = QStringLiteral("Radio Browser API is temporarily unavailable. Please try again later.");
            Q_EMIT lastErrorChanged();
            
            // Reset for next search
            m_currentEndpointIndex = 0;
            m_retryCount = 0;
            
            m_isSearching = false;
            Q_EMIT isSearchingChanged();
            Q_EMIT searchCompleted(0); // Signal with 0 results
            return;
        }
    }

    // Success! Reset counters for next search
    m_currentEndpointIndex = 0;
    m_retryCount = 0;
    
    m_isSearching = false;
    Q_EMIT isSearchingChanged();

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