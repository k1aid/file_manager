#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <clocale>

using namespace std;

// --- Базовый ООП класс ---
class FileSystemItem {
protected:
    string path;
public:
    FileSystemItem(string p) : path(p) {}
    virtual ~FileSystemItem() = default;
    virtual unsigned long long getSize() const = 0;
};

class FileItem : public FileSystemItem {
public:
    using FileSystemItem::FileSystemItem;
    unsigned long long getSize() const override {
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fad)) {
            LARGE_INTEGER size;
            size.LowPart = fad.nFileSizeLow;
            size.HighPart = fad.nFileSizeHigh;
            return size.QuadPart;
        }
        return 0;
    }
};

class DirectoryItem : public FileSystemItem {
public:
    using FileSystemItem::FileSystemItem;
    unsigned long long getSize() const override {
        return calculateSize(path);
    }
private:
    unsigned long long calculateSize(string folderPath) const {
        unsigned long long totalSize = 0;
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA((folderPath + "\\*").c_str(), &findData);
        if (hFind == INVALID_HANDLE_VALUE) return 0;
        do {
            string fileName = findData.cFileName;
            if (fileName != "." && fileName != "..") {
                string fullPath = folderPath + "\\" + fileName;
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    totalSize += calculateSize(fullPath);
                else {
                    LARGE_INTEGER size;
                    size.LowPart = findData.nFileSizeLow;
                    size.HighPart = findData.nFileSizeHigh;
                    totalSize += size.QuadPart;
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
        return totalSize;
    }
};

// --- Менеджер функций ---
class FileManager {
public:
    void list(string p) {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA((p + "\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) { cout << "Путь не найден!\n"; return; }
        cout << left << setw(10) << "\nТИП" << " | " << "ИМЯ" << endl;
        cout << "------------------------------------------\n";
        do {
            cout << left << setw(10) << ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "[ПАПКА]" : "[ФАЙЛ]")
                << " | " << fd.cFileName << endl;
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    void makeDir(string p) { if (CreateDirectoryA(p.c_str(), NULL)) cout << "Папка создана.\n"; }

    void makeFile(string p) {
        HANDLE h = CreateFileA(p.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); cout << "Файл создан.\n"; }
    }

    void remove(string p) {
        DWORD attr = GetFileAttributesA(p.c_str());
        if (attr & FILE_ATTRIBUTE_DIRECTORY) RemoveDirectoryA(p.c_str());
        else DeleteFileA(p.c_str());
        cout << "Выполнено удаление (если объект существовал).\n";
    }

    void search(string root, string mask) {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA((root + "\\" + mask).c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do { if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) cout << "Найдено: " << root << "\\" << fd.cFileName << endl; } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
        h = FindFirstFileA((root + "\\*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0)
                    search(root + "\\" + fd.cFileName, mask);
            } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
    }
};

// --- Интерфейс ---
void showMenu() {
    cout << "\n===== ФАЙЛОВЫЙ МЕНЕДЖЕР =====" << endl;
    cout << "1. Показать содержимое папки" << endl;
    cout << "2. Создать папку" << endl;
    cout << "3. Создать пустой файл" << endl;
    cout << "4. Удалить файл/папку" << endl;
    cout << "5. Переименовать/Переместить" << endl;
    cout << "6. Копировать файл" << endl;
    cout << "7. Найти по маске (везде)" << endl;
    cout << "8. Узнать размер объекта" << endl;
    cout << "0. Выход" << endl;
    cout << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    FileManager fm;
    int choice;
    string path1, path2;

    while (true) {
        showMenu();
        cin >> choice;
        cin.ignore(); // Очистка буфера после cin >>

        if (choice == 0) break;

        switch (choice) {
        case 1:
            cout << "Введите путь (например, . или C:\\): "; getline(cin, path1);
            fm.list(path1);
            break;
        case 2:
            cout << "Путь и имя новой папки: "; getline(cin, path1);
            fm.makeDir(path1);
            break;
        case 3:
            cout << "Путь и имя нового файла: "; getline(cin, path1);
            fm.makeFile(path1);
            break;
        case 4:
            cout << "Что удалить?: "; getline(cin, path1);
            fm.remove(path1);
            break;
        case 5:
            cout << "Старый путь: "; getline(cin, path1);
            cout << "Новый путь: "; getline(cin, path2);
            MoveFileA(path1.c_str(), path2.c_str());
            break;
        case 6:
            cout << "Что копируем: "; getline(cin, path1);
            cout << "Куда копируем: "; getline(cin, path2);
            CopyFileA(path1.c_str(), path2.c_str(), FALSE);
            break;
        case 7:
            cout << "Где искать (папка): "; getline(cin, path1);
            cout << "Маска (напр. *.cpp): "; getline(cin, path2);
            fm.search(path1, path2);
            break;
        case 8:
        {
            cout << "Путь к объекту: "; getline(cin, path1);
            DWORD attr = GetFileAttributesA(path1.c_str());
            if (attr & FILE_ATTRIBUTE_DIRECTORY) {
                DirectoryItem d(path1);
                cout << "Размер папки рекурсивно: " << d.getSize() << " байт.\n";
            }
            else {
                FileItem f(path1);
                cout << "Размер файла: " << f.getSize() << " байт.\n";
            }
            break;
        }
        default:
            cout << "Неверный выбор!\n";
        }
    }
    return 0;
}