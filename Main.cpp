#include <windows.h>
#include <iostream>
#include <conio.h>

// --- Configuration Constants ---
const int SUPER_FAST_SPEED = 31; // Fastest possible repeat rate
const int SUPER_FAST_DELAY = 0;  // Shortest possible delay (Windows minimum)

const int STANDARD_DEFAULT_SPEED = 22; // Mid-high speed, a safe baseline
const int STANDARD_DEFAULT_DELAY = 1;  // Mid-range delay

// Global variables to store the true original system settings
DWORD g_originalSpeed;
int g_originalDelay;

// Function to set the keyboard speed and delay
void setKeyboardSettings(int speed, int delay) {
    SystemParametersInfo(SPI_SETKEYBOARDSPEED, speed, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
    SystemParametersInfo(SPI_SETKEYBOARDDELAY, delay, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

// Exit Handler (Now CLOSES but DOES NOT RESTORE settings, giving you full manual control)
BOOL WINAPI ExitHandler(DWORD fdwCtrlType) {
    if (fdwCtrlType == CTRL_CLOSE_EVENT || fdwCtrlType == CTRL_C_EVENT) {
        std::cout << "\n\n[WARNING]: Closing. Settings NOT restored. Current speed remains active.\n";
        return FALSE; // Allow system to terminate
    }
    return FALSE;
}

int main() {
    // 1. Save Original Settings to Global Variables
    SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &g_originalSpeed, 0);
    SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &g_originalDelay, 0);

    // 2. Register the Exit Handler (Safe closing)
    SetConsoleCtrlHandler(ExitHandler, TRUE);

    bool isFastMode = false;
    char key = ' ';

    std::cout << "========================================\n";
    std::cout << "    KEYBOARD RATE TOGGLE UTILITY\n";
    std::cout << "    (MANUAL RESTORE MODE)\n";
    std::cout << "========================================\n";
    std::cout << "Original System Settings Saved:\n";
    std::cout << "  - Speed: " << g_originalSpeed << ", Delay: " << g_originalDelay << "\n\n";

    std::cout << "CONTROLS:\n";
    std::cout << "  [S] - Toggle SUPER FAST Mode (Speed: 31, Delay: 0)\n";
    std::cout << "  [R] - Restore SAVED Original Settings\n";
    std::cout << "  [D] - Set STANDARD Default (Speed: " << STANDARD_DEFAULT_SPEED << ", Delay: " << STANDARD_DEFAULT_DELAY << ")\n";
    std::cout << "  [Q] - Quit Program (Leaves current settings active)\n";
    std::cout << "  [X] - eXit and Restore (Resets to original settings and quits)\n\n";

    while (true) {
        if (isFastMode) {
            std::cout << "\r[STATUS]: SUPER FAST ACTIVE  " << std::flush;
        }
        else {
            std::cout << "\r[STATUS]: Normal/Default     " << std::flush;
        }

        key = _getch();
        key = tolower(key);

        switch (key) {
        case 's':
            std::cout << "\n[ACTION]: Setting SUPER FAST...\n";
            setKeyboardSettings(SUPER_FAST_SPEED, SUPER_FAST_DELAY);
            isFastMode = true;
            break;

        case 'r':
            std::cout << "\n[ACTION]: Restoring Saved Original...\n";
            setKeyboardSettings(g_originalSpeed, g_originalDelay);
            isFastMode = false;
            break;

        case 'd':
            std::cout << "\n[ACTION]: Setting STANDARD Default...\n";
            setKeyboardSettings(STANDARD_DEFAULT_SPEED, STANDARD_DEFAULT_DELAY);
            isFastMode = false;
            break;

        case 'q':
            // Quit immediately, leaving current settings active
            std::cout << "\n\nQuitting (Settings left active)...\n";
            goto end_loop;

        case 'x': // <-- New explicit restore and exit option
            std::cout << "\n\n[ACTION]: Restoring original settings and exiting...\n";
            setKeyboardSettings(g_originalSpeed, g_originalDelay);
            goto end_loop;

        default:
            // Ignore other key presses
            break;
        }
    }

end_loop:
    return 0;
}