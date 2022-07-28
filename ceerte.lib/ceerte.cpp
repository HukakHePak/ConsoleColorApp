
#include <Windows.h>
#include "ceerte.h"
#include <iostream>
#include <fstream>
#include <conio.h>

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
using namespace std;

char settingsfile[] = "settings.txt";

short mainColor = 0;		// основной цвет фона. Цвет текста задается читаемым (черный или белый)
short themeColor = 0;	// цвет тематической раскраски

bool drawing = false;		// включить раскраску вывода
bool rainbow = false;		// включить разноцветную раскраску
bool inverseDraw = false;// раскрашивать значения вместо текста
bool drawAll = false;		// раскрашивать строки целиком
bool randomTheme = false;
short doubleColor = mainColor;		// хранит текущий цвет закраски консоли

short al = 0;		// горизонтальный отступ от левого края	для вывода базы данных
int cols = 1;		// количество колонок для вывода базы данных


void GotoXY(short x, short y)
{
	COORD cp;
	cp.X = x;
	cp.Y = y;
	SetConsoleCursorPosition(hStdOut, cp);
}
void SetTextColor(short color)
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(hStdOut, &inf);
	color = color & 0x0f;
	inf.wAttributes = inf.wAttributes & 0xf0;
	inf.wAttributes = inf.wAttributes | color;
	SetConsoleTextAttribute(hStdOut, inf.wAttributes);
}
void SetTextBackground(short color)
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(hStdOut, &inf);
	color = color & 0x0f;
	inf.wAttributes = inf.wAttributes & 0x0f;
	inf.wAttributes = inf.wAttributes | color<<4;
	SetConsoleTextAttribute(hStdOut, inf.wAttributes);
}
void HideCursor()
{
	CONSOLE_CURSOR_INFO cur;
	GetConsoleCursorInfo(hStdOut, &cur);
	cur.bVisible = false;
	SetConsoleCursorInfo (hStdOut, &cur);

}
void ShowCursor()
{
	CONSOLE_CURSOR_INFO cur;
	GetConsoleCursorInfo(hStdOut, &cur);
	cur.bVisible = true;
	SetConsoleCursorInfo (hStdOut, &cur);
}

short WidthOfWindow()
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(hStdOut, &inf);
	return inf.dwMaximumWindowSize.X;
}

short HeightOfWindow()
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(hStdOut, &inf);
	return inf.dwMaximumWindowSize.Y;
}

short getConcoleY()
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(hStdOut, &inf);
	return inf.dwCursorPosition.Y;
}

short getConsoleX()
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(hStdOut, &inf);
	return inf.dwCursorPosition.X;
}

void center(const char* text, short x, short y, bool line, short color, bool all, bool inverse)
{		// выравнивание текста по центру
	if (!x)
		x = (WidthOfWindow() - strlen(text)) / 2;	// если отступ ширины не задан, использует центр	
	else if (x == -1)
		x = x = WidthOfWindow() / 2 - strlen(text);	// если пришло -1, сдвигаем влево на ддлину строки
	else
		x = WidthOfWindow() / 2 - x;		// иначе пермещает указатель влево на заданную длину

	if (!y)
		y = getConcoleY();			// eсли отступ высоты не задан, то использует текущий
	else
		y = (HeightOfWindow() - y) / 2;

	GotoXY(x, y);			// перемещаем указатель в заданное положение

	if (!drawing || color == mainColor)
		cout << text;
	else
		logicOfDraw(text, color, all, inverse);		// чтобы разгрузить эту функцию, перейдем к следующей

	if (line)		//  отключение переноса по строкам
		cout << endl;

	doubleColor = color;
};


void logicOfDraw(const char* text, short color, bool all, bool inverse)		// логика установки цвета текста
{
	if (rainbow)				// если включена радужная раскраска, возвращает типо случайный цвет
		color = randomColor();

	if (all)		// если задана вся область раскраски, то установит цвет и выйдет
	{
		setTheme(color);
		cout << text;
		return;
	}

	if (inverse)		// если инверсия, то сначала раскрасит пришедший текст, и установит общую тему
	{
		setTheme();
		cout << text;
		setTheme(color);
	}
	else
	{
		setTheme(color);
		cout << text;
		setTheme();
	}
}

