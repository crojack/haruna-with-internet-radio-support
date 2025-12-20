/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef RADIOSTATIONSMODEL_H
#define RADIOSTATIONSMODEL_H

#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <qqml.h>
#include "radiostation.h"


class RadioStationsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool isSearching READ isSearching NOTIFY isSearchingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(int favoriteCount READ favoriteCount NOTIFY favoriteCountChanged)

public:
    enum Roles {
        StationUuidRole = Qt::UserRole + 1,
        NameRole,
        UrlRole,
        FaviconRole,
        TagsRole,
        CountryRole,
        CountryCodeRole,
        BitrateRole,
        CodecRole,
        IsFavoriteRole,
        VotesRole
    };

    explicit RadioStationsModel(QObject *parent = nullptr);
    ~RadioStationsModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool isSearching() const { return m_isSearching; }
    QString lastError() const { return m_lastError; }
    int favoriteCount() const;

    // Invokable methods for QML
    Q_INVOKABLE void searchStations(const QString &query);
    Q_INVOKABLE void showFavorites();
    Q_INVOKABLE void toggleFavorite(int index);
    Q_INVOKABLE void playStation(int index);
    Q_INVOKABLE void clearResults();
    Q_INVOKABLE void loadFavorites();
    Q_INVOKABLE void saveFavorites();

Q_SIGNALS:
    void isSearchingChanged();
    void lastErrorChanged();
    void favoriteCountChanged();
    void searchCompleted(int resultCount);
    void playStationRequested(const QString &url, const QString &name);

private:
    void searchByName(const QString &query);
    void searchByCountry(const QString &countryCode);
    void searchByTag(const QString &tag);
    void handleSearchReply(QNetworkReply *reply);
    void processStations(const QByteArray &data);
    QString getFavoritesFilePath() const;
    bool isFavoriteStation(const QString &uuid) const;
    
    QNetworkAccessManager *m_networkManager;
    QList<RadioStation> m_stations;
    QList<RadioStation> m_favoriteStations;
    bool m_isSearching{false};
    QString m_lastError;
    
    // API endpoints - will try multiple mirrors
    const QStringList m_apiEndpoints = {
        QStringLiteral("https://de1.api.radio-browser.info"),
        QStringLiteral("https://at1.api.radio-browser.info"),
        QStringLiteral("https://nl1.api.radio-browser.info")
    };
    int m_currentEndpointIndex{0};
};

#endif // RADIOSTATIONSMODEL_H
