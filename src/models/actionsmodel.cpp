/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "actionsmodel.h"

#include <QMessageBox>
#include <QPushButton>

#include <KConfigGroup>
#include <KLocalizedString>

#include "pathutils.h"

using namespace Qt::StringLiterals;

ActionsModel::ActionsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_config(KSharedConfig::openConfig(PathUtils::instance()->configFilePath(PathUtils::ConfigFile::Shortcuts)))
{
    Action action;
    action.name = QStringLiteral("openActionsDialogAction");
    action.text = i18nc("@action", "Open Actions Dialog");
    action.defaultShortcut = Qt::CTRL | Qt::Key_QuoteLeft; // Key_QuoteLeft = backtick
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("aboutHarunaAction");
    action.text = i18nc("@action", "About Haruna");
    action.iconName = QStringLiteral("haruna");
    action.defaultShortcut = Qt::Key_F1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("reportBugAction");
    action.text = i18nc("@action", "Report bug");
    action.iconName = QStringLiteral("tools-report-bug");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_F1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("sponsorAction");
    action.text = i18nc("@action", "Sponsor Haruna");
    action.iconName = QStringLiteral("help-donate-ars");
    action.defaultShortcut = Qt::Key_Dollar;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("audioCycleUpAction");
    action.text = i18nc("@action", "Cycle Up Audio Track");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("audioCycleDownAction");
    action.text = i18nc("@action", "Cycle Down Audio Track");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("configureAction");
    action.text = i18nc("@action", "Settings");
    action.iconName = QStringLiteral("configure");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("configureShortcutsAction");
    action.text = i18nc("@action", "Configure Keyboard Shortcuts");
    action.iconName = QStringLiteral("configure-shortcuts");
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("exitFullscreenAction");
    action.text = i18nc("@action", "Exit Fullscreen");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Escape;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("frameStepForwardAction");
    action.text = i18nc("@action", "Step Frame Forward");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QStringLiteral("Move one frame forward, then pause");
    m_actions << action;

    action.name = QStringLiteral("frameStepBackwardAction");
    action.text = i18nc("@action", "Step Frame Backward");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QStringLiteral("Move one frame backward, then pause");
    m_actions << action;

    action.name = QStringLiteral("loadLastPlayedFileAction");
    action.text = i18nc("@action", "Load Last Played File");
    action.iconName = QStringLiteral("document-open-recent");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("muteAction");
    action.text = i18nc("@action", "Mute");
    action.iconName = QStringLiteral("player-volume-muted");
    action.defaultShortcut = Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("openContextMenuAction");
    action.text = i18nc("@action", "Open Context Menu");
    action.iconName = QStringLiteral("application-menu");
    action.defaultShortcut = Qt::Key_Menu;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = i18nc("@action", "Opens and closes the context menu at the position of the mouse pointer");
    m_actions << action;

    action.name = QStringLiteral("openFileAction");
    action.text = i18nc("@action", "Open File");
    action.iconName = QStringLiteral("folder-videos");
    action.defaultShortcut = Qt::CTRL | Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("addExternalSubtitleAction");
    action.text = i18nc("@action", "Add External Subtitle…");
    action.iconName = QStringLiteral("add-subtitle");
    action.defaultShortcut = Qt::ALT | Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("openUrlAction");
    action.text = i18nc("@action", "Open URL");
    action.iconName = QStringLiteral("internet-services");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedIncreaseAction");
    action.text = i18nc("@action", "Increase Playback Speed");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_BracketRight;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedDecreaseAction");
    action.text = i18nc("@action", "Decrease Playback Speed");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_BracketLeft;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedResetAction");
    action.text = i18nc("@action", "Reset Playback Speed");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Backspace;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("playPauseAction");
    action.text = i18nc("@action", "Play/Pause");
    action.iconName = QStringLiteral("media-playback-pause");
    action.defaultShortcut = Qt::Key_Space;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("playNextAction");
    action.text = i18nc("@action", "Play Next");
    action.iconName = QStringLiteral("media-skip-forward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("playPreviousAction");
    action.text = i18nc("@action", "Play Previous");
    action.iconName = QStringLiteral("media-skip-backward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("quitApplicationAction");
    action.text = i18nc("@action", "Quit");
    action.iconName = QStringLiteral("application-exit");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Q;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("restartPlaybackAction");
    action.text = i18nc("@action", "Restart Playback");
    action.iconName = QStringLiteral("edit-reset");
    action.defaultShortcut = Qt::Key_F5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekForwardSmallAction");
    action.text = i18nc("@action", "Seek Small Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekBackwardSmallAction");
    action.text = i18nc("@action", "Seek Small Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekForwardMediumAction");
    action.text = i18nc("@action", "Seek Medium Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekBackwardMediumAction");
    action.text = i18nc("@action", "Seek Medium Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekForwardBigAction");
    action.text = i18nc("@action", "Seek Big Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekBackwardBigAction");
    action.text = i18nc("@action", "Seek Big Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekNextChapterAction");
    action.text = i18nc("@action", "Seek To Next Chapter");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_PageUp;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekPreviousChapterAction");
    action.text = i18nc("@action", "Seek To Previous Chapter");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_PageDown;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekNextSubtitleAction");
    action.text = i18nc("@action", "Seek To Next Subtitle");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekPreviousSubtitleAction");
    action.text = i18nc("@action", "Seek To Previous Subtitle");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("seekToWatchLaterPositionAction");
    action.text = i18nc("@action", "Seek To Watch Later Position");
    action.iconName = QStringLiteral("pin");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("setLoopAction");
    action.text = i18nc("@action", "Set Loop");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("screenshotAction");
    action.text = i18nc("@action", "Screenshot");
    action.iconName = QStringLiteral("image-x-generic");
    action.defaultShortcut = Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleToggleAction");
    action.text = i18nc("@action", "Toggle Subtitles");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleIncreaseFontSizeAction");
    action.text = i18nc("@action", "Increase Subtitle Font Size");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleDecreaseFontSizeAction");
    action.text = i18nc("@action", "Decrease Subtitle Font Size");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_X;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleMoveUpAction");
    action.text = i18nc("@action", "Move Subtitle Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleMoveDownAction");
    action.text = i18nc("@action", "Move Subtitle Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleQuickenAction");
    action.text = i18nc("@action", "Show Subtitles Earlier");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleDelayAction");
    action.text = i18nc("@action", "Show Subtitles Later");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleCycleUpAction");
    action.text = i18nc("@action", "Cycle Up Subtitle");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("subtitleCycleDownAction");
    action.text = i18nc("@action", "Cycle Down Subtitle");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("toggleDeinterlacingAction");
    action.text = i18nc("@action", "Toggle Deinterlacing");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_D;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("toggleFullscreenAction");
    action.text = i18nc("@action", "Toggle Fullscreen");
    action.iconName = QStringLiteral("view-fullscreen");
    action.defaultShortcut = Qt::Key_F;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("toggleMenuBarAction");
    action.text = i18nc("@action", "Toggle Menu Bar");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("toggleHeaderAction");
    action.text = i18nc("@action", "Toggle Header");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_H;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("togglePlaylistAction");
    action.text = i18nc("@action", "Playlist");
    action.iconName = QStringLiteral("view-media-playlist");
    action.defaultShortcut = Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("videoPanXLeftAction");
    action.text = i18nc("@action", "Move Video Left");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("videoPanXRightAction");
    action.text = i18nc("@action", "Move Video Right");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("videoPanYUpAction");
    action.text = i18nc("@action", "Move Video Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("videoPanYDownAction");
    action.text = i18nc("@action", "Move Video Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("rotateClockwiseAction");
    action.text = i18nc("@action", "Rotate clockwise");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("rotateCounterClockwiseAction");
    action.text = i18nc("@action", "Rotate counter clockwise");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_E;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("volumeUpAction");
    action.text = i18nc("@action", "Volume Up");
    action.iconName = QStringLiteral("audio-volume-high");
    action.defaultShortcut = Qt::Key_9;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("volumeDownAction");
    action.text = i18nc("@action", "Volume Down");
    action.iconName = QStringLiteral("audio-volume-low");
    action.defaultShortcut = Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("zoomInAction");
    action.text = i18nc("@action", "Zoom In");
    action.iconName = QStringLiteral("zoom-in");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Plus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("zoomOutAction");
    action.text = i18nc("@action", "Zoom Out");
    action.iconName = QStringLiteral("zoom-out");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Minus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("zoomResetAction");
    action.text = i18nc("@action", "Reset Zoom");
    action.iconName = QStringLiteral("zoom-original");
    action.defaultShortcut = Qt::CTRL | Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("contrastUpAction");
    action.text = i18nc("@action", "Increase Contrast");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("contrastDownAction");
    action.text = i18nc("@action", "Decrease Contrast");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("contrastResetAction");
    action.text = i18nc("@action", "Reset Contrast");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("brightnessUpAction");
    action.text = i18nc("@action", "Increase Brightness");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("brightnessDownAction");
    action.text = i18nc("@action", "Decrease Brightness");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_4;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("brightnessResetAction");
    action.text = i18nc("@action", "Reset Brightness");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("gammaUpAction");
    action.text = i18nc("@action", "Increase Gamma");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("gammaDownAction");
    action.text = i18nc("@action", "Decrease Gamma");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_6;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("gammaResetAction");
    action.text = i18nc("@action", "Reset Gamma");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("saturationUpAction");
    action.text = i18nc("@action", "Increase Saturation");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("saturationDownAction");
    action.text = i18nc("@action", "Decrease Saturation");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_8;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;

    action.name = QStringLiteral("saturationResetAction");
    action.text = i18nc("@action", "Reset Saturation");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QString{};
    m_actions << action;
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) {
        return 0;
    }

    return m_actions.count();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto action = m_actions.at(index.row());

    switch (role) {
    case NameRole:
        return QVariant(action.name);
    case TextRole:
        return QVariant(action.text);
    case IconRole:
        return QVariant(action.iconName);
    case ShortcutRole:
        return QVariant(action.shortcut.toString(QKeySequence::PortableText));
    case DefaultShortcutRole:
        return QVariant(action.defaultShortcut.toString(QKeySequence::PortableText));
    case DescriptionRole:
        return QVariant(action.description);
    case TypeRole:
        return QVariant(action.type);
    case SearchStringRole:
        return QVariant(QStringLiteral("%1 %2 %3 %4").arg(action.text, action.name, action.description, action.shortcut.toString(QKeySequence::PortableText)));
    }

    return QVariant();
}

QHash<int, QByteArray> ActionsModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles{
        {NameRole,            QByteArrayLiteral("actionName")},
        {TextRole,            QByteArrayLiteral("actionText")},
        {IconRole,            QByteArrayLiteral("actionIcon")},
        {ShortcutRole,        QByteArrayLiteral("actionShortcut")},
        {DefaultShortcutRole, QByteArrayLiteral("actionDefaultShortcut")},
        {DescriptionRole,     QByteArrayLiteral("actionDescription")},
        {TypeRole,            QByteArrayLiteral("actionType")},
        {SearchStringRole,    QByteArrayLiteral("searchString")},
    };
    // clang-format on

    return roles;
}

void ActionsModel::appendCustomAction(const Action &action)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_actions.append(action);
    endInsertRows();
}

void ActionsModel::editCustomAction(const QString &name, const QString &text, const QString &description)
{
    for (int i{0}; i < m_actions.count(); ++i) {
        if (m_actions[i].name == name) {
            m_actions[i].text = text;
            m_actions[i].description = description;
            Q_EMIT dataChanged(index(i, 0), index(i, 0));
            return;
        }
    }
}

bool ActionsModel::saveShortcut(const QString &name, QKeySequence keySequence)
{
    for (int i{0}; i < m_actions.count(); ++i) {
        if (m_actions[i].name == name) {
            return saveShortcut(i, keySequence);
        }
    }
    return false;
}

bool ActionsModel::saveShortcut(int row, QKeySequence keySequence)
{
    auto group = m_config->group(QStringLiteral("Shortcuts"));
    // action whose shortcut is changed
    auto action = &m_actions[row];

    // if shortcut is being cleared, no need to search for a conflict
    if (!keySequence.isEmpty()) {
        // if shortcut is used, this is the action holding the shortcut
        Action *result = nullptr;
        int i{0};
        for (; i < m_actions.count(); ++i) {
            if (m_actions[i].shortcut == keySequence) {
                result = &m_actions[i];
                break;
            }
        }

        if (result != nullptr && action->name == result->name) {
            return false;
        }

        if (result != nullptr) {
            // shortcut is used by an action
            // ask user what whether to reassign or to cancel
            if (keyConflictMessageBox(result->text)) {
                // user chose reassign, remove shortcut from action holding it
                result->shortcut = QKeySequence{};
                group.writeEntry(result->name, QString{});
                Q_EMIT shortcutChanged(result->name, result->shortcut);
                Q_EMIT dataChanged(index(i, 0), index(i, 0));
            } else {
                // user chose cancel
                return false;
            }
        }
    }
    // set shortcut on the action being changed
    action->shortcut = keySequence;
    group.writeEntry(action->name, action->shortcut.toString(QKeySequence::PortableText));
    Q_EMIT shortcutChanged(action->name, action->shortcut);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));

    return group.sync();
}

