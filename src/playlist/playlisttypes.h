#ifndef PLAYLISTTYPES_H
#define PLAYLISTTYPES_H

#include <QObject>

namespace Playlist {

    Q_NAMESPACE

    enum class PlaylistType {
        Regular,
        Radio
    };
    Q_ENUM_NS(PlaylistType)

} // namespace Playlist

#endif // PLAYLISTTYPES_H

