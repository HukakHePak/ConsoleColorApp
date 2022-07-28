
#include <iostream>
#include <conio.h>
#include <fstream>
#include <ceerte.h>
#include <windows.h>
#include <authorization.h>
#include <vector>
#include <algorithm>

using namespace std;

char externalfile[25] = "database.dat";		// файл хранения базы данных

LPCWSTR windowName = L"Contract Manager";	// имя окна

bool FULL = true;		// запуск полного экрана
bool auth = false;		// включить авторизацию

//extern short mainColor;
//extern bool drawing;
extern short doubleColor;

short menuY = 8;		// вертикальный отступ от середины для окна меню (количество пунктов меню)
short menuX = 16;		// горизонтальный отступ от середины для окна меню

short listY = 4;		// вертикальный отступ от вернего края для вывода базы данных
short listX = 8;		// горизонтальный отступ от левого края	для вывода базы данных
int columns = 8;		// количество колонок для вывода базы данных
const int contextSize = 101;		// длина строки, отведенной для хранения содержания договора
int contextColumnsWidth = 3;		// сколько столбцов таблицы будет занимать содержание договора

void printNull();		// уведомление, если нет результата
double checkDoubleException();		// проверка

bool authorizationScene();		// форма для ввода пароля
// в файле и пароль, возвращает логическое значение, если пароли совпадают

struct contract				// Структура хранения информации о договоре
{
	int number;
	char surname[15];
	char name[15];
	char patronymic[15];
	char context[contextSize];
	double price;
};

struct comp			// структура оператора сортировки
{
	bool operator()(const contract& a, const contract& b)
	{
		return a.number < b.number;
	}
};

vector <contract> list;

void printTableHead()		// вывод шапки таблицы
{
	setTheme();
	system("cls");

	if (drawing)		// если цветная тема включена, то закрашиваем
	{
		inverseColor(mainColor);		// инвертируем цвета для шапки
		rainbowTable(listY, listY);			// закраска строки
	}

	table("Номер договора:", 0, listY);	// печатаем в 5 столбцов начальные значения, в первом задаем отступ
	table("Фамилия клиента:", 1);
	table("Имя клиента:", 2);
	table("Отчество клиента:", 3);
	table("Содержание договора:", 4);
	table("Цена договора:", 7);
	cout << endl;	// перенос строки после заполнения первой строки таблицы
	cout << endl;	// для разделения
}

void printContractInTable(int i)		// вывод полученных значений в виде таблицы
{
	// сначала я бы хотел вывести содержание договора, так как от его высоты зависит раскраска строк
	short line = getConcoleY();		// необходимо сохранить вертикальный отступ перед выводом форматированного текста
	short nextline = parseInColumn(list[i].context, table("", 4) * contextColumnsWidth);
	// также необходимо сохранить отступ для следующего элемента
	/*Это наш форматированный вывод содержимого договора. Отправлет строку и выводит ее по заданной ширине.
	Ширина задается количеством столбцов, а текущую ширину, помимо того, чтобы установить красную строку 4-го
столбца, нам возвращает функция table. Также эта функция возвращает вертикальный отступ для вывод следующей строки*/

	if (drawing) // если цветная тема отключена, ставит стандартный цвет
	{
		setColor();					// для остальных берем автоматически выбранный
		line = rainbowTable(line, nextline);		// так как parseInColumn не заполняет всю выделенную для него область,
	}											// то нам нужна функция раскраски строки
	else
		setTheme();		// иначе ставим основную тему

	parseInColumn(list[i].context, table("", 4, line) * contextColumnsWidth, true); // когда мы получили отступы и закрасили область
	// можно смело печатать содержимое договора. Для того, чтобы функция выводила текст необходимо отправить true, чтобы включить вывод

	table("", 0, line);		// при преобразовании int в const char* выдает ошибки, поэтому просто перемещаем указатель
						// а так как у нас из за содержания он мог сместиться, то берем исходный
	cout << list[i].number;		// а затем выводим

	table(list[i].surname, 1);		//
	table(list[i].name, 2);
	table(list[i].patronymic, 3);
	table("", 7);
	cout << list[i].price;
	GotoXY(0, nextline);	// так как указатель вернулся, чтобы не затереть содержание договора устанавливаем отступ, 
	cout << endl;			// который нам вернула функция вывода содержания договора. Переносим строку
}

