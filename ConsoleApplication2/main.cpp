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
#include <cstdlib> // Для system()
#include <algorithm> // Для std::transform
#include <cctype>    // Для std::tolower

#define TH32CS_PROCESS 0x00000002

std::vector<std::wstring> commandHistory; // Вектор для хранения истории команд
std::vector<std::wstring> deletedFiles; // Вектор для хранения удалённых файлов
std::wstring lastCommand; // Переменная для хранения последней выполненной команды
void setConsoleColor(int textColor) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);
}

// Функция для преобразования строки в нижний регистр
std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}






void printRepositoryHeaders() {
    setConsoleColor(15); // Белый для текста
    std::cout << "Repository: "; // Выводим "Repository" в белом

    setConsoleColor(10); // Ярко-зелёный
    std::cout << "https://github.com/retrojan/rt-cmd/"; // Выводим ссылку

    setConsoleColor(15); // Сброс цвета текста обратно на белый
    std::cout << " [version: 2.13] ";
    std::cout << std::endl; // Добавляем пустую строку
    std::cout << std::endl; // Выводит пустую строку и переводит курсор на следующую строку.
}
std::string downloadedVersion = "2.13"; // Версия, которую скачал пользователь

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
    setConsoleColor(2);
    std::cout << "Доступные команды:" << std::endl;
    setConsoleColor(15);
    std::cout << "  dir         - Показать содержимое текущей директории." << std::endl;
    std::cout << "  go <path>   - Перейти в указанную директорию." << std::endl;
    std::cout << "  kill <name> - Завершить процесс по имени." << std::endl;
    std::cout << "  open <path> - Открыть указанный файл или директорию." << std::endl;
    std::cout << "  info        - Показать информацию о системе." << std::endl;
    std::cout << "  exit        - Выход из программы." << std::endl;
    std::cout << "  taskmgr     - Открыть диспетчер задач." << std::endl;
    std::cout << "  msconfig    - Открыть конфигурацию системы." << std::endl;
    std::cout << "  appdata     - Открыть папку AppData." << std::endl;
    std::cout << "  local       - Открыть папку LocalAppData." << std::endl;
    std::cout << "  roaming     - Открыть папку Roaming AppData." << std::endl;
    std::cout << "  temp        - Открыть папку Temp." << std::endl;
    std::cout << "  clr/cls     - Очистить экран консоли." << std::endl;
    std::cout << "  cd <path>   - Перейти в указанную директорию." << std::endl;
    std::cout << "  back        - Вернуться на уровень вверх в файловой системе." << std::endl;
    std::cout << "  create <file> - Создать новый файл." << std::endl;
    std::cout << "  delete <file> - Удалить указанный файл." << std::endl;
    std::cout << "  rename <old_name> <new_name> - Переименовать файл." << std::endl;
    std::cout << "  help        - Показать это сообщение." << std::endl;
    std::cout << "  processtab  - Показать таблицу процессов." << std::endl;
    std::cout << "  settings     - Открыть параметры Windows." << std::endl;
    std::cout << "  control     - Открыть панель управления." << std::endl;
    std::cout << "  locallow    - Открыть папку локального кеша." << std::endl;
    std::cout << "  windows     - Открыть папку Windows." << std::endl;
    std::cout << "  system32    - Открыть папку System32." << std::endl;
    std::cout << "  connect <adress> - Проверка подключения к ресурсу." << std::endl;
    std::cout << "  runas       - Запуск консоли от имени администратора." << std::endl;
}



void clearScreen() {
    system("cls");  // Очищаем консоль (для Windows)
}




void createDirectory(const std::string& dirName) {
    if (fs::create_directory(dirName)) {
        std::cout << "Папка '" << dirName << "' успешно создана." << std::endl;
    }
    else {
        std::cerr << "Ошибка: не удалось создать папку '" << dirName << "'." << std::endl;
    }
}