QKeySequence ActionsModel::getShortcut(const QString &key, const QKeySequence &defaultValue) const
{
    auto v = defaultValue.toString(QKeySequence::PortableText);
    auto group = m_config->group(QStringLiteral("Shortcuts"));
    QKeySequence keySequence(group.readEntry(key, v), QKeySequence::PortableText);
    return keySequence;
}

bool ActionsModel::keyConflictMessageBox(const QString &actionText)
{
    auto message = i18nc("@info",
                         "The chosen shortcut conflicts with "
                         "the shortcut of the <b>%1</b> action.<br><br>"
                         "Do you want to reassign the shortcut?",
                         actionText);
    QMessageBox msgBox;
    msgBox.setText(message);
    QPushButton *reassignBtn = msgBox.addButton(i18nc("@action:button", "Reassign"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.exec();

    if (msgBox.clickedButton() == reassignBtn) {
        return true;
    }
    return false;
}

QList<Action> &ActionsModel::actions()
{
    return m_actions;
}

void ActionsModel::signalEmitter(const QString &actionName)
{
    QMetaObject::invokeMethod(this, actionName.toUtf8().data());
}

ProxyActionsModel::ProxyActionsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_nameRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    m_textRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    m_typeRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
}

bool ProxyActionsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex typeIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    QString name = sourceModel()->data(nameIndex, ActionsModel::NameRole).toString();
    QString text = sourceModel()->data(nameIndex, ActionsModel::TextRole).toString();
    QString type = sourceModel()->data(typeIndex, ActionsModel::TypeRole).toString();

    return ((name.contains(m_nameRegExp) || text.contains(m_nameRegExp)) && type.contains(m_typeRegExp));
}

void ProxyActionsModel::setNameFilter(const QString &regExp)
{
    m_nameRegExp.setPattern(regExp);
    invalidateFilter();
}

void ProxyActionsModel::setTypeFilter(const QString &regExp)
{
    m_typeRegExp.setPattern(regExp);
    invalidateFilter();
}

bool ProxyActionsModel::saveShortcut(int row, QKeySequence keySequence)
{
    auto actionsModel = qobject_cast<ActionsModel *>(sourceModel());
    return actionsModel->saveShortcut(mapToSource(index(row, 0)).row(), keySequence);
}

#include "moc_actionsmodel.cpp"