int findByNumber(int num)
{
	int i = 0;
	while (i != list.size())
	{
		if (list[i].number == num)	// если найден, то возвращает индекс вектора
			return i;
		i++;
	}
	return -1;	// пустота, если нет элемента
}

int findByNumberScene()		// поиск индекса договора в векторе по номеру договора
{
	ShowCursor();
	system("cls");

	randomColor();

	int number;
	center("Введите номер договора: ", -1, 1, false, setColor());
	cin >> number;

	return findByNumber(number);
}

bool printContract(int i)		// вывод договора на экран
{
	if (i == -1)		// если договор не найден, то выходим
		return false;			// и говорим, что договоров нет

	setTheme();
	system("cls");

	randomColor();	// устанавливаем рандомный цвет вывода
	//short color = setColor();

	center("Номер договора: ", -1, 6, false, setColor());
	cout << list[i].number << endl;

	center("Фамилия клиента: ", -1, 0, false, setColor());
	cout << list[i].surname << endl;

	center("Имя клиента: ", -1, 0, false, setColor());
	cout << list[i].name << endl;

	center("Отчество клиента: ", -1, 0, false, setColor());
	cout << list[i].patronymic << endl;

	center("Содержание договора: ", -1, -1, false, setColor());
	parseInColumn(list[i].context, (WidthOfWindow() / columns) * contextColumnsWidth, true);
	cout << endl;

	center("Цена договора: ", -1, 0, false, setColor());
	cout << list[i].price << endl;

	setTheme();
	return true;
}

bool addElement()			// ввод нового договора
{
	system("cls");
	contract element = {};
	ShowCursor();
	randomColor();

	center("Введите номер договора: ", -1, 6, false, setColor());
	cin >> element.number;

	if (element.number < 1 || findByNumber(element.number) != -1)		// проверяем значение на уникальность
	{
		setTheme();
		return false;
	}

	center("Введите фамилию клиента: ", -1, 0, false, setColor());
	cin >> element.surname;
	center("Введите имя клиента: ", -1, 0, false, setColor());
	cin >> element.name;
	center("Введите отчество клиента: ", -1, 0, false, setColor());
	cin >> element.patronymic;

	center("Введите содержание договора: ", -1, 0, false, setColor());
	cin.get();
	cin.getline(element.context, sizeof(element.context));

	center("Введите цену договора: ", -1, 0, false, setColor());
	element.price = checkDoubleException();

	list.push_back(element);

	HideCursor();
	setTheme();
	
	return true;
}

bool findBySurnameScene()
{
	system("cls");

	center("Введите фамилию: ", -1, 1, false, setColor());
	ShowCursor();

	char surname[15];	// ихихих фыр фыр фыр шкшкшшк фыр фыр
	cin >> surname;

	printTableHead();
	randomColor();		// установка цвета
	
	bool finded = false;

	for (int i = 0; i < list.size(); i++)
		if (!strcmp(list[i].surname, surname))
		{
			printContractInTable(i);
			finded = true;
		}

	return finded;
}

bool findByPriceZoneScene()
{
	setTheme();
	system("cls");

	center("Введите минимальную цену: ", -1, 2, false, setColor());
	ShowCursor();
	double minPrice = checkDoubleException();		// выполняем проверку на правильность ввода
	center("Введите максимальную цену: ", -1, -2, false, setColor());
	double maxPrice = checkDoubleException();

	printTableHead();
	randomColor();		// установка цвета

	bool finded = false;

	for (int i = 0; i < list.size(); i++)
		if (list[i].price >= minPrice && list[i].price <= maxPrice)
		{
			printContractInTable(i);
			finded = true;
		}

	return finded;
}