void executeKill(const std::string& processName) {
    std::string command = "taskkill /IM " + processName + " /F";
    system(command.c_str());
    std::cout << "Завершение процесса: " << processName << std::endl;
}

void executeDir() {
    try {
        std::vector<std::pair<std::string, uintmax_t>> files;

        uintmax_t totalSize = 0;

        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            if (entry.is_regular_file()) {
                uintmax_t fileSize = entry.file_size();
                totalSize += fileSize;
                files.push_back({ entry.path().filename().string(), fileSize });
            }
        }

        // Выводим список файлов с их размерами
        std::cout << "Содержимое директории:" << std::endl;
        for (const auto& [fileName, fileSize] : files) {
            std::cout << "  " << fileName << " (" << fileSize << " байт)" << std::endl;
        }

        std::cout << "Общий размер всех файлов: " << totalSize << " байт" << std::endl;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
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
// Функция для установки цвета текста в консоли


















// Функция для запуска программы от имени администратора
void runAsAdmin(const std::string& command) {
    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas";  // Запуск от имени администратора
    sei.nShow = SW_SHOWNORMAL;  // Показывать окно

    // Получаем путь к текущему исполняемому файлу
    wchar_t filePath[MAX_PATH];
    GetModuleFileName(NULL, filePath, MAX_PATH);

    sei.lpFile = filePath;  // Указываем исполняемый файл

    if (!command.empty()) {
        // Если есть команда, то передаём её как аргументы
        std::wstring wCommand(command.begin(), command.end());
        sei.lpParameters = wCommand.c_str();  // Аргументы для запуска
    }

    // Запускаем консоль от имени администратора
    if (!ShellExecuteEx(&sei)) {
        std::cerr << "Ошибка: не удалось запустить программу от имени администратора." << std::endl;
    }
    else {
        std::cout << "Консоль запущена от имени администратора." << std::endl;
        // Закрываем текущую консоль, если новая успешно запущена
        ExitProcess(0);
    }
}










void executePing(const std::string& address) {
    std::string command = "ping " + address + " -n 4"; // Отправляем 4 пакета
    // Выполняем команду и перенаправляем вывод в временный файл
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        setConsoleColor(12); // Красный цвет для ошибки
        std::cerr << "Не удалось выполнить команду ping." << std::endl;
        setConsoleColor(7); // Сброс цвета к стандартному
        return;
    }

    char buffer[128];
    std::string result = "";

    // Чтение вывода команды
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int returnCode = _pclose(pipe); // Закрываем поток

    // Проверяем наличие ответа и выводим результат
    if (result.find("Reply from") != std::string::npos) {
        // Определяем количество успешно отправленных и потерянных пакетов
        int successfulPackets = 0;
        int lostPackets = 0;

        std::istringstream iss(result);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("Reply from") != std::string::npos) {
                successfulPackets++;
            }
            else if (line.find("Request timed out") != std::string::npos) {
                lostPackets++;
            }
        }

        // Выводим результаты
        if (successfulPackets > 0) {
            setConsoleColor(10); // Зеленый цвет для успешного пакета
            std::cout << "Успешно отправлено пакетов: " << successfulPackets << std::endl;
            setConsoleColor(7); // Сброс цвета к стандартному
            setConsoleColor(10); // Зеленый цвет для корректного подключения
            std::cout << "Подключение к " << address << " ";
            std::cout << "корректное." << std::endl; // Выводим слово "корректное" зеленым
        }
        else {
            setConsoleColor(12); // Красный цвет для ошибки
            std::cout << "Не удалось отправить ни одного пакета." << std::endl;
            setConsoleColor(12); // Красный цвет для некорректного подключения
            std::cout << "Подключение к " << address << " ";
            std::cout << "некорректное." << std::endl; // Выводим слово "некорректное" красным
        }
    }
    else {
        setConsoleColor(12); // Красный цвет для ошибки
        std::cout << "Не удалось отправить пакет." << std::endl;
        setConsoleColor(12); // Красный цвет для некорректного подключения
        std::cout << "Подключение к " << address << " некорректное." << std::endl; // Сообщение о некорректном подключении
    }

    setConsoleColor(7); // Сброс цвета к стандартному
}











