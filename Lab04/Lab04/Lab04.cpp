#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

void PrintRegistryEntries(HKEY root, LPCSTR subkey) {
    HKEY hKey;
    if (RegOpenKeyExA(root, subkey, 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
        CHAR valueName[256], valueData[MAX_PATH];
        DWORD valueNameSize, valueDataSize, valueType;
        DWORD index = 0;

        std::cout << "\n[" << (root == HKEY_LOCAL_MACHINE ? "HKLM" : "HKCU") << "\\" << subkey << "]\n";

        while (true) {
            valueNameSize = sizeof(valueName);
            valueDataSize = sizeof(valueData);
            LONG result = RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &valueType, (LPBYTE)valueData, &valueDataSize);
            if (result == ERROR_NO_MORE_ITEMS) break;

            if (result == ERROR_SUCCESS && valueType == REG_SZ)
                std::cout << valueName << " => " << valueData << "\n";

            index++;
        }

        RegCloseKey(hKey);
    }
    else {
        std::cerr << "Failed to open registry key.\n";
    }
}

void AddToStartup() {
    std::cout << "\n=== ADD TO STARTUP ===\n";
    HKEY hKey;
    LPCSTR path = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        LPCSTR appName = "MyWinWord";
        LPCSTR appPath = "\"C:\\Program Files\\Microsoft Office\\root\\Office16\\WINWORD.EXE\""; // Change path as needed
        if (RegSetValueExA(hKey, appName, 0, REG_SZ, (const BYTE*)appPath, strlen(appPath) + 1) == ERROR_SUCCESS) {
            std::cout << "WinWord added to startup successfully.\n";
        }
        else {
            std::cerr << "Error writing to registry.\n";
        }
        RegCloseKey(hKey);
    }
    else {
        std::cerr << "Failed to open registry key.\n";
    }
}

void ExportRegistry() {
    std::cout << "\n=== EXPORT REGISTRY ===\n";
    system("reg export \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\" startup_export.reg /y");
    std::cout << "Exported to startup_export.reg\n";
}

void ShowTaskScheduler() {
    std::cout << "\n=== SCHEDULED TASKS ===\n";

    std::cout << "Active scheduled tasks:\n";
    system("schtasks /query /fo TABLE");
}

void CreateAndImportTTTRegFile() {
    std::cout << "\n=== CREATE .TTT FILE ASSOCIATION ===\n";
    std::ofstream regFile("ttt_association.reg");
    if (!regFile) {
        std::cerr << "Failed to create file.\n";
        return;
    }

    regFile << "Windows Registry Editor Version 5.00\n\n"
        << "[HKEY_CLASSES_ROOT\\.ttt]\n"
        << "@=\"tttfile\"\n\n"
        << "[HKEY_CLASSES_ROOT\\tttfile]\n"
        << "@=\"TTT File\"\n\n"
        << "[HKEY_CLASSES_ROOT\\tttfile\\shell\\open\\command]\n"
        << "@=\"\\\"C:\\\\Windows\\\\system32\\\\notepad.exe\\\" \\\"%1\\\"\"\n";
    regFile.close();

    std::cout << "File ttt_association.reg created.\n";

    int result = system("reg import ttt_association.reg");
    if (result == 0) {
        std::cout << ".ttt file extension associated with Notepad successfully.\n";
    }
    else {
        std::cout << "Registry import failed - Administrator privileges required.\n";
        std::cout << "To complete the association:\n";
        std::cout << "1. Run this program as Administrator, OR\n";
        std::cout << "2. Right-click 'ttt_association.reg' and select 'Merge', OR\n";
        std::cout << "3. Double-click 'ttt_association.reg' and confirm the UAC prompt\n";
    }
}

void Menu() {
    int choice;
    do {
        std::cout << "\n====== MENU ======\n";
        std::cout << "1. Show startup programs\n";
        std::cout << "2. Add WinWord to startup\n";
        std::cout << "3. Show registered tasks (Task Scheduler)\n";
        std::cout << "4. Export startup registry to .reg file\n";
        std::cout << "5. Create and import .ttt -> Notepad .reg file\n";
        std::cout << "0. Exit\n";
        std::cout << "Your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            std::cout << "\n=== STARTUP PROGRAMS ===\n";
            PrintRegistryEntries(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run");
            PrintRegistryEntries(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run");
            break;
        case 2:
            AddToStartup();
            break;
        case 3:
            ShowTaskScheduler();
            break;
        case 4:
            ExportRegistry();
            break;
        case 5:
            CreateAndImportTTTRegFile();
            break;
        case 0:
            std::cout << "\n=== EXITING ===\n";
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "\n=== ERROR ===\n";
            std::cout << "Unknown option.\n";
        }
    } while (choice != 0);
}

int main() {
    Menu();
    return 0;
}