void deleteElementScene()
{
	int numberof = findByNumberScene();
	setTheme();

	while (true)
	{
		if (!printContract(numberof))
		{
			setTheme();
			system("cls");
			center("Договор не найден", 0, 1, true);
			return;
		}
		setTheme();

		cout << endl;
		center("Вы действительно хотите удалить этот элемент?", 0, 0, true);

		HideCursor();

		cout << endl;
		center("1. Да", 10, 0);
		center("2. Нет", -5, 0, true);

		int key = _getch();
		if (key == 27)
			break;

		system("cls");
		switch (key)
		{
		case '1':
			list.erase(list.begin() + numberof);
			center("Удаление прошло успешно", 0, 1, true);
			return;

		case '2':
			center("Удаление отменено", 0, 1, true);
			return;

		default:
			center("Пункта с таким номером не существует", 0, 1, true);
			pause();
			break;
		}
	}
}

void setExternalFileName(const char* login)		// принимает строку и изменяет имя для обращения к внешнему файлу
{
	strcpy_s(externalfile, login);		// копируем логин в название файла
	char dat[] = ".dat\0";				// дописываем формат файла \0 для конца строки
	int length = strlen(externalfile);		// нам нужна текущая длина названия для установки указателя массива
	for (int i = 0; i < 6; i++)
		externalfile[length + i] = dat[i];	// копируем формат файла
}

void importDataBase()		// импорт всех данных из файла в память
{
	ifstream imp(externalfile, ios::binary);		// открытие в бинарном виде
	if (imp)
	{
		contract element;
		while (!imp.read((char*)&element, sizeof(contract)).eof())	// считывает все элементы и создает их в памяти		
			list.push_back(element);

		imp.close();
	}
}

void exportDataBase()	// экспорт всех договоров из вектора в файл
{
	ofstream exp(externalfile, ios::binary);
	if (exp)
	{
		while (!list.empty())		// пока вектор не пуст
		{
			contract element = list.back();		// получает последний договор
			if (element.number > 0)				// если договор не пустой, то
				exp.write((char*)&element, sizeof(element));	// записывает данные в файл	
			list.pop_back();		// удаляет элемент
		}
		exp.close();
	}
}

void printMenu()		// печать главного меню
{
	setTheme();
	system("cls");
	randomColor();

	center("УЧЕТ ДОГОВОРОВ НОТАРИАЛЬНОЙ КОНТОРЫ", 0, menuY, true, setColor(), true);	// добавим немного цветов в заголовок
	cout << endl;
	setTheme();

	center("1. Добавить договор", menuX, 0, true);
	center("2. Вывод списка договоров", menuX, 0, true);
	center("3. Поиск договора по номеру", menuX, 0, true);
	center("4. Поиск договора по фамилии клиента", menuX, 0, true);
	center("5. Поиск договора по диапазону цен", menuX, 0, true);
	center("6. Удалить договор по номеру", menuX, 0, true);
	center("7. Настройки", menuX, 0, true);
	center("8. Выйти", menuX, 0, true);

	HideCursor();
}

