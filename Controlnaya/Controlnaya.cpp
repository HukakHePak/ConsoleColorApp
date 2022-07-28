
#include <iostream>
#include <conio.h>
#include <fstream>
#include <ceerte.h>
#include <windows.h>
#include <authorization.h>

using namespace std;

char externalfile[25] = "database.dat";		// файл хранения базы данных

LPCWSTR windowName = L"Contract Manager";	// имя окна

bool FULL = true;		// запуск полного экрана
bool auth = false;		// включить авторизацию

extern short doubleColor;

short menuY = 8;		// вертикальный отступ от середины для окна меню (количество пунктов меню)
short menuX = 16;		// горизонтальный отступ от середины для окна меню

short listY = 4;		// вертикальный отступ от вернего края для вывода базы данных
short listX = 8;		// горизонтальный отступ от левого края	для вывода базы данных
int columns = 8;		// количество колонок для вывода базы данных
const int contextSize = 101;		// длина строки, отведенной для хранения содержания договора
int contextColumnsWidth = 3;		// сколько столбцов таблицы будет занимать содержание договора

void sortByNumber();		// пузырьковая сортировка по номеру
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
	contract* next;		// указатель для элементов списка
};

contract* last = NULL;		// указатель на конец списка

contract* newElement(int num, const char* sur, const char* nam, const char* pat, const char* con, double pri)
{			// создание и вставка нового элемента в конец списка
	contract* element = new contract;		// создаем шаблон элемента

	element->number = num;						// копируем пришедшее число в созданный шаблон
	strcpy_s(element->surname, sur);				// копируем пришедшую строку в созданный шаблон
	strcpy_s(element->name, nam);
	strcpy_s(element->patronymic, pat);
	strcpy_s(element->context, con);
	element->price = pri;

	element->next = last;					// сохраняем ссылку на следующий элемент
	last = element;					// записываем указатель нового шаблона как первый
	return element;						// возвращаем указатель на только что созданный элемент
}

void deleteList()			// очистка занятой списком памяти
{
	contract* element = last;		// создаем указатель и присваиваем ссылку последнего элемента из списка
	contract* ghost;			// переменная для хранения указателей после удаления
	while (element != NULL)			// если список не пуст, то заходим в цикл
	{
		ghost = element->next;	// запоминаем ссылку на следующий элемент
		delete element;
		element = ghost;
	}
}

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

contract* printElementsByZone(contract* element)		// вывод полученных значений в виде таблицы
{
	// сначала я бы хотел вывести содержание договора, так как от его высоты зависит раскраска строк
	short line = getConcoleY();		// необходимо сохранить вертикальный отступ перед выводом форматированного текста
	short nextline = parseInColumn(element->context, table("", 4) * contextColumnsWidth);
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

	parseInColumn(element->context, table("", 4, line) * contextColumnsWidth, true); // когда мы получили отступы и закрасили область
	// можно смело печатать содержимое договора. Для того, чтобы функция выводила текст необходимо отправить true, чтобы включить вывод

	table("", 0, line);		// при преобразовании int в const char* выдает ошибки, поэтому просто перемещаем указатель
						// а так как у нас из за содержания он мог сместиться, то берем исходный
	cout << element->number;		// а затем выводим

	table(element->surname, 1);		//
	table(element->name, 2);
	table(element->patronymic, 3);
	table("", 7);
	cout << element->price;
	GotoXY(0, nextline);	// так как указатель вернулся, чтобы не затереть содержание договора устанавливаем отступ, 
	cout << endl;			// который нам вернула функция вывода содержания договора. Переносим строку

	return element->next;	// запоминаем ссылку на следующий элемент
}

contract* findElementByNumber(int num, bool del)		// поиск элемента списка по номеру с возможностью удаления
{
	contract* element = last;			// создаем указатель и присваиваем ссылку последнего элемента из списка
	contract* preLink = element;		// указатель для хранения адреса предыдущей перед искомой ячейкой
	while (element != NULL)			// перебираем, пока не пройдем список
	{
		if (element->number == num)		// сравниваем текущее имя с искомым
		{
			if (del)				// если хотим удалить, то вернем ссылку на предыдущий элемент
				return preLink;
			return element;		// иначе ссылку на текущий
		}
		preLink = element;		// сохраняем текущую ссылку
		element = element->next;	// берем следующую ссылку
	}
	return NULL;	// пустота, если нет элемента
}

