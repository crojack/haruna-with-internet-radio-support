/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "pathutils.h"

#include <QDir>
#include <QFileInfo>
#include <QQmlEngine>

#include <KIO/OpenFileManagerWindowJob>

using namespace Qt::StringLiterals;

PathUtils *PathUtils::instance()
{
    static PathUtils g;
    return &g;
}

PathUtils *PathUtils::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

PathUtils::PathUtils()
    : m_config(KSharedConfig::openConfig(QStringLiteral("haruna/haruna.conf"), KConfig::SimpleConfig, QStandardPaths::GenericConfigLocation))
    , m_ccConfig(KSharedConfig::openConfig(QStringLiteral("haruna/custom-commands.conf"), KConfig::SimpleConfig, QStandardPaths::GenericConfigLocation))
    , m_shortcutsConfig(KSharedConfig::openConfig(QStringLiteral("haruna/shortcuts.conf"), KConfig::SimpleConfig, QStandardPaths::GenericConfigLocation))
{
}

const QString PathUtils::configFileParentPath(ConfigFile configFile)
{
    switch (configFile) {
    case ConfigFile::Main: {
        auto path = QStandardPaths::writableLocation(m_config->locationType()).append(QStringLiteral("/haruna"));
        if (QFileInfo::exists(path)) {
            return path;
        }
        break;
    }
    case ConfigFile::CustomCommands: {
        auto path = QStandardPaths::writableLocation(m_ccConfig->locationType()).append(QStringLiteral("/haruna"));
        if (QFileInfo::exists(path)) {
            return path;
        }
        break;
    }
    case ConfigFile::Shortcuts: {
        auto path = QStandardPaths::writableLocation(m_shortcutsConfig->locationType()).append(QStringLiteral("/haruna"));
        if (QFileInfo::exists(path)) {
            return path;
        }
        break;
    }
    case ConfigFile::Database:
    case ConfigFile::PlaylistCache: {
        auto path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append(QStringLiteral("/haruna"));

        if (QFileInfo::exists(path)) {
            return path;
        }
        break;
    }
    }

    return {};
}

const QString PathUtils::configFilePath(ConfigFile configFile)
{
    switch (configFile) {
    case ConfigFile::Main: {
        auto path = QStandardPaths::writableLocation(m_config->locationType()).append(QStringLiteral("/"));
        return path.append(m_config->name());
    }
    case ConfigFile::CustomCommands: {
        auto path = QStandardPaths::writableLocation(m_ccConfig->locationType()).append(QStringLiteral("/"));
        return path.append(m_ccConfig->name());
    }
    case ConfigFile::Shortcuts: {
        auto path = QStandardPaths::writableLocation(m_shortcutsConfig->locationType()).append(QStringLiteral("/"));
        return path.append(m_shortcutsConfig->name());
    }
    case ConfigFile::Database: {
        auto path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append(QStringLiteral("/haruna/"));

        QDir dir(path);
        if (dir.exists()) {
            return path.append(QStringLiteral("haruna.db"));
        }
        if (dir.mkpath(path)) {
            return path.append(QStringLiteral("haruna.db"));
        }

        return {};
    }
    case ConfigFile::PlaylistCache: {
        auto path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append(QStringLiteral("/haruna/"));

        QDir dir(path);
        if (dir.exists()) {
            return path.append(QStringLiteral("playlist.json"));
        }
        if (dir.mkpath(path)) {
            return path.append(QStringLiteral("playlist.json"));
        }

        return {};
    }
    }

    return {};
}

const QString PathUtils::playlistsFolder()
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path.append(QStringLiteral("/haruna/playlists"));

    QDir dir(path);
    if (dir.exists()) {
        return path;
    }
    if (dir.mkpath(path)) {
        return path;
    }

    return {};
}

void PathUtils::highlightInFileManager(const QString &path)
{
    KIO::highlightInFileManager({QUrl::fromUserInput(path)});
}

bool PathUtils::pathExists(const QString &path)
{
    return QFileInfo::exists(path);
}

QUrl PathUtils::pathToUrl(const QString &path)
{
    return QUrl::fromUserInput(path);
}

QString PathUtils::urlToPath(const QUrl &url)
{
    return url.toLocalFile();
}

QString PathUtils::parentPath(const QString &path)
{
    QString _path = path;
    if (_path.endsWith(QStringLiteral("/"))) {
        _path.removeLast();
    }

    QFileInfo fileInfo(_path);

    return fileInfo.absolutePath();
}

QUrl PathUtils::parentUrl(const QUrl &url)
{
    if (!url.isLocalFile()) {
        return url;
    }
    const auto _filePath = urlToPath(url);
    const auto _parentPath = parentPath(_filePath);
    const auto _url = pathToUrl(_parentPath);

    return _url;
}

#include "moc_pathutils.cpp"
