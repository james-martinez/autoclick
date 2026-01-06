#include <windows.h>
#include <winuser.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// Global variables
std::atomic<bool> isClicking(false);
std::atomic<int> clickCount(0);
int clicksPerSecond = 100;
POINT lastMousePos;
HWND rateEdit; // Edit control for rate input
HWND applyButton; // Button to apply rate changes
const int TOGGLE_HOTKEY_ID = 1; // ID for the toggle hotkey
HWND keyCombo; // ComboBox for key selection
int keyToHoldVK = 0; // Virtual Key code to hold (0 for none)

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Helper function to simulate key press/release
void SimulateKey(int vk, bool down) {
    if (vk == 0) return;
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

// Auto-clicker thread function
void AutoClickerThread() {
    while (isClicking) {
        // Simulate mouse click at current position
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));

        clickCount++;

        // Sleep for the desired click rate
        if (clicksPerSecond > 0) {
            int sleepTime = 1000 / clicksPerSecond; // Convert to milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }
}

// Function to start clicking
void StartClicking(HWND hwnd) {
    if (!isClicking) {
        isClicking = true;
        clickCount = 0;

        // Get current mouse position
        GetCursorPos(&lastMousePos);

        // Start the auto-clicker thread
        std::thread clicker(AutoClickerThread);
        clicker.detach();

        // Update status
        SetWindowText(GetDlgItem(hwnd, 3), "Status: Clicking...");

        // Disable start button, enable stop button
        EnableWindow(GetDlgItem(hwnd, 1), FALSE);
        EnableWindow(GetDlgItem(hwnd, 2), TRUE);

        // Disable key combo
        EnableWindow(keyCombo, FALSE);

        // Press the key
        SimulateKey(keyToHoldVK, true);
    }
}

// Function to stop clicking
void StopClicking(HWND hwnd) {
    if (isClicking) {
        isClicking = false;
        SetWindowText(GetDlgItem(hwnd, 3), "Status: Stopped");

        // Enable start button, disable stop button
        EnableWindow(GetDlgItem(hwnd, 1), TRUE);
        EnableWindow(GetDlgItem(hwnd, 2), FALSE);

        // Enable key combo
        EnableWindow(keyCombo, TRUE);

        // Release the key
        SimulateKey(keyToHoldVK, false);
    }
}

// Function to update the rate label
void UpdateRateLabel(HWND hwnd) {
    char rateText[100];
    snprintf(rateText, sizeof(rateText), "Rate: %d clicks/second", clicksPerSecond);
    SetWindowText(GetDlgItem(hwnd, 5), rateText);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char CLASS_NAME[] = "AutoClickerWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Auto Clicker - Adjustable Rate with Hotkey",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 400, // Increased width and height
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    // Register global hotkey (ESC key)
    if (!RegisterHotKey(hwnd, TOGGLE_HOTKEY_ID, 0, VK_ESCAPE)) {
        MessageBox(hwnd, "Could not register hotkey. It may already be in use.", "Hotkey Warning", MB_OK | MB_ICONWARNING);
    }

    // Create controls
    CreateWindow("BUTTON", "Start Clicking", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        50, 50, 120, 30, hwnd, (HMENU)1, hInstance, NULL);

    CreateWindow("BUTTON", "Stop Clicking", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        50, 90, 120, 30, hwnd, (HMENU)2, hInstance, NULL);

    CreateWindow("STATIC", "Status: Stopped", WS_VISIBLE | WS_CHILD,
        50, 140, 200, 20, hwnd, (HMENU)3, hInstance, NULL);

    CreateWindow("STATIC", "Clicks: 0", WS_VISIBLE | WS_CHILD,
        50, 170, 200, 20, hwnd, (HMENU)4, hInstance, NULL);

    CreateWindow("STATIC", "Rate: 100 clicks/second", WS_VISIBLE | WS_CHILD,
        50, 200, 200, 20, hwnd, (HMENU)5, hInstance, NULL);

    // Rate input controls
    CreateWindow("STATIC", "Clicks/second:", WS_VISIBLE | WS_CHILD,
        50, 230, 100, 20, hwnd, (HMENU)6, hInstance, NULL);

    rateEdit = CreateWindow("EDIT", "100", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        160, 230, 80, 20, hwnd, (HMENU)7, hInstance, NULL);

    applyButton = CreateWindow("BUTTON", "Apply Rate", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        250, 230, 80, 20, hwnd, (HMENU)8, hInstance, NULL);

    // Hotkey display
    CreateWindow("STATIC", "Press ESC to toggle clicking (global hotkey)", WS_VISIBLE | WS_CHILD,
        50, 280, 350, 20, hwnd, (HMENU)9, hInstance, NULL);

    // Key hold controls
    CreateWindow("STATIC", "Hold Key:", WS_VISIBLE | WS_CHILD,
        50, 310, 80, 20, hwnd, (HMENU)10, hInstance, NULL);

    keyCombo = CreateWindow("COMBOBOX", "",
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        140, 310, 80, 200, hwnd, (HMENU)11, hInstance, NULL);

    // Add items to combo box
    SendMessage(keyCombo, CB_ADDSTRING, 0, (LPARAM)"None");
    SendMessage(keyCombo, CB_ADDSTRING, 0, (LPARAM)"W");
    SendMessage(keyCombo, CB_ADDSTRING, 0, (LPARAM)"A");
    SendMessage(keyCombo, CB_ADDSTRING, 0, (LPARAM)"S");
    SendMessage(keyCombo, CB_ADDSTRING, 0, (LPARAM)"D");

    // Select "None" by default
    SendMessage(keyCombo, CB_SETCURSEL, 0, 0);

    ShowWindow(hwnd, nCmdShow);

    // Set up a timer to update the click count display every second
    SetTimer(hwnd, 1, 1000, NULL);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unregister the hotkey when exiting
    UnregisterHotKey(hwnd, TOGGLE_HOTKEY_ID);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_HOTKEY:
        if (wParam == TOGGLE_HOTKEY_ID) {
            // Toggle clicking on/off
            if (isClicking) {
                StopClicking(hwnd);
            } else {
                StartClicking(hwnd);
            }
        }
        return 0;

    case WM_DESTROY:
        if (isClicking) {
             StopClicking(hwnd);
        }
        isClicking = false;  // Stop clicking if window is closed
        PostQuitMessage(0);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: // Start button clicked
            StartClicking(hwnd);
            break;

        case 2: // Stop button clicked
            StopClicking(hwnd);
            break;

        case 8: // Apply rate button clicked
            {
                char rateBuffer[10];
                GetWindowText(rateEdit, rateBuffer, sizeof(rateBuffer));

                // Parse and validate the rate
                int newRate = atoi(rateBuffer);
                if (newRate > 0 && newRate <= 1000) { // Reasonable limits
                    clicksPerSecond = newRate;
                    UpdateRateLabel(hwnd);
                }
            }
            break;
        case 11: // Key combo box changed
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                int index = SendMessage(keyCombo, CB_GETCURSEL, 0, 0);
                switch (index) {
                    case 0: keyToHoldVK = 0; break;
                    case 1: keyToHoldVK = 'W'; break;
                    case 2: keyToHoldVK = 'A'; break;
                    case 3: keyToHoldVK = 'S'; break;
                    case 4: keyToHoldVK = 'D'; break;
                }
            }
            break;
        }
        return 0;

    case WM_TIMER:
        // Update click count display every second
        if (wParam == 1) {
            char countText[50];
            snprintf(countText, sizeof(countText), "Clicks: %d", clickCount.load());
            SetWindowText(GetDlgItem(hwnd, 4), countText);
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}