/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "miscutils.h"

#include <KFileItem>

using namespace Qt::StringLiterals;

MiscUtils *MiscUtils::instance()
{
    static MiscUtils i;
    return &i;
}

MiscUtils *MiscUtils::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

MiscUtils::MiscUtils()
    : QObject{nullptr}
{
}

QString MiscUtils::formatTime(const double time)
{
    int totalNumberOfSeconds = static_cast<int>(time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString hoursString = QStringLiteral("%1").arg(hours, 2, 10, QLatin1Char('0'));
    QString minutesString = QStringLiteral("%1").arg(minutes, 2, 10, QLatin1Char('0'));
    QString secondsString = QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0'));
    QString timeString = QStringLiteral("%1:%2:%3").arg(hoursString, minutesString, secondsString);

    return timeString;
}

QString MiscUtils::mimeType(QUrl url)
{
    KFileItem fileItem(url, KFileItem::NormalMimeTypeDetermination);
    return fileItem.mimetype();
}

// #include "moc_miscutils.h"