short rainbowTable(short startY, short finishY, short leftX, short rightX)		// принимает диапазон строк и область для исключения, раскрашивает их и возвращает начальный вертикальный отступ
{
	if (startY > finishY)		// если мы вдруг неправильно указали конченый отступ, то меняем местами
		swap(startY, finishY);

	GotoXY(0, startY);		// пермещаем указатель в начало области

	for (int i = startY; i <= finishY; i++)		// переним строку, пока не закрасим
	{
		for (int j = 0; j < WidthOfWindow(); j++)		// закрашиваем, пока не конец окна
		{
			if (j >= leftX && rightX != 0 && j < rightX)		// проверяем, если курсор дошел до области исключения и область задана,
			{
				j = rightX + 1;
				GotoXY(j, i);			//  то пропустим ее
			}
			cout << " ";
		}
		cout << endl;
	}
	return startY;		// возвращаем исходный Y
}

short randomColor()		// устанавливает рандомные цвета фона и констрастные цвета текста
{
	if (!randomTheme)
		return mainColor;
	short color = rand() % 0x0F;		// выдает любой цвет из 16
	if (color == mainColor || color == 0x06 || color == doubleColor)	// исключаем совпадение с основным цветом, предыдущим и противный цвет																		
		return randomColor();					// если таковые нашлись, то определяем их заново

	setFontColor(color);
	doubleColor = color;			// запоминаем наш цвет для исключения повторений
	SetTextBackground(color);		// и нацонец устанавливаем цвет фона

	return color;
}

void setFontColor(short color)	// принимает цвет фона, подбирает и устанавливает читабельный цвет текста
{
	SetTextColor(0x0f);			// для всех фонов текст белый
	if (color == 0x07 || color == 0x0b || color == 0x0e || color == 0x0a || color == 0x0f)
		SetTextColor(0);		// для некоторых цветов фона устанавливаем черный цвет текста для читаемости
}

short setTheme(short color)		// устанавливает цвета основной темы
{
	SetTextBackground(color);	// установка основного цвета фона
	setFontColor(color);		// установка основного цвета текста
	doubleColor = color;	// также обновим текущий цвет фона
	return color;
}

short inverseColor(short color)	// принимает цвет фона, устанавливает противоположный ему
{
	SetTextBackground(0x0f - color);	// инверсия фона
	setFontColor(0x0f - color);		// подходящий цвет текста
	doubleColor = 0x0f - color;		// фиксируем изменения
	return doubleColor;
}

short setColor()	// устанавливает цвет иходя из констант
{
	if (!drawing)		// если раскраска отключена, то оставим стандартную
		return setTheme();

	if (rainbow)	// если включена радужная раскраска, вернет новый цвет
		return randomColor();

	if (themeColor != mainColor && !randomTheme)	// если цвет темы не отличается от главного, вернет его
		setTheme(themeColor);

	return doubleColor;		// иначе вернет предыдущий
}

void printBool(bool pin)	// выводит текстовое значение вместо логического
{
	if (pin)
		cout << "да ";
	else
		cout << "нет";
	cout << endl;
}

void printColor(short color)	// выводит квадратик принятого цвета текста
{
	setTheme(color);
	cout << "  " << endl;
	setTheme();
}

int GoUp(short Up_Y, int sel)	// принимает верхнюю границу и текущий селектор
{
	if (getConcoleY() == Up_Y)	// вернет текущий селектор, если достигнута верхняя граница
		return sel;
	GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);	// иначе переместит курсор вверх
	return --sel;		// вернет селектор предыдущего элемента списка
}

int GoDown(short Down_Y, int sel)	// принимает нижнюю границу и текущий селектор
{
	if (getConcoleY() == Down_Y)	// вернет текущий, если достигнута нижняя граница
		return sel;
	GotoXY(WidthOfWindow() / 2, getConcoleY() + 1);	// иначе переместит курсор вниз
	return ++sel;		// вернет селектор следующего элемента
}

