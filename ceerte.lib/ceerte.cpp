
#include <Windows.h>
#include "ceerte.h"
#include <iostream>
#include <fstream>
#include <conio.h>

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
using namespace std;

char settingsfile[] = "settings.txt";

short mainColor = 0;		// �������� ���� ����. ���� ������ �������� �������� (������ ��� �����)
short themeColor = 0;	// ���� ������������ ���������

bool drawing = false;		// �������� ��������� ������
bool rainbow = false;		// �������� ������������ ���������
bool inverseDraw = false;// ������������ �������� ������ ������
bool drawAll = false;		// ������������ ������ �������
bool randomTheme = false;
short doubleColor = mainColor;		// ������ ������� ���� �������� �������

short al = 0;		// �������������� ������ �� ������ ����	��� ������ ���� ������
int cols = 1;		// ���������� ������� ��� ������ ���� ������


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
{		// ������������ ������ �� ������
	if (!x)
		x = (WidthOfWindow() - strlen(text)) / 2;	// ���� ������ ������ �� �����, ���������� �����	
	else if (x == -1)
		x = x = WidthOfWindow() / 2 - strlen(text);	// ���� ������ -1, �������� ����� �� ������ ������
	else
		x = WidthOfWindow() / 2 - x;		// ����� ��������� ��������� ����� �� �������� �����

	if (!y)
		y = getConcoleY();			// e��� ������ ������ �� �����, �� ���������� �������
	else
		y = (HeightOfWindow() - y) / 2;

	GotoXY(x, y);			// ���������� ��������� � �������� ���������

	if (!drawing || color == mainColor)
		cout << text;
	else
		logicOfDraw(text, color, all, inverse);		// ����� ���������� ��� �������, �������� � ���������

	if (line)		//  ���������� �������� �� �������
		cout << endl;

	doubleColor = color;
};


void logicOfDraw(const char* text, short color, bool all, bool inverse)		// ������ ��������� ����� ������
{
	if (rainbow)				// ���� �������� �������� ���������, ���������� ���� ��������� ����
		color = randomColor();

	if (all)		// ���� ������ ��� ������� ���������, �� ��������� ���� � ������
	{
		setTheme(color);
		cout << text;
		return;
	}

	if (inverse)		// ���� ��������, �� ������� ��������� ��������� �����, � ��������� ����� ����
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

short rainbowTable(short startY, short finishY, short leftX, short rightX)		// ��������� �������� ����� � ������� ��� ����������, ������������ �� � ���������� ��������� ������������ ������
{
	if (startY > finishY)		// ���� �� ����� ����������� ������� �������� ������, �� ������ �������
		swap(startY, finishY);

	GotoXY(0, startY);		// ��������� ��������� � ������ �������

	for (int i = startY; i <= finishY; i++)		// ������� ������, ���� �� ��������
	{
		for (int j = 0; j < WidthOfWindow(); j++)		// �����������, ���� �� ����� ����
		{
			if (j >= leftX && rightX != 0 && j < rightX)		// ���������, ���� ������ ����� �� ������� ���������� � ������� ������,
			{
				j = rightX + 1;
				GotoXY(j, i);			//  �� ��������� ��
			}
			cout << " ";
		}
		cout << endl;
	}
	return startY;		// ���������� �������� Y
}

short randomColor()		// ������������� ��������� ����� ���� � ������������ ����� ������
{
	if (!randomTheme)
		return mainColor;
	short color = rand() % 0x0F;		// ������ ����� ���� �� 16
	if (color == mainColor || color == 0x06 || color == doubleColor)	// ��������� ���������� � �������� ������, ���������� � ��������� ����																		
		return randomColor();					// ���� ������� �������, �� ���������� �� ������

	setFontColor(color);
	doubleColor = color;			// ���������� ��� ���� ��� ���������� ����������
	SetTextBackground(color);		// � ������� ������������� ���� ����

	return color;
}

void setFontColor(short color)	// ��������� ���� ����, ��������� � ������������� ����������� ���� ������
{
	SetTextColor(0x0f);			// ��� ���� ����� ����� �����
	if (color == 0x07 || color == 0x0b || color == 0x0e || color == 0x0a || color == 0x0f)
		SetTextColor(0);		// ��� ��������� ������ ���� ������������� ������ ���� ������ ��� ����������
}

short setTheme(short color)		// ������������� ����� �������� ����
{
	SetTextBackground(color);	// ��������� ��������� ����� ����
	setFontColor(color);		// ��������� ��������� ����� ������
	doubleColor = color;	// ����� ������� ������� ���� ����
	return color;
}

short inverseColor(short color)	// ��������� ���� ����, ������������� ��������������� ���
{
	SetTextBackground(0x0f - color);	// �������� ����
	setFontColor(0x0f - color);		// ���������� ���� ������
	doubleColor = 0x0f - color;		// ��������� ���������
	return doubleColor;
}

short setColor()	// ������������� ���� ����� �� ��������
{
	if (!drawing)		// ���� ��������� ���������, �� ������� �����������
		return setTheme();

	if (rainbow)	// ���� �������� �������� ���������, ������ ����� ����
		return randomColor();

	if (themeColor != mainColor && !randomTheme)	// ���� ���� ���� �� ���������� �� ��������, ������ ���
		setTheme(themeColor);

	return doubleColor;		// ����� ������ ����������
}

void printBool(bool pin)	// ������� ��������� �������� ������ �����������
{
	if (pin)
		cout << "�� ";
	else
		cout << "���";
	cout << endl;
}

void printColor(short color)	// ������� ��������� ��������� ����� ������
{
	setTheme(color);
	cout << "  " << endl;
	setTheme();
}

int GoUp(short Up_Y, int sel)	// ��������� ������� ������� � ������� ��������
{
	if (getConcoleY() == Up_Y)	// ������ ������� ��������, ���� ���������� ������� �������
		return sel;
	GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);	// ����� ���������� ������ �����
	return --sel;		// ������ �������� ����������� �������� ������
}

