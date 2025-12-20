# Windows Auto Clicker - C++ GUI Application

A Windows desktop application that automatically clicks the mouse at 10 times per second with a simple GUI interface.

## Features

- **10 Clicks Per Second**: Automatically clicks at the specified rate
- **Simple GUI**: Clean interface with Start/Stop controls
- **Click Counter**: Shows total number of clicks performed
- **Status Display**: Current operation status (Clicking/Stopped)
- **Thread-safe**: Uses atomic variables for thread safety

## Requirements

- Windows operating system
- wdevkit64 or compatible GCC-based compiler with Win32 API support
- MinGW-w64 or similar development environment

## Building

### Using wdevkit64/Makefile:
```bash
cd auto_clicker
make all
```

### Manual Build (wdevkit64):
```bash
g++ -Wall -O2 -mwindows main.cpp -o auto_clicker.exe -luser32 -lkernel32 -lwinmm
```

### Alternative with Resource File:
```bash
windres auto_clicker.rc -O coff -o auto_clicker.res
g++ -Wall -O2 -mwindows main.cpp auto_clicker.res -o auto_clicker.exe -luser32 -lkernel32 -lwinmm
```

## Usage

1. **Run the application**: Double-click `auto_clicker.exe`
2. **Start clicking**: Click the "Start Clicking" button
3. **Stop clicking**: Click the "Stop Clicking" button
4. **Monitor**: Watch the status and click count display

## How It Works

- The application uses a separate thread for the auto-clicking functionality
- Clicks are simulated using the `SendInput` Win32 API
- The clicking rate is controlled by sleeping for 100ms between clicks (10 clicks/second)
- The current mouse position is captured when clicking starts
- Thread-safe atomic variables prevent race conditions

## Controls

- **Start Button**: Begins auto-clicking at current mouse position
- **Stop Button**: Stops the auto-clicking process
- **Status Label**: Shows current operation state
- **Click Counter**: Displays total number of clicks performed
- **Info Label**: Shows the clicking rate (10 clicks/second)

## wdevkit64 Specific Notes

This application is optimized for building with **wdevkit64**, which provides GCC/MinGW toolchain for Windows development:

- Uses GCC compiler instead of MSVC
- Compatible with MinGW-w64 libraries
- Supports POSIX threads and C++11 features
- Uses `-mwindows` flag to create GUI application without console

## Safety Notes

- The application will automatically stop clicking if the window is closed
- Use responsibly and in accordance with application terms of service
- Not recommended for use in competitive gaming or applications that prohibit automation

## Files

- `main.cpp`: Main application source code with GUI and auto-clicking logic
- `Makefile`: Build configuration for wdevkit64/GCC compilation
- `auto_clicker.rc`: Resource file with version information
- `README.md`: This documentation file

## Compilation

The application requires the following libraries:
- `user32.lib`: For Windows GUI and input functions
- `kernel32.lib`: For system functions and threading
- `winmm.lib`: For multimedia/timing functions (optional)

## Troubleshooting

If you encounter build issues:
1. Ensure wdevkit64 is properly installed and in your PATH
2. Verify Win32 development headers are available
3. Check that MinGW-w64 libraries are installed
4. Try building with just the main.cpp file first