void GoLeft(short* mColor, bool* mBool, int sel)	// принимает массивы и селектор
{
	if (sel < sizeof(mColor) / 2)		// если селектор не вышел за первый массив
	{
		if (mColor[sel] > 0)			// пока цвет не достиг нижней границы палитры
			mColor[sel]--;				// уменьшаем код цвета
		printColor(mColor[sel]);		// сразу выведем изменения
		GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);		// вернем курсор на место
		return;
	}
	mBool[sel - sizeof(mColor) / 2] = !mBool[sel - sizeof(mColor) / 2];	// иначе изменяем 
	// следующий массив. просто делаем инверсию логических переменных, не зависимо от направления
	// вычтем из селектора размер первого массива
	printBool(mBool[sel - sizeof(mColor) / 2]);		// сразу выведем изменения
	GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);		// и вернем курсор
}

void GoRight(short* mColor, bool* mBool, int sel)	// аналогична движению влево
{
	if (sel < sizeof(mColor) / 2)
	{
		if (mColor[sel] < 15)		// но определяет верхнюю границу цветов
			mColor[sel]++;			// и идет к ней, пока не достигнет
		printColor(mColor[sel]);
		GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);
		return;
	}
	mBool[sel - sizeof(mColor) / 2] = !mBool[sel - sizeof(mColor) / 2];
	printBool(mBool[sel - sizeof(mColor) / 2]);
	GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);
}

void setSettings(short* mColor, bool* mBool)	// применим новые настройки
{
	mainColor = mColor[0];
	themeColor = mColor[1];
	drawing = mBool[0];
	rainbow = mBool[1];
	inverseDraw = mBool[2];
	drawAll = mBool[3];
	randomTheme = mBool[4];
}

void settings()			// пункт настроек, изменение цветов экрана и сохранение их в файле
{
	short settColor[] = { mainColor, themeColor };		// создадим массив для наших значений для удобства работы
	bool settBool[] = { drawing, rainbow, inverseDraw, drawAll, randomTheme };
	int selector = 0;		// селектор выбора значения из массива (подобно номеру списка запоминает
	// какую характеристику нам надо изменить
	setTheme();		// установим основную тему
	system("cls");

	center("Основной цвет: ", -1, 7);	// вывод меню настроек
	short Up_Y = getConcoleY();			// запоминаем верхнюю грацицу
	printColor(mainColor);
	center("Цвет темы: ", -1);
	printColor(themeColor);		// выводит цветной квадратик

	center("Включить цвета: ", -1);
	printBool(drawing);			// выводит булевое значение словом
	center("Включить радугу: ", -1);
	printBool(rainbow);
	center("Инертировать закрашивание: ", -1);
	printBool(inverseDraw);
	center("Закрашивать полностью: ", -1);
	printBool(drawAll);
	center("Случайные цвета темы: ", -1);

	short Down_Y = getConcoleY();		// запоминаем нижнюю границу
	printBool(randomTheme);

	GotoXY(WidthOfWindow() / 2, Up_Y);	// устанавливаем в первый пункт
	ShowCursor();			// нам надо видеть, где курсор, поэтому покажем его

	char key = _getch();	// получим первую команду

	while (key != '\r' && key != 27)	// пока не нажмем enter или ecape
	{
		key = _getch();

		switch (key)
		{
		case 77:
			GoRight(settColor, settBool, selector);
			break;
		case 75:
			GoLeft(settColor, settBool, selector);
			break;
		case 72:
			selector = GoUp(Up_Y, selector);
			break;
		case 80:
			selector = GoDown(Down_Y, selector);
			break;
		}
	}
	setSettings(settColor, settBool);		// сохраним настройки
	GotoXY(0, Down_Y + 1);		// поставим курсор вниз, чтобы продолжение не затерло меню настроек
}

bool loadSettings()
{
	ifstream file(settingsfile);
	if (file)
	{
		file >> mainColor;
		file >> themeColor;
		file >> drawing;
		file >> rainbow;
		file >> inverseDraw;
		file >> drawAll;
		file >> randomTheme;

		file.close();
		return true;
	}
	ofstream create(settingsfile);
	if (create)
	{
		create.close();
		return true;
	}
	return false;
}

