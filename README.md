
# Haruna with internet radio support

Haruna with internet radio support is an open source media player built with Qt/QML and libmpv. It's a temporary fork of the official Haruna media player.
This fork is used for experimenting with internet radio functionality.

Improved user experience


When you run the original Haruna media player, the video area appears black. This version lets Haruna load custom background images. For now you'll need to create /haruna/images directory in ~/.local/share/KDE with these subdirectories: background, default-album-art and radio-stations-logos. In the background directory put a background.png image, in the default-album directory put a music-default.png image and in the radio-stations-logos directory put a radio-default.png image. It's up to you which images you choose. 


<img width="3840" height="2160" alt="Screenshot_20251118_012442" src="https://github.com/user-attachments/assets/9ec4bdca-0909-4ad1-9453-93b4cbb62042" />



The original Haruna also doesn’t display default album art for music without embedded artwork. Instead, it shows the previous track’s embedded album art - so if you’re listening to Mozart and the previous track was a Coldplay song, Haruna will show the Coldplay album art while playing Mozart.
This fork adds support for custom default album art for tracks that don’t have embedded artwork.
The same applies to internet radio stations - if a station doesn’t provide its own logo, this fork displays a default radio-station logo.



<img width="3840" height="2160" alt="Screenshot_20251117_155332" src="https://github.com/user-attachments/assets/5c0d5fdd-2179-4e33-966a-99969fa1242c" />



More screenshots:



<img width="3840" height="2160" alt="Screenshot_20251117_021025" src="https://github.com/user-attachments/assets/12371035-d8ea-453e-84f8-0f0b8b95bc8d" />

<img width="3840" height="2160" alt="Screenshot_20251117_021106" src="https://github.com/user-attachments/assets/76120491-be75-412c-8bc0-170be2205984" />

<img width="3840" height="2160" alt="Screenshot_20251117_021205" src="https://github.com/user-attachments/assets/1555ee3b-1a9d-4c58-a119-9d9796e44b93" />

<img width="3840" height="2160" alt="Screenshot_20251117_021229" src="https://github.com/user-attachments/assets/ba76c570-5ac2-4e89-b1d3-2d5734cb0e85" />

<img width="3840" height="2160" alt="Screenshot_20251117_021333" src="https://github.com/user-attachments/assets/456fd1e6-8cf6-4dd0-b55e-19443b583f62" />

<img width="3840" height="2160" alt="Screenshot_20251117_021527" src="https://github.com/user-attachments/assets/5102b75d-7d1c-4d3f-b799-016d43db614b" />



# Features

Two separate playlists, one for videos and music and the other one for internet radio. 

Search for internet radio stations by name, genres, country and city.

Add favourites to favourites playlist.

# Dependencies
Dependencies will be printed by `cmake` when building.

# Build

```bash
git clone https://github.com/crojack/haruna-with-internet-radio-support
cd haruna-with-internet-radio-support
cmake -B build -G Ninja
cmake --build build
(if the build is ok, you can try the application by launching it from the build directory: ./build/bin/haruna)
If everything works as it should run:
cmake --install build
```
