#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <clocale>
#include <conio.h> 
using namespace std;

class Authorization {
private:
    // Секретный ключ для XOR-шифрования
    string key = "f7f9c01bd1229a17de6d9a9a5815b61cb0b6459f5ebd5cbfa3361a1e40048803";

    // Внутренний метод для шифрования данных в файле
    string crypt(string data) {
        string output = data;
        for (int i = 0; i < data.size(); i++) {
            output[i] = data[i] ^ key[i % key.size()];
        }
        return output;
    }

    // Внутренний метод для ввода пароля "звездочками"
    string getHiddenInput() {
        string input = "";
        char ch;
        while (true) {
            ch = _getch(); // Читаем символ без эха в консоль
            if (ch == 13) { // Клавиша Enter
                cout << endl;
                break;
            }
            if (ch == 8) { // Клавиша Backspace
                if (input.length() > 0) {
                    input.pop_back();
                    cout << "\b \b"; // Стираем звездочку с экрана
                }
            }
            else if (ch >= 32) { // Печатаемые символы
                input += ch;
                cout << "*";
            }
        }
        return input;
    }

public:
    // Метод для входа в систему
    bool Entrance() {
        string log_enc, pass_enc, l_user, p_user;
        ifstream f_log("login.txt"), f_pass("password.txt");

        if (!f_log.is_open() || !f_pass.is_open()) {
            cout << "\n[!] База пользователей пуста. Сначала зарегистрируйтесь.\n";
            Sleep(1500);
            return false;
        }

        f_log >> log_enc;
        f_pass >> pass_enc;

        // Расшифровываем эталонные данные из файлов
        string log_real = crypt(log_enc);
        string pass_real = crypt(pass_enc);

        cout << "Введите логин: "; cin >> l_user;
        cout << "Введите пароль: "; p_user = getHiddenInput();

        if (l_user == log_real && p_user == pass_real) {
            cout << "Успешный вход!";
            Sleep(1000);
            return true;
        }
        else {
            cout << "Неверный логин или пароль!";
            Sleep(1500);
            return false;
        }
    }

    // Метод для регистрации нового пользователя
    void Sign_up() {
        string l_new, p_new;
        cout << "Придумайте логин: "; cin >> l_new;
        cout << "Придумайте пароль: "; p_new = getHiddenInput();

        // Шифруем перед записью
        string l_to_save = crypt(l_new);
        string p_to_save = crypt(p_new);

        ofstream f_log("login.txt"), f_pass("password.txt");
        f_log << l_to_save;
        f_pass << p_to_save;

        cout << "Регистрация завершена! Теперь войдите в систему.\n";
        Sleep(2000);
    }

    // То самое RunMenu, которое управляет процессом
    int Run() {
        while (true) {
            system("cls");
            cout << "1. Вход\n2. Регистрация\nВыберите пункт: ";

            int punkt;
            if (!(cin >> punkt)) { // Защита от букв
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore(10000, '\n'); // Очистка после ввода цифры

            if (punkt == 1) {
                if (Entrance()) return 1; // Возвращаем 1, чтобы пустить в менеджер
            }
            else if (punkt == 2) {
                Sign_up();
            }
            else if (punkt == 0) {
                exit(0);
            }
        }
    }
};

void showMenu(string path) 
{
    system("cls");
    cout << "----- ТЕКУЩАЯ ПАПКА: " << path << " -----\n";
    cout << "1. Листинг \n2. Перейти (cd) \n3. Новая папка \n4. Новый файл\n";
    cout << "5. Удалить \n6. Переименовать \n7. Поиск \n8. Размер \n0. Выход\n";
    cout << "Выберите действие: ";
}