bool saveSettings()
{
	ofstream file(settingsfile);
	if (file)
	{
		file << mainColor << " ";
		file << themeColor << " ";
		file << drawing << " ";
		file << rainbow << " ";
		file << inverseDraw << " ";
		file << drawAll << " ";
		file << randomTheme << " ";

		file.close();
		return true;
	}
	return false;
}

short table(const char* text, int col, short y)		// принимает строки, номер столбца и начальный отступ, в соответствие с этим выводит строку
{													// возвращает ширину столбца
	int x = al + ((WidthOfWindow() - al * 2) / cols) * col;	// определяет позицию X по заданному отступу, 
																		// количеству колонок и входящей заданной колонке
	if (!y)
		y = getConcoleY();			// eсли отступ высоты не задан, то использует текущий
	GotoXY(x, y);
	cout << text;
	return (WidthOfWindow() - al * 2) / cols;		// текущая ширина столбца
}

short parseInColumn(const char* text, short width, bool print)	// принимает строку для вывода и ширину поля, 
{								//а по ним выводит текст в определенном столбце и возвращает Y первой строки текста
	if (strlen(text) == 0)			// опасное исключение, с ним пришлось бороться около двух часов. дело в том, что при разбиении строки
		return getConcoleY();		// с помощью strtok_s при работе с нулевой строкой вылетает ошибка. а так как сравнение с пустотой 
									// не проходит, то приходится проверять длину строки, что уже вполне неплохо и выдает нам результат
	char* buf = 0;				// указатель для хранения оставшейся части строки
	char string[101];	// создание массива для работы с нашей строкой
	strcpy_s(string, text);		// копируем пришедшую строку сюда
	char* word = strtok_s(string, " ", &buf);	// инициализируем указатель и получаем первое слово строки
	int length = 0;		// длина строки для перемещений указателя
	short x = getConsoleX();		// запоминаем левую границу для нашего поля
	short y = getConcoleY();		// запоминаем номер сроки консоли для возврата

	while (word)	// выводим, пока строка не закончится
	{
		length = length + strlen(word) + 1;		// счетчик длины текущей строки

		if (length > width)		// если строка превышает заданную ширину
		{
			length = strlen(word);		// обнуляем текущую ширину строки, но запоминаем ширину последнего слова
			cout << endl;				// иначе выравнивание слетает. переносим строку
			GotoXY(x, getConcoleY());	// абзац
		}
		if (print)
			cout << word << " ";		// выводит, если это нам нужно
		word = strtok_s(0, " ", &buf);	// перемещаем указатель на следующее слово
	}
	return getConcoleY();		// вернуть текущий Y
}

bool checkDataBase(const char* filename)		// проверка существования и создание внешнего файла
{
	ifstream check(filename);
	if (check)
	{
		check.close();
		return true;
	}
	ofstream create(filename);
	if (create)
	{
		create.close();
		return true;
	}
	return false;
}


void pause()		// приостанавливает выполнение и выводит уведомление
{
	setTheme();
	cout << endl;
	center("Нажмите любую клавишу для продолжения");
	HideCursor();
	(void)_getch();
}

void printNull()		// выводит уведомление о том, что список пустой
{
	setTheme();
	system("cls");
	center("Ничего не найдено", 0, 1, true);
}

void setTableAlign(short align, short columns)
{
	al = align;		// горизонтальный отступ от левого края	для вывода базы данных
	cols = columns;		// количество колонок для вывода базы данных
}

void printRectangle(int length, int heigh)	// принимает размер окна и цвет, затем выводит прямоугольную область по центру
{

	GotoXY((WidthOfWindow() - length) / 2, (HeightOfWindow() - heigh) / 2);
	for (int y = 0; y < heigh; y++)
	{
		for (int x = 0; x < length; x++)
		{
			cout << " ";
		}
		cout << endl;
		GotoXY((WidthOfWindow() - length) / 2, getConcoleY());
	}
}