void menu()		// функция главного меню
{
	char key;		// переменная для ввода с клавиатуры
	//short y;		// переменная для вертикального отступа
	contract* findZone;

	while (true)		// цикл для меню
	{
		printMenu();		// печать меню

		key = _getch();		// ввод с клавиатуры

		if (key == 27 || key == '8')
			break;		// если ESC или 7, то выходим из цикла

		if (key == '1')		// добавление элемента
		{
			if (addElement())	// если элемент создан, то уведомим пользователя
			{
				system("cls");
				center("Договор успешно добавлен", 0, 1, true);
			}
			else
			{
				system("cls");
				center("Невозможно добавить договор с таким номером, попробуйте другой номер", 0, 1, true);
			}
		}

		else if (key == '7')
			settings();

		else if (!list.empty())	// почти все наши функции имеет смысл выполнять, если список не пустой, поэтому сразу проверка.
			switch (key)
			{
			case '2':	// вывод всех элементов вектора
				printTableHead();
				randomColor();			// установка цвета
				
				for(int i = 0; i < list.size(); i++)
					printContractInTable(i);		// выводит все элементы в виде таблицы
				break;

			case '3':	// вывод элемента по номеру договора
				if (!printContract(findByNumberScene())) // false если мы обращаемся из пункта печати
					printNull();	// уведомление, если не найдены значения
				break;

			case '4':		// вывод всех элементов, найденных по фамилии клиента
				if (!findBySurnameScene())
					printNull();;
				break;

			case '5':		// вывод всех элементов по заданному диапазону цен

				if (!findByPriceZoneScene())
					printNull();
				break;

			case '6':		// удаление элемента по номеру договора
				deleteElementScene();
				break;

			default:		// обработка неправильных нажатий
				system("cls");
				center("Пункта с таким номером не существует", 0, 1, true);
				break;
			}
		else
			printNull();
		pause();
	}
}

int main()
{
	loadSettings();

	if (FULL)		// использование полноэкранного режима
	{
		COORD coord;
		SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, &coord);
	}

	system("chcp 1251");
	SetConsoleTitle(windowName);		// установка имени окна

	while (auth)	// цикл запроса авторизации
	{
		if (authorizationScene())
			break;
		if (_getch() == 27)
			return 0;
	}

	if (checkDataBase(externalfile))	// проверка существвания базы данных
		importDataBase();	// если есть, то импортируем
	else
	{
		cout << "Ошибка подключения файла";		// иначе выходим, нам что, заняться нечем? (ну вообще да, хех)
		pause();
		return 0;
	}
	setTableAlign(listX, columns);

	menu();		// запуск главного меню

	if (!list.empty())
	{
		sort(list.begin(), list.end(), comp());		// сортировка
		exportDataBase();			// отгрузка всех записей в файл
		list.clear();
	}

	saveSettings();

	return 0;
}

double checkDoubleException()		// проверка значений для диапазона цен на правильность
{
	double number;
	do
	{
		cin >> number;
		if (cin.fail())			// если есть ошибка вывода из cin
		{
			cin.clear();		// чистим ошибку
			cin.ignore(32767, '\n');		// чистим ненужные значения
		}
		else
			return number;
	} while (true);		// цикл будет выполняться, пока не будут введены цифры (стремно, но лучше, чем просто краши)
}

bool authorizationScene()	// экран авторизации, запрос ввода и вывод результата
{
	setTheme();
	system("cls");
	ShowCursor();
	randomColor();		// установка рандомного цвета вывода для цветных выводов

	form auth;			// библиотечная форма длях ранения логина, пароля и имени пользователя

	center("Введите логин: ", -1, 2, false, setColor());
	cin >> auth.login;

	center("Введите пароль: ", -1, -2, false, setColor());

	logicOfDraw("", doubleColor);	// так как в double у нас хранится последний использованный,
	SetTextColor(doubleColor);	// то эта фунцкия логически вернет нам то, что должно 
								// выводиться после нее. так мы скрываем пароль
	cin >> auth.password;

	setTheme();
	system("cls");

	form element = returnFormByLogin(findLogin(auth.login));	// возвращает заполненную форму пользователя

	if (!strcmp(element.password, auth.password))		// сравниваем полученный пароль с заданным
	{
		randomColor();		// установка цвета для окна приетствия

		center("Добрый день, ", -1, 1, false, setColor());
		cout << element.name << "!" << endl;

		setExternalFileName(element.login);		// устанавливает имя файла для базы данных

		setTheme();
		pause();
		return true;
	}
	center("Ошибка авторизации. Попробуйте еще раз", 0, 1, true);
	HideCursor();
	return false;
}