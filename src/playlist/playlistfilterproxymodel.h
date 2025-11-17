/*
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTFILTERPROXYMODEL_H
#define PLAYLISTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include "playlistmodel.h"
#include "playlistproxymodel.h"
#include "playlistsortproxymodel.h"
#include "playlisttypes.h"

// âœ… Forward declarations
class PlaylistMultiProxiesModel;
class PlaylistModel;
class PlaylistSortProxyModel;
class PlaylistProxyModel;

class PlaylistFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistFilterProxyModel)

public:
    explicit PlaylistFilterProxyModel(QObject *parent = nullptr);
    friend class PlaylistMultiProxiesModel;
    friend class MpvItem;

    void setPlaylistType(Playlist::PlaylistType type);
    Playlist::PlaylistType playlistType() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // clang-format off
    enum class Selection {
        Clear,
        ClearSingle,
        Single,
        Toggle,
        Range,
        RangeStart,
        Invert,
        All,
    };
    Q_ENUM(Selection)
    // clang-format on

    enum Filter {
        Name = Qt::UserRole,
        Title,
    };
    Q_ENUM(Filter)

    Q_PROPERTY(uint selectionCount READ selectionCount NOTIFY selectionCountChanged)
    uint selectionCount();

    Q_PROPERTY(uint itemCount READ itemCount NOTIFY itemCountChanged)
    uint itemCount();

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    QString searchText();
    void setSearchText(QString text);

    // âœ… NEW: Expose playlist name to QML
    Q_PROPERTY(QString playlistName READ playlistName NOTIFY playlistNameChanged)
    QString playlistName() const;

    Q_INVOKABLE uint getPlayingItem();
    Q_INVOKABLE void setPlayingItem(uint i);
    Q_INVOKABLE void playNext();
    Q_INVOKABLE void playPrevious();
    Q_INVOKABLE void saveM3uFile(const QString &path);
    Q_INVOKABLE void highlightInFileManager(uint row);
    Q_INVOKABLE void removeItem(uint row);
    Q_INVOKABLE void removeItems();
    Q_INVOKABLE void renameFile(uint row);
    Q_INVOKABLE void trashFile(uint row);
    Q_INVOKABLE void trashFiles();
    Q_INVOKABLE void copyFileName(uint row);
    Q_INVOKABLE void copyFilePath(uint row);
    Q_INVOKABLE QString getFilePath(uint row);
    Q_INVOKABLE bool isLastItem(uint row);
    Q_INVOKABLE void moveItems(uint row, uint destinationRow);
    Q_INVOKABLE void selectItem(uint row, Selection selectionMode);
    Q_INVOKABLE void refreshData();
    Q_INVOKABLE void addFilesAndFolders(QList<QUrl> urls, PlaylistModel::Behavior behavior, uint insertOffset = 0);
    Q_INVOKABLE bool isDirectory(const QUrl &url);

    Q_INVOKABLE void sortItems(PlaylistSortProxyModel::Sort sortMode);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void addItem(const QString &path, PlaylistModel::Behavior behavior);
    Q_INVOKABLE void addItem(const QUrl &url, PlaylistModel::Behavior behavior);
    Q_INVOKABLE void addItems(const QList<QUrl> &urls, PlaylistModel::Behavior behavior);

Q_SIGNALS:
    void selectionCountChanged();
    void itemCountChanged();
    void itemsSorted();
    void itemsMoved();
    void itemsRemoved();
    void itemsInserted();
    void searchTextChanged();
    void playlistNameChanged();  // âœ… NEW signal

private:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void shufflePlaylistModel();

    PlaylistProxyModel *playlistProxyModel() const;
    PlaylistSortProxyModel *playlistSortProxyModel() const;
    PlaylistModel *playlistModel() const;

    Playlist::PlaylistType m_playlistType{Playlist::PlaylistType::Regular};

    QModelIndex mapFromPlaylistModel(uint row) const;
    QModelIndex mapToPlaylistModel(uint row) const;

    void splitItemSelection(const QModelIndexList &original, int splitRow, bool isTopDown, QModelIndexList &lowerPart, QModelIndexList &upperPart);
    QModelIndexList selectedRows() const;
    void saveInternalPlaylist(const QString &path, const QString &playlistName);

    std::unique_ptr<PlaylistModel> m_playlistModel;
    std::unique_ptr<PlaylistSortProxyModel> m_playlistSortProxyModel;
    std::unique_ptr<PlaylistProxyModel> m_playlistProxyModel;
    QItemSelectionModel m_selectionModel;
    bool m_scheduledReshuffle{false};
};

#endif // PLAYLISTFILTERPROXYMODEL_H
