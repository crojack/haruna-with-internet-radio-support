/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "database.h"

#include <QFile>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>

#include "pathutils.h"
#include "recentfile.h"

using namespace Qt::StringLiterals;

const QString RECENT_FILES_TABLE = QStringLiteral("recent_files");
const QString PLAYBACK_POSITION_TABLE = QStringLiteral("playback_position");

QString getLastExecutedQuery(const QSqlQuery &query)
{
    QString str = query.lastQuery();

    const QVariantList values = query.boundValues();
    const QStringList keys = query.boundValueNames();

    for (qsizetype i = 0; i < values.size(); ++i) {
        str.replace(keys.at(i), values.at(i).toString());
    }

    return str;
}

Database *Database::instance()
{
    static Database *db = new Database();
    return db;
}

Database *Database::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

Database::Database(QObject *parent)
    : QObject(parent)
{
    const auto dbFile{PathUtils::instance()->configFilePath(PathUtils::ConfigFile::Database)};

    auto mangaDB = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("haruna"));
    mangaDB.setDatabaseName(dbFile);
    if (mangaDB.open()) {
        createTables();
    } else {
        qDebug() << "Could not open database:" << mangaDB.lastError();
    }
}

void Database::createTables()
{
    QStringList tables = db().tables();

    if (!tables.contains(QStringLiteral("recent_files"))) {
        QFile sqlFile(QStringLiteral(":sql/create-recent_files-table.sql"));
        if (sqlFile.open(QFile::ReadOnly)) {
            QSqlQuery qManga(db());
            qManga.exec(QString::fromUtf8(sqlFile.readAll()));
        }
    }
    if (!tables.contains(QStringLiteral("playback_position"))) {
        QFile sqlFile(QStringLiteral(":sql/create-playback_position-table.sql"));
        if (sqlFile.open(QFile::ReadOnly)) {
            QSqlQuery qManga(db());
            qManga.exec(QString::fromUtf8(sqlFile.readAll()));
        }
    }
}

QSqlDatabase Database::db()
{
    return QSqlDatabase::database(QStringLiteral("haruna"));
}

QList<RecentFile> Database::recentFiles(uint limit)
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT * FROM ") % RECENT_FILES_TABLE % QStringLiteral(" ORDER BY timestamp DESC LIMIT ") % QString::number(limit));
    query.exec();

    QList<RecentFile> recentFiles;
    while (query.next()) {
        RecentFile rf;
        rf.id = query.value(QStringLiteral("recent_file_id")).toUInt();
        rf.url = query.value(QStringLiteral("url")).toUrl();
        rf.filename = query.value(QStringLiteral("filename")).toString();
        rf.openedFrom = query.value(QStringLiteral("opened_from")).toString();
        rf.timestamp = query.value(QStringLiteral("timestamp")).toLongLong();

        recentFiles.append(rf);
    }

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }

    return recentFiles;
}

void Database::addRecentFile(const QUrl &url, const QString &filename, const QString &openedFrom, qint64 timestamp)
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("INSERT INTO ") % RECENT_FILES_TABLE %
                  u" (url, filename, opened_from, timestamp) "
                  "VALUES (:url, :filename, :openedFrom, :timestamp) "
                  "ON CONFLICT(url) DO UPDATE SET "
                  "opened_from = :openedFrom, timestamp = :timestamp"_s);
    query.bindValue(QStringLiteral(":url"), url);
    query.bindValue(QStringLiteral(":filename"), filename);
    query.bindValue(QStringLiteral(":openedFrom"), openedFrom);
    query.bindValue(QStringLiteral(":timestamp"), timestamp);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

void Database::deleteRecentFiles()
{
    QSqlQuery query(db());
    query.exec(QStringLiteral("DELETE FROM ") % RECENT_FILES_TABLE);
}

double Database::playbackPosition(const QString &md5Hash)
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT * FROM ") % PLAYBACK_POSITION_TABLE % QStringLiteral(" WHERE md5_hash = :md5Hash LIMIT 1"));
    query.bindValue(QStringLiteral(":md5Hash"), md5Hash);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }

    while (query.first()) {
        return query.value(QStringLiteral("position")).toDouble();
    }

    return 0.0;
}

void Database::addPlaybackPosition(const QString &md5Hash, const QString &path, double position, QSqlDatabase dbConnection)
{
    QSqlDatabase database = dbConnection.isValid() ? dbConnection : db();
    QSqlQuery query(database);
    query.prepare(QStringLiteral("INSERT INTO ") % PLAYBACK_POSITION_TABLE %
                  u" (md5_hash, path, position) "
                  "VALUES (:md5Hash, :path, :position) "
                  "ON CONFLICT(md5_hash) DO UPDATE SET "
                  "position = excluded.position"_s);
    query.bindValue(QStringLiteral(":md5Hash"), md5Hash);
    query.bindValue(QStringLiteral(":path"), path);
    query.bindValue(QStringLiteral(":position"), position);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

void Database::deletePlaybackPositions()
{
    QSqlQuery query(db());
    query.exec(QStringLiteral("DELETE FROM ") % PLAYBACK_POSITION_TABLE);
}

void Database::deletePlaybackPosition(const QString &md5Hash)
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM ") % PLAYBACK_POSITION_TABLE %
                  QStringLiteral(" WHERE md5_hash = :md5Hash"));
    query.bindValue(QStringLiteral(":md5Hash"), md5Hash);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

#include "moc_database.cpp"
