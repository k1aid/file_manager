#include "File_System.h"
#include "Function_Manager.h"
#include "Authorization.h"

int main() 
{
    setlocale(LC_ALL, "");


    Authorization vhod;
    while (vhod.Run() != 1);

    FileManager fm;
    char buf[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buf);
    string currentPath = buf;

    int choice;
    string p1, p2;

    while (true) {
        showMenu(currentPath);
        cin >> choice;
        cin.ignore();

        if (choice == 0) break;

        switch (choice) 
        {
        case 1: fm.list(currentPath); break;
        case 2:
            cout << "Имя папки или '..': "; getline(cin, p1);
            if (SetCurrentDirectoryA(p1.c_str())) 
            {
                GetCurrentDirectoryA(MAX_PATH, buf);
                currentPath = buf;
            }
            else cout << "Ошибка пути!\n";
            break;
        case 3: cout << "Имя папки: "; getline(cin, p1); fm.makeDir(p1); break;
        case 4: cout << "Имя файла: "; getline(cin, p1); fm.makeFile(p1); break;
        case 5: cout << "Имя для удаления: "; getline(cin, p1); fm.remove(p1); break;
        case 6:
            cout << "Старое имя: "; getline(cin, p1);
            cout << "Новое имя: "; getline(cin, p2);
            MoveFileA(p1.c_str(), p2.c_str());
            break;
        case 7:
            cout << "Маска (напр. *.txt): "; getline(cin, p1);
            fm.search(currentPath, p1);
            break;
        case 8:
            cout << "Объект: "; getline(cin, p1);
            {
                DWORD attr = GetFileAttributesA(p1.c_str());
                if (attr != INVALID_FILE_ATTRIBUTES) {
                    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
                        DirectoryItem d(p1);
                        cout << "Размер папки: " << d.getSize() << " байт.\n";
                    }
                    else {
                        FileItem f(p1);
                        cout << "Размер файла: " << f.getSize() << " байт.\n";
                    }
                }
            }
            break;
        }
        cout << "\nНажмите Enter..."; cin.get();
    }

    return 0;
}