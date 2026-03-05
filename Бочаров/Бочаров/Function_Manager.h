#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <clocale>
using namespace std;

class FileManager {
public:
    void list(string p) {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA((p + "\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) { cout << "Путь не найден!\n"; return; }

        cout << left << setw(10) << "\n ТИП" << " | " << "ИМЯ" << endl;
        cout << "------------------------------------------\n";
        do {
            string type = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "[ПАПКА]" : "[ФАЙЛ]";
            cout << left << setw(10) << type << " | " << fd.cFileName << endl;
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
        cout << "Удаление выполнено.\n";
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