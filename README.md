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

## Screenshots

(In Settings --> General --> Interface choose: Color Scheme: Breeze Dark; GUI Style: Breeze; Application Font: Supermolot Light (this free font can be downloaded here: https://www.1001fonts.com/search.html?search=supermolot ) Images are provided here: https://github.com/crojack/haruna-with-internet-radio-support/tree/master/images

<img width="2620" height="1600" alt="Screenshot_20251208_035843" src="https://github.com/user-attachments/assets/b4e5bc7e-f57c-428a-846f-bdd55838944e" />

<img width="2620" height="1600" alt="Screenshot_20251208_035909" src="https://github.com/user-attachments/assets/76097d36-3be6-4b9d-9210-a774f0cd39eb" />

<img width="2620" height="1600" alt="Screenshot_20251208_035923" src="https://github.com/user-attachments/assets/0916ff0e-ab95-4f07-8d1b-5061bb551b98" />

<img width="2620" height="1600" alt="Screenshot_20251208_035959" src="https://github.com/user-attachments/assets/e740a543-c6d8-46ea-b85a-a3646a51be37" />

<img width="2620" height="1600" alt="Screenshot_20251208_040041" src="https://github.com/user-attachments/assets/931d42bb-e5cb-4b89-b0d8-dc91e25cc7c5" />

<img width="2430" height="1532" alt="Screenshot_20251208_040102" src="https://github.com/user-attachments/assets/5dfcfe64-3273-4abc-a244-e001153b8358" />

<img width="2380" height="1552" alt="Screenshot_20251208_034720" src="https://github.com/user-attachments/assets/4d55d6a2-8b13-48f5-9409-56c117bf10f4" />

<img width="2380" height="1552" alt="Screenshot_20251208_034729" src="https://github.com/user-attachments/assets/ec7b1b2e-2984-47cf-8bd9-d34bb15f572b" />

<img width="2380" height="1552" alt="Screenshot_20251208_035102" src="https://github.com/user-attachments/assets/9c3253d2-f48d-4c7e-8719-f8e47d46b891" />

<img width="2380" height="1552" alt="Screenshot_20251208_035137" src="https://github.com/user-attachments/assets/c0aaedef-51cc-488a-b146-f4ce6c1770fc" />

<img width="3831" height="2021" alt="Screenshot_20251208_035528" src="https://github.com/user-attachments/assets/54bc288a-a954-451a-b02b-83b3fa33aa5d" />


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

### Debian/Ubuntu/Mint

Compiling a modern KDE 6 application like Haruna Enhanced on Debian 13 requires a specific combination of build tools, development headers, and runtime QML modules.

Below is the comprehensive list of packages and steps required to successfuly build it:

1. Build Tools & Core Components

These are the fundamental tools required to manage the project's build logic and compilation.

    cmake: The cross-platform build system generator.

    ninja-build: A fast build system used as an alternative to make.

    extra-cmake-modules: Essential KDE-specific CMake macros and scripts.

    pkg-config: A helper tool used when compiling applications and libraries.

2. Qt 6 Development Libraries

Haruna is built on Qt 6 and requires several specific modules, including private headers for deep integration.

    qt6-base-dev: Basic Qt 6 development files.

    qt6-base-private-dev: Private headers for Qt 6 Core.

    qt6-declarative-dev: Qt 6 QML and Quick development files.

    qt6-declarative-private-dev: Private headers for Qt 6 QML modules.

    qt6-5compat-dev: Compatibility layer for Qt 5 APIs used in Qt 6 projects.

    libqt6dbus6-dev: D-Bus support for Qt 6.

3. KDE Frameworks 6 (KF6) Development Files

Haruna relies heavily on KDE's modular libraries for functionality like metadata extraction, window management, and UI components.

    libkf6config-dev: Configuration settings framework.

    libkf6coreaddons-dev: Add-ons to the standard Qt Core.

    libkf6filemetadata-dev: Library for extracting and handling file metadata.

    libkf6i18n-dev: Internationalization and translation support.

    libkf6iconthemes-dev: Support for icon themes and widgets.

    libkf6kio-dev: Network and resource abstraction (KIO).

    libkf6windowsystem-dev: Window manager interaction libraries.

    libkf6colorscheme-dev: Classes for interacting with system color schemes.

    libkf6crash-dev: Support for application crash analysis.

    libkf6dbusaddons-dev: Classes that extend Qt D-Bus functionality.

    libkirigami-dev: Development headers for the Kirigami UI framework (Note: binary package name is often libkirigami-dev on Debian).

4. Multimedia & External Dependencies

    libmpv-dev: Development headers for the mpv media player engine.

    ffmpeg and libavcodec-dev, libavformat-dev, libswscale-dev: Required for media processing and thumbnail generation.

    breeze-icon-theme: Recommended for a consistent KDE look and feel.

5. Essential Runtime QML Modules

Even if the build succeeds, the application will crash at startup if these QML modules are not installed.

    qml6-module-org-kde-kirigami: The Kirigami UI components.

    qml6-module-org-kde-desktop: Native desktop styling for QML applications.

    qml6-module-org-kde-kitemmodels: Additional item/view models for data display.

    qml6-module-org-kde-coreaddons: QML bindings for CoreAddons.

6. Source-Built Dependencies

On Debian 13, some specific libraries may not yet be in the official repositories and must be built from source:

    MpvQt: A Qt wrapper for libmpv.

    KDSingleApplication: A helper for single-instance policy enforcement.

### Build Instructions for Debian 13

#### Standard Build (Recommended for Most Users)

Run this command from terminal:

```bash
sudo apt update && sudo apt install -y \
cmake ninja-build extra-cmake-modules pkg-config gettext \
qt6-base-dev qt6-base-private-dev qt6-declarative-dev qt6-declarative-private-dev \
qt6-5compat-dev libqt6sql6-sqlite \
libkf6config-dev libkf6coreaddons-dev libkf6filemetadata-dev libkf6i18n-dev \
libkf6iconthemes-dev libkf6kio-dev libkf6windowsystem-dev libkf6colorscheme-dev \
libkf6crash-dev libkf6dbusaddons-dev libkirigami-dev \
libmpv-dev ffmpeg libavcodec-dev libavformat-dev libswscale-dev breeze-icon-theme \
qml6-module-org-kde-kirigami qml6-module-org-kde-desktop qml6-module-org-kde-kitemmodels \
qml6-module-org-kde-coreaddons qml6-module-org-kde-config qml6-module-org-kde-kquickcontrols
```

This script automates the installation of the remaining source dependencies (MpvQt and KDSingleApplication) that are not yet available in the Debian 13 repositories.

Dependency Automation Script

Copy and paste this into your terminal to build and install the final required components:

```bash
# Create a temporary directory for building dependencies
mkdir -p ~/HarunaDeps && cd ~/HarunaDeps

# 1. Clone and patch MpvQt
git clone https://invent.kde.org/libraries/mpvqt.git
cd mpvqt
# Automatically comment out the faulty ClangFormat lines
sed -i 's/^kde_clang_format/# kde_clang_format/g' CMakeLists.txt
cmake -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/usr
sudo cmake --build build --target install
cd ..

# 2. Clone and patch KDSingleApplication
git clone https://github.com/KDAB/KDSingleApplication.git
cd KDSingleApplication
# Automatically comment out the faulty ClangFormat lines if present
sed -i 's/^kde_clang_format/# kde_clang_format/g' CMakeLists.txt
# Build with Qt6 support enabled
cmake -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/usr -DKDSingleApplication_QT6=ON
sudo cmake --build build --target install
cd ..

# Clean up build files
cd ~ && rm -rf ~/HarunaDeps

echo "-------------------------------------------------------"
echo "Dependencies successfully patched and installed."
echo "You can now return to your Haruna folder and run ninja."
echo "-------------------------------------------------------"
```
Finish the cleanup by running the removal command with sudo:

```bash
sudo rm -rf ~/HarunaDeps
```

If you have Qt6 and KF6 installed in standard system locations:

```bash
# Clone the repository
git clone https://github.com/crojack/haruna-with-internet-radio-support/
cd haruna-with-internet-radio-support/

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