int GoDown(short Down_Y, int sel)	// ��������� ������ ������� � ������� ��������
{
	if (getConcoleY() == Down_Y)	// ������ �������, ���� ���������� ������ �������
		return sel;
	GotoXY(WidthOfWindow() / 2, getConcoleY() + 1);	// ����� ���������� ������ ����
	return ++sel;		// ������ �������� ���������� ��������
}

void GoLeft(short* mColor, bool* mBool, int sel)	// ��������� ������� � ��������
{
	if (sel < sizeof(mColor) / 2)		// ���� �������� �� ����� �� ������ ������
	{
		if (mColor[sel] > 0)			// ���� ���� �� ������ ������ ������� �������
			mColor[sel]--;				// ��������� ��� �����
		printColor(mColor[sel]);		// ����� ������� ���������
		GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);		// ������ ������ �� �����
		return;
	}
	mBool[sel - sizeof(mColor) / 2] = !mBool[sel - sizeof(mColor) / 2];	// ����� �������� 
	// ��������� ������. ������ ������ �������� ���������� ����������, �� �������� �� �����������
	// ������ �� ��������� ������ ������� �������
	printBool(mBool[sel - sizeof(mColor) / 2]);		// ����� ������� ���������
	GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);		// � ������ ������
}

void GoRight(short* mColor, bool* mBool, int sel)	// ���������� �������� �����
{
	if (sel < sizeof(mColor) / 2)
	{
		if (mColor[sel] < 15)		// �� ���������� ������� ������� ������
			mColor[sel]++;			// � ���� � ���, ���� �� ���������
		printColor(mColor[sel]);
		GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);
		return;
	}
	mBool[sel - sizeof(mColor) / 2] = !mBool[sel - sizeof(mColor) / 2];
	printBool(mBool[sel - sizeof(mColor) / 2]);
	GotoXY(WidthOfWindow() / 2, getConcoleY() - 1);
}

void setSettings(short* mColor, bool* mBool)	// �������� ����� ���������
{
	mainColor = mColor[0];
	themeColor = mColor[1];
	drawing = mBool[0];
	rainbow = mBool[1];
	inverseDraw = mBool[2];
	drawAll = mBool[3];
	randomTheme = mBool[4];
}

