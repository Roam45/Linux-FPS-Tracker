# Linux-FPS-Tracker

A simple FPS tracker application built using GTK and C++ that displays the frames per second (FPS) and other performance stats. It supports both **Wayland** and **X11** display servers on Linux-based systems.

## Features

- Displays FPS on the top-left corner of the screen.
- Works on both **Wayland** and **X11** display servers.
- Uses GTK for the GUI and X11/Wayland for frame rate monitoring.
- Semi-transparent background for a minimalistic look.

## Requirements

Before running the application, you'll need to install the necessary dependencies.

### Dependencies

- **GTK 3**: For creating the GUI.
- **X11** (for X11 users): To interact with the X server for FPS monitoring.
- **Wayland** (for Wayland users): For Wayland-based FPS monitoring.

## Installation

### Step 1: Install Dependencies

#### On Ubuntu/Debian-based Distributions:

To install the necessary dependencies, run the following command in your terminal:

```bash
sudo apt update
sudo apt install build-essential libgtk-3-dev libx11-dev libxcomposite-dev libxfixes-dev wayland-protocols
```

#### On Fedora:

```bash
sudo dnf install gcc-c++ gtk3-devel libX11-devel libXcomposite-devel libXfixes-devel wayland-protocols
```

#### On Arch Linux:

```bash
sudo pacman -S base-devel gtk3 x11-utils xorg-xcomposite xorg-xfixes wayland-protocols
```

### Step 2 (optional): Clone the Repository

Clone the repository to your local machine:

```bash
git clone https://github.com/Roam45/Linux-FPS-Tracker.git
cd Linux-FPS-Tracker
```

### Step 3: Compile the Code

Run the following command to compile the code:

```bash
g++ -o fps_tracker main.cpp `pkg-config --cflags --libs gtk+-3.0` -lX11 -lXcomposite -lXfixes -lwayland-client -std=c++11 -pthread
```

### Step 4: Run the Application

After compiling, you can run the application:

```bash
./fps_tracker
```

The FPS tracker window should appear in the top-left corner of your screen, and it will show FPS and performance stats.

## How it Works

- The application first checks whether it is running on **Wayland** or **X11**.
  - If running on **Wayland**, it uses the Wayland protocol to track FPS.
  - If running on **X11**, it uses the X11 server's `XComposite` extension to track FPS.
- It then calculates the FPS and updates the label in the window every second.
- The application runs continuously and updates the FPS information in real-time.

### Key Components:

- **GTK for the GUI**: The application uses GTK to create a window and display the FPS and performance stats.
- **X11/Wayland for FPS calculation**: Based on the display server detected, either X11 or Wayland is used to calculate the FPS.
- **Threading**: A separate thread is used to calculate the FPS, so it doesn't block the GUI.

## Troubleshooting

### Issue: "Unable to open X display"
- Ensure that you're running the application on an X11 session.
- If you're using **Wayland**, ensure the application supports your specific display server.

### Issue: "Cannot run on the current display server."
- This error occurs if neither X11 nor Wayland is detected. Ensure that your system is using a supported display server.

## License

This project is licensed under the GNU License - see the [LICENSE](LICENSE) file for details. 

---

Feel free to contribute or create issues for any bugs or features you would like to see!
