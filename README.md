# Haruna Media Player - Enhanced Edition

## Credits and Acknowledgments

This project is an enhanced fork of **Haruna Video Player**, an open-source video player built with Qt/QML and libmpv.

### Original Haruna Development Team

**Haruna** was created and is maintained by:
- **George Florea Bănuș** - Original author and lead developer
- **The KDE Community** - Contributors and maintainers

**Original Repository:** https://invent.kde.org/multimedia/haruna

### License Attribution

This enhanced version maintains full compliance with the original **GPL-3.0-or-later** license. All modifications and additions are released under the same license.

### What This Fork Adds

This fork extends the original Haruna with Internet Radio capabilities, dual-tab playlist management, and enhanced visual customization while preserving all core functionality of the original player. **All credit for the excellent base media player goes to the original Haruna development team.**

If you appreciate this software, please consider supporting the original Haruna project and the KDE community.
---------------

## Screenshots (In Settings --> General --> Interface choose: Color Scheme: Breeze Dark; GUI Style: Breeze; Application Font: Supermolot Light (this free font can be downloaded here: https://www.1001fonts.com/search.html?search=supermolot ) Images are provided here: https://github.com/crojack/haruna-with-internet-radio-support/tree/master/images

<img width="3840" height="2160" alt="Screenshot_20251123_023107" src="https://github.com/user-attachments/assets/2263de1d-e55c-4b80-8a51-72738b61e8e8" />

<img width="3840" height="2160" alt="Screenshot_20251123_023125" src="https://github.com/user-attachments/assets/57c704fc-a582-4abf-b627-be84777898d7" />

<img width="3840" height="2160" alt="Screenshot_20251123_023151" src="https://github.com/user-attachments/assets/27652df1-4bd5-4970-bc10-3ec514863524" />

<img width="3840" height="2160" alt="Screenshot_20251123_023221" src="https://github.com/user-attachments/assets/0f11d41f-9152-42bb-9de7-daf55f8ef06c" />

<img width="3840" height="2160" alt="Screenshot_20251123_023243" src="https://github.com/user-attachments/assets/b07406d2-71ef-4386-913b-0e5013363076" />

<img width="3840" height="2160" alt="Screenshot_20251123_023304" src="https://github.com/user-attachments/assets/8d3bbc4a-55fb-4fa7-953d-aeed8b252f21" />

<img width="3840" height="2160" alt="Screenshot_20251123_023333" src="https://github.com/user-attachments/assets/122f2592-f7fb-4fe2-bcdb-fc1cdaf8542c" />

<img width="3840" height="2160" alt="Screenshot_20251123_023341" src="https://github.com/user-attachments/assets/d938c759-5bd6-4798-b1ca-b1f509915b7b" />

<img width="3840" height="2160" alt="Screenshot_20251123_023432" src="https://github.com/user-attachments/assets/860f2c8d-ef1b-4566-b421-83f027eafd68" />

<img width="3840" height="2160" alt="Screenshot_20251123_023528" src="https://github.com/user-attachments/assets/b0da9d24-b5b4-4f5c-ba3a-247b103cb807" />

<img width="3840" height="2160" alt="Screenshot_20251123_023556" src="https://github.com/user-attachments/assets/b57205a3-01c3-430e-a5b2-5ff31d89332e" />


## New Features

### Internet Radio

A complete Internet Radio implementation integrated directly into the player:

