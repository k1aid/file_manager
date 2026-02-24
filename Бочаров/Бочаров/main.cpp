
#include <iostream>
#include <fstream>
#include <string>
#include "functions.h"

using namespace std;

int main() 
{
    setlocale(LC_ALL, "");

    FileManager fm;
    int choice;
    string path1, path2;
    menu vhod;


    vhod.RunMenu();

        while (true)
        {
            showMenu();
            cin >> choice;
            cin.ignore();

            if (choice == 0) break;

            switch (choice)
            {
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
                if (attr & FILE_ATTRIBUTE_DIRECTORY)
                {
                    DirectoryItem d(path1);
                    cout << "Размер папки рекурсивно: " << d.getSize() << " байт.\n";
                }
                else
                {
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