void settings()			// ����� ��������, ��������� ������ ������ � ���������� �� � �����
{
	short settColor[] = { mainColor, themeColor };		// �������� ������ ��� ����� �������� ��� �������� ������
	bool settBool[] = { drawing, rainbow, inverseDraw, drawAll, randomTheme };
	int selector = 0;		// �������� ������ �������� �� ������� (������� ������ ������ ����������
	// ����� �������������� ��� ���� ��������
	setTheme();		// ��������� �������� ����
	system("cls");

	center("�������� ����: ", -1, 7);	// ����� ���� ��������
	short Up_Y = getConcoleY();			// ���������� ������� �������
	printColor(mainColor);
	center("���� ����: ", -1);
	printColor(themeColor);		// ������� ������� ���������

	center("�������� �����: ", -1);
	printBool(drawing);			// ������� ������� �������� ������
	center("�������� ������: ", -1);
	printBool(rainbow);
	center("������������ ������������: ", -1);
	printBool(inverseDraw);
	center("����������� ���������: ", -1);
	printBool(drawAll);
	center("��������� ����� ����: ", -1);

	short Down_Y = getConcoleY();		// ���������� ������ �������
	printBool(randomTheme);

	GotoXY(WidthOfWindow() / 2, Up_Y);	// ������������� � ������ �����
	ShowCursor();			// ��� ���� ������, ��� ������, ������� ������� ���

	char key = _getch();	// ������� ������ �������

	while (key != '\r' && key != 27)	// ���� �� ������ enter ��� ecape
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
	setSettings(settColor, settBool);		// �������� ���������
	GotoXY(0, Down_Y + 1);		// �������� ������ ����, ����� ����������� �� ������� ���� ��������
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

short table(const char* text, int col, short y)		// ��������� ������, ����� ������� � ��������� ������, � ������������ � ���� ������� ������
{													// ���������� ������ �������
	int x = al + ((WidthOfWindow() - al * 2) / cols) * col;	// ���������� ������� X �� ��������� �������, 
																		// ���������� ������� � �������� �������� �������
	if (!y)
		y = getConcoleY();			// e��� ������ ������ �� �����, �� ���������� �������
	GotoXY(x, y);
	cout << text;
	return (WidthOfWindow() - al * 2) / cols;		// ������� ������ �������
}

short parseInColumn(const char* text, short width, bool print)	// ��������� ������ ��� ������ � ������ ����, 
{								//� �� ��� ������� ����� � ������������ ������� � ���������� Y ������ ������ ������
	if (strlen(text) == 0)			// ������� ����������, � ��� �������� �������� ����� ���� �����. ���� � ���, ��� ��� ��������� ������
		return getConcoleY();		// � ������� strtok_s ��� ������ � ������� ������� �������� ������. � ��� ��� ��������� � �������� 
									// �� ��������, �� ���������� ��������� ����� ������, ��� ��� ������ ������� � ������ ��� ���������
	char* buf = 0;				// ��������� ��� �������� ���������� ����� ������
	char string[101];	// �������� ������� ��� ������ � ����� �������
	strcpy_s(string, text);		// �������� ��������� ������ ����
	char* word = strtok_s(string, " ", &buf);	// �������������� ��������� � �������� ������ ����� ������
	int length = 0;		// ����� ������ ��� ����������� ���������
	short x = getConsoleX();		// ���������� ����� ������� ��� ������ ����
	short y = getConcoleY();		// ���������� ����� ����� ������� ��� ��������

	while (word)	// �������, ���� ������ �� ����������
	{
		length = length + strlen(word) + 1;		// ������� ����� ������� ������

		if (length > width)		// ���� ������ ��������� �������� ������
		{
			length = strlen(word);		// �������� ������� ������ ������, �� ���������� ������ ���������� �����
			cout << endl;				// ����� ������������ �������. ��������� ������
			GotoXY(x, getConcoleY());	// �����
		}
		if (print)
			cout << word << " ";		// �������, ���� ��� ��� �����
		word = strtok_s(0, " ", &buf);	// ���������� ��������� �� ��������� �����
	}
	return getConcoleY();		// ������� ������� Y
}

bool checkDataBase(const char* filename)		// �������� ������������� � �������� �������� �����
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


void pause()		// ���������������� ���������� � ������� �����������
{
	setTheme();
	cout << endl;
	center("������� ����� ������� ��� �����������");
	HideCursor();
	(void)_getch();
}

void printNull()		// ������� ����������� � ���, ��� ������ ������
{
	setTheme();
	system("cls");
	center("������ �� �������", 0, 1, true);
}

void setTableAlign(short align, short columns)
{
	al = align;		// �������������� ������ �� ������ ����	��� ������ ���� ������
	cols = columns;		// ���������� ������� ��� ������ ���� ������
}

void printRectangle(int length, int heigh)	// ��������� ������ ���� � ����, ����� ������� ������������� ������� �� ������
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