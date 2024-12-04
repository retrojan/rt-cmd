#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <stdexcept>
#include <array>
#include <fstream>
#include <windows.h>
#include <locale>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <tlhelp32.h>
#include <memory>
#include <codecvt>


#define TH32CS_PROCESS 0x00000002

std::vector<std::wstring> commandHistory; // Вектор для хранения истории команд
std::vector<std::wstring> deletedFiles; // Вектор для хранения удалённых файлов
std::wstring lastCommand; // Переменная для хранения последней выполненной команды
void setConsoleColor(int textColor) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);
}

void printRepositoryHeaders() {
    setConsoleColor(15); // Белый для текста
    std::cout << "Repository: "; // Выводим "Repository" в белом

    setConsoleColor(10); // Ярко-зелёный
    std::cout << "https://github.com/retrojan/cmd/releases"; // Выводим ссылку

    setConsoleColor(15); // Сброс цвета текста обратно на белый
    std::cout << std::endl; // Добавляем пустую строку
    std::cout << std::endl; // Выводит пустую строку и переводит курсор на следующую строку.
}

// Функция для преобразования строки из многобайтового формата в wide string
std::wstring toWideString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
    return wstr;
}

namespace fs = std::filesystem;
// Функция для преобразования строки из std::string в std::wstring
std::wstring stringToWstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

void executeGo(const std::string& path) {
    // Преобразование std::string в std::wstring
    std::wstring widePath = toWideString(path); // Используем вашу функцию преобразования

    std::wcout << L"Переход в директорию: " << widePath << std::endl; // Используем wcout для wide string
    if (SetCurrentDirectoryW(widePath.c_str())) { // Передаем wide string
        std::wcout << L"Успешный переход в " << widePath << std::endl; // Используем wcout для wide string
    }
    else {
        std::wcerr << L"Ошибка: не удалось перейти в " << widePath << std::endl; // Используем wcerr для wide string
    }
}

void runCommand(const std::string& command) {
    // Выполняем системную команду
    system(command.c_str());
}

void changeDirectory(const std::string& path) {
    try {
        fs::current_path(path);
        std::cout << "Текущая директория: " << fs::current_path() << std::endl;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void goBack() {
    fs::path currentPath = fs::current_path();
    fs::path parentPath = currentPath.parent_path();
    changeDirectory(parentPath.string());
}

void createFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "";  // Пустой файл
        file.close();
        std::cout << "Файл '" << filename << "' был создан." << std::endl;
    }
    else {
        std::cerr << "Ошибка: Не удалось создать файл." << std::endl;
    }
}

void deleteFile(const std::string& filename) {
    if (remove(filename.c_str()) == 0) {
        std::cout << "Файл '" << filename << "' был удалён." << std::endl;
    }
    else {
        std::cerr << "Ошибка: Файл '" << filename << "' не найден." << std::endl;
    }
}










void proccestab() {
    // Создаем снимок всех процессов
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_PROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка: не удалось создать снимок процессов." << std::endl;
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Получаем информацию о первом процессе
    if (Process32First(hProcessSnap, &pe32)) {
        std::cout << "Список запущенных процессов:" << std::endl;
        do {
            std::wcout << L"" << pe32.szExeFile << L" (PID: " << pe32.th32ProcessID << L")" << std::endl;
        } while (Process32Next(hProcessSnap, &pe32));
    }
    else {
        std::cerr << "Ошибка: не удалось получить информацию о процессах." << std::endl;
    }

    // Закрываем дескриптор снимка
    CloseHandle(hProcessSnap);
}






















void renameFile(const std::string& oldName, const std::string& newName) {
    if (rename(oldName.c_str(), newName.c_str()) == 0) {
        std::cout << "'" << oldName << "' переименован в '" << newName << "'." << std::endl;
    }
    else {
        std::cerr << "Ошибка: Не удалось переименовать файл." << std::endl;
    }
}