contract* findElementBySurname(contract* lastFound, const char* sur)		// поиск элемента списка по фамилии
{
	contract* element = lastFound;		// берем входящую ссылку
	while (element != NULL)
	{
		if (strcmp(element->surname, sur) == 0)
			return element;
		element = element->next;
	}
	return NULL;
}

contract* findElementByPriceZone(contract* lastFound, double minPrice, double maxPrice)		// поиск элемента списка по диапазону цен
{
	if (minPrice > maxPrice)
		swap(minPrice, maxPrice);

	contract* element = lastFound;
	while (element != NULL)
	{
		if (element->price >= minPrice && element->price <= maxPrice)		// ищем между заданными минимальным и максимальным
			return element;
		element = element->next;
	}
	return NULL;
}

bool printElement(contract* element)		// вывод элемента на экран
{
	if (element == NULL)		// если ссылка пустая, то выходим
		return false;			// и говорим, что элементов нет

	setTheme();
	system("cls");

	randomColor();	// устанавливаем рандомный цвет вывода

	//short color = setColor();

	center("Номер договора: ", -1, 6, false, setColor());
	cout << element->number << endl;

	center("Фамилия клиента: ", -1, 0, false, setColor());
	cout << element->surname << endl;

	center("Имя клиента: ", -1, 0, false, setColor());
	cout << element->name << endl;

	center("Отчество клиента: ", -1, 0, false, setColor());
	cout << element->patronymic << endl;

	center("Содержание договора: ", -1, -1, false, setColor());
	parseInColumn(element->context, (WidthOfWindow() / columns) * contextColumnsWidth, true);
	cout << endl;

	center("Цена договора: ", -1, 0, false, setColor());
	cout << element->price << endl;

	setTheme();
	return true;
}

bool deleteElementByNumber(int num)		// удаление элемента по номеру
{
	if (last->number == num)		// если удаляемый элемент последний, то 
	{
		contract* lastElement = last->next;		// сохраняем ссылку на следующий элемент
		delete last;			// удаляем элемент
		last = lastElement;		// обозначаем следующий как последний
		return true;		// завершаем
	}
	contract* preLink = findElementByNumber(num, true);		// иначе ищем элемент, стоящий перед искомым, и сохраняем ссылку на него
	if (preLink == NULL)
		return false;

	contract* element = preLink->next;		// берем ссылку на следующий(искомый) элемент
	preLink->next = element->next;			// записываем в указатель предыдущего элемента указатель элемента, стоящего перед искомым
	delete element;			// и наконец удаляем искомый элемент
	return true;
}

int askNumber()		// запрашивает и возвращает номер договора
{
	ShowCursor();
	system("cls");

	randomColor();

	int number;
	center("Введите номер договора: ", -1, 1, false, setColor());
	cin >> number;

	return number;
}

bool addElement()			// ввод нового договора
{
	system("cls");
	contract element = {};
	ShowCursor();
	randomColor();

	center("Введите номер договора: ", -1, 6, false, setColor());
	cin >> element.number;

	if (element.number < 1 || findElementByNumber(element.number, false) != NULL)		// проверяем значение на уникальность
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

	HideCursor();
	setTheme();
	newElement(element.number, element.surname, element.name, element.patronymic, element.context, element.price);
	return true;
}

bool findByNameScene()
{
	system("cls");

	center("Введите фамилию: ", -1, 1, false, setColor());
	ShowCursor();

	char surname[15];	// ихихих фыр фыр фыр шкшкшшк фыр фыр
	cin >> surname;
	contract* findZone = findElementBySurname(last, surname);	// поиск первого элемента с начала списка

	if (findZone != NULL)	//	если найден хоть один, то выводим
	{
		printTableHead();
		randomColor();		// установка цвета
		do
		{
			printElementsByZone(findZone);
			findZone = findElementBySurname(findZone->next, surname);
			// для этого отправляем указатели на следующий, после найденного, элемент
		} while (findZone != NULL);		// и так пока не кончится список
		return true;
	}
	return false;
}

