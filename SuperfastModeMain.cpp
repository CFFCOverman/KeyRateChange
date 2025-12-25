#include <windows.h>
#include <iostream>
#include <conio.h>

// --- THE "FIXED" STANDARD NUMBERS ---
const int FIXED_DEFAULT_SPEED = 20;
const int FIXED_DEFAULT_DELAY = 1;

// --- FAST & INSANE NUMBERS ---
const int SUPER_FAST_SPEED = 31;
const int INSANE_REPEAT_MS = 10;
const int INSANE_DELAY_MS = 150;

// Function to disable FilterKeys
void disableFilterKeys() {
    FILTERKEYS fk = { sizeof(FILTERKEYS), 0 };
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

// Function to set standard speeds
void setStandard(int speed, int delay) {
    disableFilterKeys();
    SystemParametersInfo(SPI_SETKEYBOARDSPEED, speed, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
    SystemParametersInfo(SPI_SETKEYBOARDDELAY, delay, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

// Function for Insane Mode
void setInsaneMode() {
    FILTERKEYS fk = { 0 };
    fk.cbSize = sizeof(FILTERKEYS);
    fk.dwFlags = FKF_FILTERKEYSON | FKF_AVAILABLE;
    fk.iWaitMSec = 0;
    fk.iDelayMSec = INSANE_DELAY_MS;
    fk.iRepeatMSec = INSANE_REPEAT_MS;
    fk.iBounceMSec = 0;
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

// --- SAFETY HANDLER ---
// This runs even if you click the 'X' button or press Ctrl+C
BOOL WINAPI ConsoleHandler(DWORD msgType) {
    if (msgType == CTRL_CLOSE_EVENT || msgType == CTRL_C_EVENT) {
        // When force closed, we assume you want your normal keyboard back
        setStandard(FIXED_DEFAULT_SPEED, FIXED_DEFAULT_DELAY);
        return FALSE;
    }
    return FALSE;
}

int main() {
    // Register the safety handler
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cout << "Error setting up safety handler!\n";
    }

    std::cout << "========================================\n";
    std::cout << "    KEYBOARD OVERDRIVE (FAIL-SAFE)\n";
    std::cout << "========================================\n";
    std::cout << "CONTROLS:\n";
    std::cout << "  [S] - SUPER FAST (30Hz)\n";
    std::cout << "  [I] - INSANE MODE (100Hz)\n";
    std::cout << "  [R] - RESTORE Standard (20 / 1)\n";
    std::cout << "  [Q] - QUIT (Keep current speed active)\n";
    std::cout << "  [X] - EXIT & RESTORE (Reset & Quit)\n\n";
    std::cout << "Note: Clicking [X] on the window will AUTO-RESTORE.\n\n";

    while (true) {
        char key = tolower(_getch());

        switch (key) {
        case 's':
            setStandard(SUPER_FAST_SPEED, 0);
            std::cout << "\r[STATUS]: SUPER FAST ACTIVE            " << std::flush;
            break;

        case 'i':
            setInsaneMode();
            std::cout << "\r[STATUS]: !!! INSANE MODE ACTIVE !!!   " << std::flush;
            break;

        case 'r':
            setStandard(FIXED_DEFAULT_SPEED, FIXED_DEFAULT_DELAY);
            std::cout << "\r[STATUS]: RESTORED TO STANDARD (20/1)  " << std::flush;
            break;

        case 'q':
            // We unregister the handler so it DOESN'T restore on exit
            SetConsoleCtrlHandler(ConsoleHandler, FALSE);
            std::cout << "\nQuitting... Speed remains active.\n";
            return 0;

        case 'x':
            setStandard(FIXED_DEFAULT_SPEED, FIXED_DEFAULT_DELAY);
            std::cout << "\nRestored and Exiting...\n";
            return 0;
        }
    }
    return 0;
}