void showHelp() {
    std::cout << "Доступные команды:\n";
    std::cout << "  pip install <package> - Устанавливает пакет через pip (если pip доступен).\n";
    std::cout << "  cd <path> - Переход в указанную директорию.\n";
    std::cout << "  back - Переход на одну директорию назад.\n";
    std::cout << "  create <file> - Создает новый файл.\n";
    std::cout << "  delete <file> - Удаляет указанный файл.\n";
    std::cout << "  rename <old_name> <new_name> - Переименовывает файл.\n";
    std::cout << "  clr - Очищает экран.\n";
    std::cout << "  go <path> - Переход в указанную директорию.\n";
    std::cout << "  kill <process> - Завершает указанный процесс.\n";
    std::cout << "  dir - Показывает список файлов и папок в текущей директории.\n";
    std::cout << "  open <file> - Открывает указанный файл.\n";
    std::cout << "  info - Показывает информацию о системе.\n";
    std::cout << "  help - Показывает это сообщение.\n";
    std::cout << "  proccestab - Показывает список запущенных процессов.\n"; // Добавлено
}


void clearScreen() {
    system("cls");  // Очищаем консоль (для Windows)
}







void executeKill(const std::string& processName) {
    std::string command = "taskkill /IM " + processName + " /F";
    system(command.c_str());
    std::cout << "Завершение процесса: " << processName << std::endl;
}

void executeDir() {
    std::cout << "Содержимое текущей директории:" << std::endl;
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
        std::cout << entry.path().filename().string() << std::endl;
    }
}

void executeOpen(const std::string& fileName) {
    std::string command = "start " + fileName;
    system(command.c_str());
    std::cout << "Открытие файла: " << fileName << std::endl;
}

void executeInfo() {
    std::cout << "Информация о системе:" << std::endl;
    system("systeminfo");
}




// Функция для обработки команд
void handleCommand(const std::string& command) {
    if (command == "clr" || command == "cls") {
        system("cls"); // Очистка консоли
    }
    else {
        // Обработка неизвестной команды
        setConsoleColor(12); // Красный цвет для текста
        std::cout << "Неизвестная команда. Напиши help " << std::endl;
        setConsoleColor(15); // Сброс цвета текста обратно на белый
    }
}










int main() {
    // Установка кодировки UTF-8 для консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, "Russian");
    std::string command;
    printRepositoryHeaders();

    while (true) {
        std::cout << fs::current_path() << "> ";
        std::getline(std::cin, command);



        if (command.find("go ") == 0) {
                executeGo(command.substr(3));
            }
        else if (command.find("kill ") == 0) {
                executeKill(command.substr(5));
            }


        else if (command == "dir") {
                executeDir();
            }
        else if (command.find("open ") == 0) {
                executeOpen(command.substr(5));
            }
        else if (command == "info") {
                executeInfo();
            }
        else if (command == "exit") {
                break;
            }
        

        else if (command == "clr" || command == "cls") {

            clearScreen();
            printRepositoryHeaders(); // Снова выводим заголовок репозитория
        }
        else if (command.rfind("cd ", 0) == 0) {
            changeDirectory(command.substr(3));
        }
        else if (command == "back") {
            goBack();
        }
        else if (command.rfind("create ", 0) == 0) {
            createFile(command.substr(7));
        }
        else if (command.rfind("delete ", 0) == 0) {
            deleteFile(command.substr(7));
        }
        else if (command.rfind("rename ", 0) == 0) {
            size_t spacePos = command.find(' ', 7);
            if (spacePos != std::string::npos) {
                std::string oldName = command.substr(7, spacePos - 7);
                std::string newName = command.substr(spacePos + 1);
                renameFile(oldName, newName);
            }
            else {
                std::cerr << "Ошибка: неправильный формат команды." << std::endl;
            }
        }
        else if (command == "help") {
            showHelp();
        }
        else if (command == "proccestab") {
            proccestab();

        }

        else {
            handleCommand(command); // Обрабатываем введенную команду
        }
    }

    return 0;
}
