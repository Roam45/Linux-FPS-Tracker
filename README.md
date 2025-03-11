# Linux & Windows FPS Tracker

A simple FPS tracker application built using GTK and C++ that displays the frames per second (FPS) and other performance stats. It supports **Wayland**, **X11**, and **Windows**.

## Features

- Displays FPS on the top-left corner of the screen.
- Works on **Wayland**, **X11**.
- Uses GTK for the GUI.
- Semi-transparent background for a minimalistic look.

## Requirements

Before running the application, you'll need to install the necessary dependencies.

### Dependencies

- **GTK 3**: For creating the GUI.
- **X11** (for X11 users): To interact with the X server for FPS monitoring.
- **Wayland** (for Wayland users): For Wayland-based FPS monitoring.

## Installation

### Linux

#### Step 1: Install Dependencies

##### On Ubuntu/Debian-based Distributions:

```bash
sudo apt update
sudo apt install build-essential libgtk-3-dev libx11-dev libxcomposite-dev libxfixes-dev wayland-protocols
```

##### On Fedora:

```bash
sudo dnf install gcc-c++ gtk3-devel libX11-devel libXcomposite-devel libXfixes-devel wayland-protocols
```

##### On Arch Linux:

```bash
sudo pacman -S base-devel gtk3 x11-utils xorg-xcomposite xorg-xfixes wayland-protocols
```

#### Step 2 (optional): Clone the Repository

```bash
git clone https://github.com/Roam45/Linux-FPS-Tracker.git
cd Linux-FPS-Tracker
```

#### Step 3: Compile the Code

```bash
g++ -o fps_tracker main.cpp `pkg-config --cflags --libs gtk+-3.0` -lX11 -lXcomposite -lXfixes -lwayland-client -std=c++11 -pthread
```

#### Step 4: Run the Application

```bash
./fps_tracker
```

---

## How it Works

- The application detects the running display server:
  - **Wayland**: Uses the Wayland protocol for FPS tracking.
  - **X11**: Uses the `XComposite` extension for FPS tracking.
- The FPS is calculated and updated in real-time using a separate thread.

### Key Components:

- **GTK for GUI**: Creates the overlay window.
- **X11/Wayland**: Detects the platform and calculates FPS accordingly.
- **Threading**: Ensures real-time FPS updates without blocking the UI.

## Troubleshooting

### Linux

#### Issue: "Unable to open X display"
- Ensure you're running an X11 session.
- If using **Wayland**, ensure your compositor supports FPS tracking.

#### Issue: "Cannot run on the current display server."
- This error occurs if neither X11 nor Wayland is detected.

### Windows


This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

```
Copyright (C) 2025 Roam45

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
```

---

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

## Contact

For any questions, open an issue on GitHub:  
🔗 **[Linux-FPS-Tracker](https://github.com/Roam45/Linux-FPS-Tracker)**
