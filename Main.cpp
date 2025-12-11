#include <windows.h>
#include <iostream>
#include <conio.h> // Used for _getch() to detect key presses

// Function to set the keyboard speed and delay
void setKeyboardSettings(int speed, int delay) {
    // Set Repeat Speed (0 to 31, where 31 is fastest)
    SystemParametersInfo(SPI_SETKEYBOARDSPEED, speed, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);

    // Set Repeat Delay (0 to 3, where 0 is shortest ~250ms)
    SystemParametersInfo(SPI_SETKEYBOARDDELAY, delay, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

int main() {
    // Variables to store the original system settings
    DWORD originalSpeed;
    int originalDelay; // SPI_GETKEYBOARDDELAY uses an int usually, but API expects PVPARAM

    // 1. Retrieve current settings so we can restore them later
    SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &originalSpeed, 0);
    SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &originalDelay, 0);

    bool isFastMode = false;
    char key = ' ';

    std::cout << "========================================\n";
    std::cout << "    KEYBOARD RATE TOGGLE UTILITY\n";
    std::cout << "========================================\n";
    std::cout << "Current System Settings:\n";
    std::cout << "  - Speed: " << originalSpeed << " (Range: 0-31)\n";
    std::cout << "  - Delay: " << originalDelay << " (Range: 0-3)\n\n";

    std::cout << "CONTROLS:\n";
    std::cout << "  [F] - Toggle FAST Mode (Speed: 31, Delay: 0)\n";
    std::cout << "  [R] - Restore ORIGINAL Settings\n";
    std::cout << "  [Q] - Quit Program\n\n";

    while (true) {
        if (isFastMode) {
            std::cout << "\r[STATUS]: FAST MODE ACTIVE   " << std::flush;
        }
        else {
            std::cout << "\r[STATUS]: Normal Mode        " << std::flush;
        }

        // Wait for a key press (no busy waiting)
        key = _getch();

        // Convert to lowercase for easy handling
        key = tolower(key);

        if (key == 'f') {
            // MAX Speed (31) and MIN Delay (0)
            setKeyboardSettings(31, 0);
            isFastMode = true;
        }
        else if (key == 'r') {
            // Restore original saved values
            setKeyboardSettings(originalSpeed, originalDelay);
            isFastMode = false;
        }
        else if (key == 'q') {
            // It is good practice to restore defaults before quitting
            setKeyboardSettings(originalSpeed, originalDelay);
            std::cout << "\n\nRestoring original settings and quitting...\n";
            break;
        }
    }

    return 0;
}