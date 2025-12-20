/*
 * SPDX-FileCopyrightText: 2020 George Florea BÄƒnuÈ™ <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QQuickWindow>

#include <KLocalizedString>

#include <KDSingleApplication>

#include "application.h"
#include "generalsettings.h"
#include "thumbnailimageprovider.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    qSetMessagePattern(QStringLiteral("%{function}() : %{message}\n"));

    QApplication::setOrganizationName(QStringLiteral("KDE"));
    QApplication::setApplicationName(QStringLiteral("Haruna"));
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QApplication::setApplicationDisplayName(QStringLiteral("Haruna - Media Player"));
    QApplication::setApplicationVersion(Application::version());

    // required by mpv
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
    QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));

    QApplication qApplication(argc, argv);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("haruna")));
    KLocalizedString::setApplicationDomain("haruna");

    auto application = Application::instance();

    KDSingleApplication kdsApp;
    if (kdsApp.isPrimaryInstance()) {
        QObject::connect(&kdsApp, &KDSingleApplication::messageReceived, [=](const QByteArray &message) {
            QString file;
            QString token;

            QByteArray byteArray{message};
            QDataStream dataStream(&byteArray, QIODevice::ReadOnly);
            dataStream >> file >> token;

            application->handleSecondayInstanceMessage(file.toUtf8(), token);
        });
    } else {
        if (GeneralSettings::self()->useSingleInstance()) {
            QCommandLineParser clParser;
            clParser.process(qApplication);
            if (clParser.positionalArguments().size() > 0) {
                const QString file = clParser.positionalArguments().constFirst();
                const QString token = qEnvironmentVariable("XDG_ACTIVATION_TOKEN");

                QByteArray byteArray;
                QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
                dataStream << file << token;

                kdsApp.sendMessage(byteArray);
            }
            exit(EXIT_SUCCESS);
        }
    }

    QQmlApplicationEngine engine(&qApplication);
    engine.addImageProvider(QStringLiteral("thumbnail"), new ThumbnailImageProvider());
    engine.rootContext()->setContextObject(new KLocalizedContext(Application::instance()));

    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/org/kde/haruna/qml/Main.qml")));
    application->setQmlEngine(&engine);

    return qApplication.exec();
}