bool findElementByZoneScene()
{
	setTheme();
	system("cls");

	center("Введите минимальную цену: ", -1, 2, false, setColor());
	ShowCursor();
	double minPrice = checkDoubleException();		// выполняем проверку на правильность ввода
	center("Введите максимальную цену: ", -1, -2, false, setColor());
	double maxPrice = checkDoubleException();

	contract* findZone = findElementByPriceZone(last, minPrice, maxPrice);	// ищем первый элемент

	if (findZone != NULL)		// если не последний, то ищем другие
	{
		printTableHead();
		randomColor();			// установка цвета
		do
		{
			printElementsByZone(findZone);
			findZone = findElementByPriceZone(findZone->next, minPrice, maxPrice);
		} while (findZone != NULL);
		return true;
	}
	return false;
}

void deleteElementScene()
{
	int numberof = askNumber();
	setTheme();

	while (true)
	{
		if (!printElement(findElementByNumber(numberof, false)))
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
			if (deleteElementByNumber(numberof))
			{
				center("Удаление прошло успешно", 0, 1, true);
				return;
			}
			center("Договор не может быть удален", 0, 1, true);
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
	char dat[] = ".dat\0";			// дописываем формат файла \0 для конца строки
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
			newElement(element.number, element.surname, element.name, element.patronymic, element.context, element.price);

		imp.close();
	}
}

void exportDataBase()	// экспорт из памяти всех элементов в файл
{
	ofstream exp(externalfile, ios::binary);
	if (exp)
	{
		contract* element = last;
		while (element != NULL)		// пока список не пуст
		{
			exp.write((char*)element, sizeof(contract));	// записывает данные в файл
			element = element->next;		// перемещает указатель
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

		else if (last != NULL)	// почти все наши функции имеет смысл выполнять, если список не пустой, поэтому сразу проверка.
			switch (key)
			{
			case '2':	// вывод всех элементов списка
				printTableHead();
				findZone = last;

				randomColor();			// установка цвета
				while (findZone != NULL)
					findZone = printElementsByZone(findZone);	// для этого отправляем указатели на следующий, после найденного, элемент
				break;

			case '3':	// вывод элемента по номеру договора
				if (!printElement(findElementByNumber(askNumber(), false))) // false если мы обращаемся из пункта печати
					printNull();	// уведомление, если не найдены значения
				break;

			case '4':		// вывод всех элементов, найденных по фамилии клиента
				if (!findByNameScene())
					printNull();;
				break;

			case '5':		// вывод всех элементов по заданному диапазону цен

				if (!findElementByZoneScene())
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

	setTableAlign(listX, columns);	// задать параметры таблицы

	menu();		// запуск главного меню

	if (last != NULL)
	{
		sortByNumber();
		exportDataBase();	// отгрузка всех записей в файл
		deleteList();	// очистка списка
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

void sortByNumber()		// пузырьковая сортировка (опционально)
{
	if (last->next != NULL)
	{
		int counter = 1;		// счетчик изменений
		while (counter)
		{
			contract* element = last;		// обозначаем первые элементы
			contract* afterElement = element->next;
			contract* preElement;		// создаем переменную для хранения предыдущих указателей
			counter = 0;		// обнуляем счетчик

			if (element->number > afterElement->number)		// нам нужно сохранять last, если мы меняем местами первые элементы
			{
				last = afterElement;		// запоминаем второй элемент как последний
				element->next = afterElement->next;		// записываем ссылку первого на третий элемент
				afterElement->next = element;		// записываем ссылку второго элемента на первый
				preElement = afterElement;		// запоминаем, что второй элемент теперь у нас первый и самый последний
				counter++;			// изменяем счетчик
			}
			else
			{	// если уэлементы не меняются, то
				preElement = element;		// запоминаем первый элемент как предыдущий
				element = element->next;	// и переходим к следующему
			}

			if (element != NULL)
			{
				afterElement = element->next;	// обозначаем следующий элемент
				while (afterElement != NULL)	// теперь можно перейти к проверке всех элементов до конца
				{
					if (element->number > afterElement->number)
					{
						preElement->next = afterElement;	// ззаписываем ссылку предыдыдущего (1-го) элемента на следующий (3-ий)
						element->next = afterElement->next;		// записываем ссылку текущего элемента (2-го) на последующий (4-ый)
						afterElement->next = element;	// записываем ссылку следующего (3-го) на текущий (2-ой)
						preElement = afterElement;		// запоминаем следующий (3-ий) как предыдущий (1-ый)
						counter++;				// изменяем счетчик

					}
					else
					{
						preElement = element;
						element = element->next;
					}
					afterElement = element->next;
				}
			}
		}
	}
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
