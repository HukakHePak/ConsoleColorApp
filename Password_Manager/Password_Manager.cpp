
#include <iostream>
#include <conio.h>
#include <fstream>
#include <authorization.h>

using namespace std;
extern char authfilename[];
char reserv[] = "reserv_pass.dat";		// файл для временных значений

bool checkDB()		// проверка наличия и создание файла
{
	if (checkFile());
		return true;

	if (createFile())
		return true;
	return false;
}

void printLogin()		// вывод всего файла на экран
{
	ifstream imp(authfilename, ios::binary);

	if (imp)
	{
		form element;

		while (!imp.read((char*)&element, sizeof(form)).eof())
		{
			cout << "Имя пользователя: " << element.name << endl;
			cout << "Логин: " << element.login << endl;
			cout << "пароль: " << element.password << endl;
		}

		imp.close();
	}
}

void addLogin()		// добавляет в список нового пользователя
{
	ofstream exp(authfilename, ios::binary | ios::app);

	if (exp)
	{
		form element;
		
		cout << "Введите имя пользователя:";
		cin >> element.name;
		cout << "Введите логин:";
		cin >> element.login;
		cout << "Введите пароль:";
		cin >> element.password;

		exp.write((char*)&element, sizeof(form));
	}
	exp.close();
}

bool deleteFile()		// стирает все данные в файле
{
	if(remove(authfilename))
		return true;
	return false;
}

bool deleteLogin(const char* log)		// удаление пользователя по логину
{
	ofstream fresh(reserv, ios::binary);	// создаем новый файл
	if (fresh)
	{
		ifstream imp(authfilename, ios::binary);	// открываем старый

		if (imp)
		{
			form element;

			while (!imp.read((char*)&element, sizeof(form)).eof())		//считываем старый файл
			{	
				if (strcmp(log, element.login) != 0)			// переписываем все, кроме указанного логина
					fresh.write((char*)&element, sizeof(form));
			}
			imp.close();
			remove(authfilename);
			fresh.close();
			rename(reserv, authfilename);
			return true;
		}
	}
	return false;
}

void menu()
{
	system("cls");
	cout << "Менеджер паролей" << endl;
	cout << endl;
	cout << "1. Добавить пользователя" << endl;
	cout << "2. Просмотр всех пользователей" << endl;
	cout << "3. Удаление пользователя по логину" << endl;
	cout << "4. Проверка авторизации" << endl;
	cout << "5. Удаление всех пользователей" << endl;
	cout << endl;
	cout << "6. Выход" << endl;
}

int main()
{
	system("chcp 1251");

	char key;

	checkDB();

	while (true)
	{
		menu();

		key = _getch();
		system("cls");
		
		if (key == 27) break;

		switch(key)
		{
		case '1':
			addLogin();
			break;
		case '2':
			printLogin();
			break;
		case'5':
			char warning[9];
			cout << "Введите 'delete' для подтверждения удаления: ";
			cin >> warning;
			
			if (strcmp(warning, "delete") == 0)
			{
				if (deleteFile())
					cout << "файл стерт" << endl;
			}
			else
				cout << "Удаление не прошло"<<endl;
			break;

		case'4':
			char log[15];
			char pass[15];
			cout << "Введите логин: ";	
			cin >> log;
			cout << "Введите пароль: ";
			cin >> pass;

			if (authorization(findLogin(log), pass))
				cout << "Авторизация прошла успешно" << endl;
			else
				cout << "Ошибка авторизации. Попробуйте еще раз" << endl;
			break;
		case'3':
			cout << "Введите логин: ";
			cin >> log;
			deleteLogin(log);
			break;

		case'6':
			return 0;
		default:
			break;
		}
		(void)_getch();
	}
}