- **Live Radio Streaming**: Stream thousands of radio stations from around the world via [radio-browser.info](https://www.radio-browser.info/)
- **Advanced Search**: Search stations by:
  - Station name (e.g., "BBC", "NPR", "Radio Paradise")
  - Country code (e.g., "GB", "US", "DE")
  - Genre (e.g., "jazz", "classical", "rock")
  - Artist/composer (e.g., "Mozart", "Beatles")
- **Favorites System**: 
  - Mark stations as favorites with a single click
  - Persistent favorites stored locally
  - Quick access with "fav" search keyword
- **Seamless Playback**: Radio stations play directly without cluttering your media playlist

### Enhanced Visual Experience

#### Background Images
- **Default Background**: Beautiful default background when player is idle
- **Music Background**: Custom background displayed during audio playback
- **Context-Aware Display**: Interface adapts based on content type

#### Album Art & Media Display
- **Embedded Album Art**: Automatically extracts and displays embedded album art from media files
- **Default Album Art**: Fallback artwork for files without embedded art
- **Default Radio Logo**: Clean, consistent branding for radio station playback

### Dual-Tab Interface

Clean separation of media types with two dedicated tabs:

1. **Videos And Music Tab**
   - Traditional playlist for local media files
   - Persistent playlist across sessions
   - Full playlist management (add, remove, reorder)

2. **Internet Radio Tab**
   - Browse and search radio stations
   - Station list with metadata (bitrate, tags)
   - Visual indicators for currently playing station
   - Favorites management

**Smart Playback Switching**: Playing content in one tab automatically stops the other tab, preventing conflicts.

### User Experience Improvements

- **Context-Aware Interface**: UI adapts based on content type (video, music, radio)
- **Visual Feedback**: Clear indicators for:
  - Currently playing station/track
  - Favorite stations
  - Loading states during search
- **Blur Effects**: Elegant translucent blurred backgrounds for overlays
- **Responsive Design**: Interface scales appropriately for different window sizes

---
## Installation

### Prerequisites

- Qt 6.6 or later
- KDE Frameworks 6.0 or later
- MPV media player library
- FFmpeg
- CMake 3.15 or later
- Ninja build system (or make)

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt install cmake ninja-build qt6-base-dev qt6-declarative-dev \
  libkf6config-dev libkf6filemetadata-dev libkf6i18n-dev \
  libkf6iconthemes-dev libkf6coreaddons-dev libkf6kio-dev \
  libkf6kirigami2-dev libkf6windowsystem-dev libkf6crash-dev \
  libmpv-dev ffmpeg
```

**Fedora:**
```bash
sudo dnf install cmake ninja-build qt6-qtbase-devel qt6-qtdeclarative-devel \
  kf6-kconfig-devel kf6-kfilemetadata-devel kf6-ki18n-devel \
  kf6-kiconthemes-devel kf6-kcoreaddons-devel kf6-kio-devel \
  kf6-kirigami-devel kf6-kwindowsystem-devel kf6-kcrash-devel \
  mpv-libs-devel ffmpeg
```

**Arch Linux:**
```bash
sudo pacman -S cmake ninja qt6-base qt6-declarative \
  kconfig kfilemetadata ki18n kiconthemes kcoreaddons kio \
  kirigami kwindowsystem kcrash mpv ffmpeg
```

### Build Instructions

#### Standard Build (Recommended for Most Users)

If you have Qt6 and KF6 installed in standard system locations:
```bash
# Clone the repository
git clone https://github.com/crojack/haruna-with-internet-radio-support/
cd haruna-enhanced

# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
ninja -C build

# Run from build directory
./build/bin/haruna

# Optional: Install system-wide
sudo cmake --install build
```

#### Alternative: Using Make Instead of Ninja
```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run
./build/bin/haruna

# Optional: Install
sudo cmake --install build
```

#### Custom Qt6/KF6 Locations

If you have Qt6 and KF6 installed in custom locations (e.g., `/opt/qt6` and `/opt/kf6`):
```bash
# Configure with custom paths
cmake -B build -G Ninja \
  -DCMAKE_PREFIX_PATH="/opt/qt6;/opt/kf6" \
  -DCMAKE_BUILD_TYPE=Release \
  -DQT_DEFAULT_MAJOR_VERSION=6

# Build
ninja -C build

# Run
./build/bin/haruna
```

#### Custom Install Prefix

To install to a custom location instead of system-wide:
```bash
# Configure with custom install prefix
cmake -B build -G Ninja \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local" \
  -DCMAKE_BUILD_TYPE=Release

# Build
ninja -C build

# Install to ~/.local
cmake --install build

# Run (make sure ~/.local/bin is in your PATH)
haruna
```

### Build Options

Additional CMake options you can use:
```bash
# Debug build (for development)
-DCMAKE_BUILD_TYPE=Debug

# Specify Qt version explicitly
-DQT_DEFAULT_MAJOR_VERSION=6

# Custom install location
-DCMAKE_INSTALL_PREFIX=/your/custom/path

# Specify custom Qt/KF6 locations
-DCMAKE_PREFIX_PATH="/path/to/qt6;/path/to/kf6"
```

### Verifying Your Build

After building, you can verify the installation:
```bash
# Check if the binary works
./build/bin/haruna --version

# Check dependencies
ldd ./build/bin/haruna
```

## Configuration

### Custom Images

Place your custom images in `~/.local/share/KDE/haruna/images/`:

#### Background Images
```
~/.local/share/KDE/haruna/images/background/
  ├── background.jpg          # Default idle background
  └── music-default.jpg       # Music playback background
```

#### Album Art
```
~/.local/share/KDE/haruna/images/
  └── album-art-default.jpg   # Default album art
```

#### Radio Logo
```
~/.local/share/KDE/haruna/images/radio-stations-logos/
  └── radio-default.png       # Default radio station logo
```

### Radio Favorites

Favorites are automatically saved to:
```
~/.local/share/KDE/haruna/favorite_radio_stations.json
```

---

## Usage

### Internet Radio

1. **Open Internet Radio Tab**: Click the "Internet Radio" tab in the playlist panel
2. **Search for Stations**: 
   - Type your search query in the search field
   - Press Enter or click "Search"
   - Type "fav" to show favorites
3. **Play a Station**: Click on any station in the list
4. **Add to Favorites**: Click the star icon next to the station name

### Managing Playlists

- **Videos And Music**: Use for local media files as you would in standard Haruna
- **Switch Between Tabs**: Click tab headers to switch between media types
- **Clean Separation**: Radio stations don't clutter your media playlist

---

## Technical Details

### Architecture

- **C++ Backend**: RadioStationsModel for API integration and data management
- **QML Frontend**: Modern Qt Quick interface with Kirigami components
- **MPV Integration**: Direct MPV library integration for playback
- **SQLite Database**: Persistent storage for playback positions and metadata

### API Integration

Radio station data is sourced from [radio-browser.info](https://www.radio-browser.info/), a free and open-source radio station directory maintained by the community.

---

## License

This enhanced version maintains the same **GPL-3.0-or-later** license as the original Haruna project.

### GPL-3.0-or-later

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

---

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

When contributing, please:
- Maintain compatibility with the original Haruna codebase where possible
- Follow the existing code style and conventions
- Test your changes thoroughly
- Document new features clearly

---

## Special Thanks

- **George Florea Bănuș**: For creating Haruna and making it available as open-source software
- **Haruna Contributors**: For ongoing development and maintenance of the original project
- **KDE Community**: For the Kirigami framework and KDE Frameworks
- **radio-browser.info**: For providing the free and open radio station API
- **MPV Project**: For the powerful and flexible media playback engine
- **Qt Project**: For the Qt framework and QML

---

## Support

For issues specific to the enhanced features (Internet Radio, dual tabs, custom images), please open an issue in this repository.

For issues with core Haruna functionality, please refer to the [original Haruna repository](https://invent.kde.org/multimedia/haruna).

---

## Links

- **Original Haruna**: https://invent.kde.org/multimedia/haruna
- **KDE Applications**: https://apps.kde.org/
- **Radio Browser API**: https://www.radio-browser.info/
- **MPV**: https://mpv.io/