int main() {

    // Установка кодировки UTF-8 для консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, "Russian");
    std::string command;
    printRepositoryHeaders();
    // Устанавливаем заголовок окна консоли
    SetConsoleTitle(TEXT("RT-CMD by ReTrojan"));
    std::istringstream iss(command); // Инициализация iss здесь
    while (true) {
        std::cout << fs::current_path() << "> ";

        std::getline(std::cin, command); // Чтение ввода от пользователя

        std::string commandLower = toLower(command); // Преобразуем команду в нижний регистр

        if (commandLower.find("go ") == 0) {
            executeGo(command.substr(3));
        }
        else if (commandLower.find("kill ") == 0) {
            executeKill(command.substr(5));
        }
        else if (commandLower == "dir") {
            executeDir();
        }
        else if (commandLower.find("open ") == 0) {
            executeOpen(command.substr(5));
        }
        else if (commandLower == "info") {
            executeInfo();
        }
        else if (commandLower == "exit") {
            break; // Выход из цикла
        }
        else if (commandLower.rfind("mkdir ", 0) == 0) { // Проверяем команду mkdir
            createDirectory(command.substr(6)); // Создаем директорию с именем, переданным после команды
        }





        else if (commandLower == "taskmgr") {
            system("taskmgr");
        }
        else if (commandLower == "msconfig") {
            system("msconfig");
        }
        else if (commandLower == "mscfg") {
            system("msconfig");
        }
        else if (commandLower == "appdata") {
            system("explorer %APPDATA%");
        }
        else if (commandLower == "local") {
            system("explorer %LOCALAPPDATA%");
        }
        else if (commandLower == "roaming") {
            system("explorer %APPDATA%");
        }
        else if (commandLower == "temp") {
            system("explorer %TEMP%");
        }

        else if (commandLower == "runas") {  // Обработка команды runas без пробела
            runAsAdmin("");  // Запуск консоли от имени администратора
        }



        else if (commandLower == "clr" || commandLower == "cls") {
            clearScreen();
            printRepositoryHeaders(); // Снова выводим заголовок репозитория
        }
        else if (commandLower.rfind("cd ", 0) == 0) {
            changeDirectory(command.substr(3));
        }
        else if (commandLower == "back") {
            goBack();
        }
        else if (commandLower.rfind("create ", 0) == 0) {
            createFile(command.substr(7));
        }
        else if (commandLower.rfind("delete ", 0) == 0) {
            deleteFile(command.substr(7));
        }
        else if (commandLower == "locallow") {
            system("explorer %LOCALAPPDATA%\\Microsoft\\Windows\\INetCache");
        }
        else if (commandLower == "windows") {
            system("explorer C:\\Windows");
        }

        else if (commandLower.find("connect ") == 0) {
            std::string address = command.substr(8); // Получаем адрес из пользовательского ввода
            executePing(address); // Выполняем ping
        }







        else if (commandLower == "system32") {
            system("explorer C:\\Windows\\system32");
        }
        else if (commandLower == "settings") {
            system("start ms-settings:"); // Открытие Параметров Windows
        }
        else if (commandLower == "control") {
            system("control"); // Открытие Панели управления
        }
        else if (commandLower.rfind("rename ", 0) == 0) {
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
        else if (commandLower == "help") {
            showHelp();
        }
        else if (commandLower == "processtab") {
            proccestab();
        }
        else {
            // Обработка неизвестной команды
            setConsoleColor(12); // Красный цвет для текста
            std::cout << "Неизвестная команда. Напиши help " << std::endl;
            setConsoleColor(15); // Сброс цвета текста обратно на белый
        }
    }

    return 0;
}