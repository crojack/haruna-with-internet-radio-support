/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>
#include <QUrlQuery>

#include "kconfig_version.h"
#include <KAboutData>
#include <KColorSchemeManager>
#include <KCrash>
#include <KFileItem>
#include <KLocalizedString>
#include <KWindowSystem>

#include <MpvAbstractItem>

#include "audiosettings.h"
#include "generalsettings.h"
#include "pathutils.h"
#include "haruna-version.h"
#include "informationsettings.h"
#include "miscutils.h"
#include "mousesettings.h"
#include "playbacksettings.h"
#include "playlistsettings.h"
#include "subtitlessettings.h"
#include "videosettings.h"
#include "worker.h"

using namespace Qt::StringLiterals;

bool ApplicationEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Leave) {
        Q_EMIT applicationMouseLeave();
    }
    if (event->type() == QEvent::Enter) {
        Q_EMIT applicationMouseEnter();
    }
    return QObject::eventFilter(obj, event);
}

Application *Application::instance()
{
    static Application app;
    return &app;
}

Application *Application::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

bool Application::actionsEnabled()
{
    return m_actionsEnabled;
}

void Application::setActionsEnabled(bool enable)
{
    if (enable == actionsEnabled()) {
        return;
    }
    m_actionsEnabled = enable;
    Q_EMIT actionsEnabledChanged();
}

Application::Application()
    : QObject{nullptr}
    , m_app(qApp)
    , m_schemes(KColorSchemeManager::instance())
    , m_systemDefaultStyle(m_app->style()->objectName())
    , m_appEventFilter{std::make_unique<ApplicationEventFilter>()}
{
    // used to hide playlist when mouse leaves the application
    // while moving between monitors while in fullscreen
    m_app->installEventFilter(m_appEventFilter.get());
    QObject::connect(m_appEventFilter.get(), &ApplicationEventFilter::applicationMouseLeave, this, &Application::qmlApplicationMouseLeave);
    QObject::connect(m_appEventFilter.get(), &ApplicationEventFilter::applicationMouseEnter, this, &Application::qmlApplicationMouseEnter);

    if (GeneralSettings::guiStyle() != QStringLiteral("System")) {
        QApplication::setStyle(GeneralSettings::guiStyle());
    }

    setupWorkerThread();
    setupAboutData();
    setupCommandLineParser();
    setupQmlSettingsTypes();

    KCrash::initialize();

    connect(PathUtils::instance(), &PathUtils::error, MiscUtils::instance(), &MiscUtils::error);
}

Application::~Application()
{
}

void Application::setupWorkerThread()
{
    auto worker = Worker::instance();
    auto thread = new QThread();
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::finished, worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(
        Worker::instance(),
        &Worker::ytdlpVersionRetrived,
        this,
        [this](const QByteArray &version) {
            m_aboutData.addComponent(QStringLiteral("yt-dlp"),
                                     i18n("Feature-rich command-line audio/video downloader"),
                                     QString::fromUtf8(version),
                                     QStringLiteral("https://github.com/yt-dlp/yt-dlp"),
                                     QStringLiteral("https://unlicense.org"));
            KAboutData::setApplicationData(m_aboutData);
        },
        Qt::QueuedConnection);
    thread->start();
}

void Application::setupAboutData()
{
    m_aboutData.setComponentName(QStringLiteral("haruna"));
    m_aboutData.setDisplayName(i18nc("application title/display name", "Haruna"));
    m_aboutData.setVersion(Application::version().toUtf8());
    m_aboutData.setShortDescription(i18nc("@title", "Media player"));
    m_aboutData.setLicense(KAboutLicense::GPL_V3);
    m_aboutData.setCopyrightStatement(i18nc("copyright statement, use copyright symbol and en dash for the year range", "© 2019–2025"));
    m_aboutData.setHomepage(QStringLiteral("https://haruna.kde.org"));
    m_aboutData.setBugAddress(QStringLiteral("https://bugs.kde.org/enter_bug.cgi?product=Haruna").toUtf8());
    m_aboutData.setDesktopFileName(QStringLiteral("org.kde.haruna"));

    m_aboutData.addAuthor(i18nc("@info:credit", "George Florea Bănuș"),
                          i18nc("@info:credit", "Developer"),
                          QStringLiteral("georgefb899@gmail.com"),
                          QStringLiteral("https://georgefb.com"));

    QMetaObject::invokeMethod(Worker::instance(), &Worker::getYtdlpVersion, Qt::QueuedConnection);

    MpvAbstractItem mpvItem;
    m_aboutData.addComponent(QStringLiteral("mpv"),
                             i18n("Command line video player"),
                             mpvItem.getProperty(QStringLiteral("mpv-version")).toString().replace(QStringLiteral("mpv "), QString{}),
                             QStringLiteral("https://mpv.io"),
                             KAboutLicense::GPL);

    m_aboutData.addComponent(QStringLiteral("ffmpeg"),
                             i18n("Cross-platform solution to record, convert and stream audio and video"),
                             mpvItem.getProperty(QStringLiteral("ffmpeg-version")).toString(),
                             QStringLiteral("https://www.ffmpeg.org"),
                             KAboutLicense::GPL);

    KAboutData::setApplicationData(m_aboutData);
}

