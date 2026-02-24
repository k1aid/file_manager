#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <clocale>

using namespace std;

class FileSystemItem
{
protected:
    string path;
public:
    FileSystemItem(string p) : path(p) {}
    virtual ~FileSystemItem() = default;
    virtual unsigned long long getSize() const = 0;
};

class FileItem : public FileSystemItem
{
public:
    using FileSystemItem::FileSystemItem;
    unsigned long long getSize() const override
    {
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fad))
        {
            LARGE_INTEGER size;
            size.LowPart = fad.nFileSizeLow;
            size.HighPart = fad.nFileSizeHigh;
            return size.QuadPart;
        }
        return 0;
    }
};

class DirectoryItem : public FileSystemItem
{
public:
    using FileSystemItem::FileSystemItem;
    unsigned long long getSize() const override
    {
        return calculateSize(path);
    }
private:
    unsigned long long calculateSize(string folderPath) const
    {
        unsigned long long totalSize = 0;
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA((folderPath + "\\*").c_str(), &findData);
        if (hFind == INVALID_HANDLE_VALUE) return 0;
        do
        {
            string fileName = findData.cFileName;
            if (fileName != "." && fileName != "..")
            {
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

//менеджер функций
class FileManager
{
public:
    void list(string p)
    {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA((p + "\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) { cout << "Путь не найден!\n"; return; }
        cout << left << setw(10) << "\nТИП" << " | " << "ИМЯ" << endl;
        cout << "------------------------------------------\n";
        do
        {
            cout << left << setw(10) << ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "[ПАПКА]" : "[ФАЙЛ]")
                << " | " << fd.cFileName << endl;
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    void makeDir(string p) { if (CreateDirectoryA(p.c_str(), NULL)) cout << "Папка создана.\n"; }

    void makeFile(string p)
    {
        HANDLE h = CreateFileA(p.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); cout << "Файл создан.\n"; }
    }

    void remove(string p)
    {
        DWORD attr = GetFileAttributesA(p.c_str());
        if (attr & FILE_ATTRIBUTE_DIRECTORY) RemoveDirectoryA(p.c_str());
        else DeleteFileA(p.c_str());
        cout << "Выполнено удаление (если объект существовал).\n";
    }

    void search(string root, string mask)
    {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA((root + "\\" + mask).c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE)
        {
            do { if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) cout << "Найдено: " << root << "\\" << fd.cFileName << endl; } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
        h = FindFirstFileA((root + "\\*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0)
                    search(root + "\\" + fd.cFileName, mask);
            } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
    }
};

void showMenu()
{
    cout << "\n---- ФАЙЛОВЫЙ МЕНЕДЖЕР ----" << endl << "1. Показать содержимое папки" << endl << "2. Создать папку" << endl << "3. Создать пустой файл" << endl << "4. Удалить файл/папку" << endl << "5. Переименовать/Переместить" << endl << "6. Копировать файл" << endl << "7. Найти по маске (везде)" << endl << "8. Узнать размер объекта" << endl << "0. Выход" << endl << "Выберите действие: " << endl;
}

// Авторизациия
int prox = 0;
class Password
{
public:

    int Entrance()
    {
        string login = "login.txt";
        string password = "password.txt";

        fstream log;
        fstream pass;

        log.open(login, fstream::in | fstream::out | fstream::app);
        pass.open(password, fstream::in | fstream::out | fstream::app);

        if (!log.is_open() && !pass.is_open())
        {
            cout << "Ошибка!" << endl;
        }
        else
        {
            string log_cheker;
            string pass_cheker;

            while (!log.eof())
            {
                log_cheker = "123";
                log >> log_cheker;
            }
            while (!pass.eof())
            {
                pass_cheker = "123";
                pass >> pass_cheker;
            }

            string login_correct;
            string pass_correct;

            cout << "Введите свой логин: "; cin >> login_correct;
            cout << endl;
            cout << "Введите свой пароль: "; cin >> pass_correct;

            if (login_correct != log_cheker && pass_correct != pass_cheker)
            {
                cout << "Пароль или логин введен не правильно!" << endl;
            }
            else
            {
                cout << "Данные введены верно!" << endl;
            }
            return 0;
        }
    }
    int Sign_up()
    {
        string login = "l.txt";
        string password = "pass.txt";

        fstream log;
        fstream pass;

        log.open(login, fstream::in | fstream::out | fstream::app);
        pass.open(password, fstream::in | fstream::out | fstream::app);

        if (!log.is_open() && !pass.is_open())
        {
            cout << "Ошибка!" << endl;
        }
        else
        {
            string log_cheker;
            string pass_cheker;
            cout << "Введите свой логин(без пробелов): "; cin >> log_cheker;
            cout << endl;
            cout << "Введите свой пароль(без пробелов): "; cin >> pass_cheker;

            log << log_cheker;
            pass << pass_cheker;

            cout << "Регистрация прошла успешно!" << endl;

            return 0;

        }
    }

};
class menu
{
public:

    int RunMenu()
    {
        int punkt;
        cout << "Добро пожаловать!" << endl << "1.Вход" << endl << "2.Регистрация" << endl;
        cin >> punkt;


        if (punkt != 1 && punkt != 2)
        {
            cout << "Выбран неверный пункт меню!" << endl;
        }
        if (punkt == 1)
        {
            system("cls");
            Password pass;
            pass.Entrance();
        }
        if (punkt == 2)
        {
            system("cls");
            Password pass;
            pass.Sign_up();
        }
        return 0;
    }
};