void Application::setupCommandLineParser()
{
    m_parser = std::make_unique<QCommandLineParser>();
    m_aboutData.setupCommandLine(m_parser.get());
    m_parser->addPositionalArgument(QStringLiteral("file"), i18nc("@info:shell", "File to open"));

    QCommandLineOption ytdlFormatSelectionOption(QStringList() << QStringLiteral("ytdl-format-selection") << QStringLiteral("ytdlfs"),
                                                 i18nc("@info:shell",
                                                       "Allows to temporarily override the yt-dlp format selection setting. "
                                                       "Will be overwritten if the setting is changed through the GUI"),
                                                 QStringLiteral("bestvideo+bestaudio/best"),
                                                 QString());
    m_parser->addOption(ytdlFormatSelectionOption);

    m_parser->process(*m_app);
    m_aboutData.processCommandLine(m_parser.get());

    for (auto i = 0; i < m_parser->positionalArguments().size(); ++i) {
        addUrl(i, m_parser->positionalArguments().at(i));
    }
}

void Application::setupQmlSettingsTypes()
{
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "AudioSettings", AudioSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "GeneralSettings", GeneralSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "InformationSettings", InformationSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "MouseSettings", MouseSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "PlaybackSettings", PlaybackSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "PlaylistSettings", PlaylistSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "SubtitlesSettings", SubtitlesSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "VideoSettings", VideoSettings::self());
}

QString Application::version()
{
    return QString::fromStdString(HARUNA_VERSION_STRING);
}

QUrl Application::url(int key)
{
    if (m_urls.contains(key)) {
        return m_urls[key];
    }

    return {};
}

void Application::addUrl(int key, const QString &value)
{
    m_urls.insert(key, QUrl::fromUserInput(value, QDir::currentPath()));
}

void Application::handleSecondayInstanceMessage(const QByteArray &message, const QString activationToken)
{
    auto msgString = QString::fromStdString(message.data());
    QFileInfo fileInfo{msgString};
    if (fileInfo.exists() && fileInfo.isFile()) {
        Q_EMIT openUrl(QUrl::fromUserInput(msgString));
        qputenv("XDG_ACTIVATION_TOKEN", activationToken.toUtf8());
        raiseWindow();
    }
}

QStringList Application::availableGuiStyles()
{
    return QStyleFactory::keys();
}

void Application::setGuiStyle(const QString &style)
{
    if (style == QStringLiteral("Default")) {
        QApplication::setStyle(m_systemDefaultStyle);
        return;
    }
    QApplication::setStyle(style);
}

QAbstractItemModel *Application::colorSchemesModel()
{
    return m_schemes->model();
}

QQmlApplicationEngine *Application::qmlEngine() const
{
    return m_qmlEngine;
}

void Application::setQmlEngine(QQmlApplicationEngine *_qmlEngine)
{
    m_qmlEngine = _qmlEngine;
}

QCommandLineParser *Application::parser() const
{
    return m_parser.get();
}

void Application::activateColorScheme(const QString &name)
{
    m_schemes->activateScheme(m_schemes->indexForScheme(name));
}

int Application::frameworksVersionMinor()
{
    return KCONFIG_VERSION_MINOR;
}

int Application::qtMajorVersion()
{
    return QT_VERSION_MAJOR;
}

void Application::raiseWindow()
{
    QObject *rootObject = m_qmlEngine->rootObjects().constFirst();
    if (!rootObject) {
        return;
    }

    QWindow *window = qobject_cast<QWindow *>(rootObject);
    if (window) {
        KWindowSystem::updateStartupId(window);
        KWindowSystem::activateWindow(window);
    }
}

#include "moc_application.cpp"
