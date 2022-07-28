
#include <iostream>
#include <fstream>
#include <conio.h>
#include <vector>
#include <map>
#include <ceerte.h>
#include <windows.h>
#include <string>

using namespace std;

char externalfile[] = "database.txt";		// файл хранения базы данных
char samplefile[] = "samples.txt";	// файл хранения шаблонов дисциплин

LPCWSTR windowName = L"Contract Manager";	// имя окна

bool FULL = true;		// запуск полного экрана

extern short doubleColor;	// последний использованный цвет

short menuY = 8;		// вертикальный отступ от середины для окна меню (количество пунктов меню)
short menuX = 16;		// горизонтальный отступ от середины для окна меню

short tableAlign = 8;			// боковой отступ вывода таблиц
short tableVertical = 6;		// вертикальный отступ вывода таблиц

short mainTableColumns = 6;		// количество столбцов для основной таблицы
short mainMenuColumn = 4;		// столбец функций основной таблицы
short mainInfoColumn = 1;		// столбец информации о студентах
short mainSearchColumn = 0;		// столбец вывода поиска

short studentTableColumns = 3;		// количество столбцов меню редактирования
short studentMenuColumn = 2;		// номер колонки дл вывода функцию редактирования
short studentScoreColumn = 1;		// номер колонки для вывода оценок студента
short studentTemplateColumn = 1;	// номер колонки для вывода имени текущего шаблона

short warningHeigh = 8;
short warningLength = 40;

bool sampleActive = false;		// храним состояние работы с шаблонами
string sample = "";				// текущий шаблон
string finding = "";			// текущая строка поиска

int sleepTimer = 1500;
int maxLength = 0;	// ширина поля вывода информации (высчитывает ширину между именем и функциями)

int minScore = 2;	// минимальная оценка
int maxScore = 5;	// максимальная оценка

struct discipline		// храним название дисциплины и оценку за нее
{
	string subject = "unknown";				// безымянный
	int score = (double)maxScore * 0.8;		// устанавливает часто встречающуюся оценку
};

vector <discipline> vectorbuf;			// вектор для хранения временных данных
map <string, vector<string>> samples;		// храним шаблоны дисциплин, first - название шаблона, second - дисциплины
multimap <string, vector <discipline>> list;	// хранение всех данных о студенте. string - имя студента, vector - дисциплины
multimap <string, vector <discipline>> findbuf;	// сюда будем записывать все найденные элементы. 
// Мне показалось, что так программа будет работать быстрее и оптимальнее (точнее это единственный путь реализации поиска)

string sampleEditor(string name);
bool importStudents();
bool exportStudents();
bool importSamples();
bool exportSamples();
void clean();		// ставит стандартную тему и чистит экран

string nameException()			// здесь мы будем проверять пользовательский ввод
{
	string name;
	ShowCursor();
	while (name == "")
		getline(cin, name);
	HideCursor();
	return name;
}

void printStudent(multimap <string, vector <discipline>>::iterator i)
{
	clean();
	setTableAlign(tableAlign, studentTableColumns);

	// сделать отдельный вывод меню, чтобы выводить его один раз при входе в редакторы или при выводе всего листа, например
	table("1. Добавить дисциплину\n\n", studentMenuColumn, tableVertical);
	table("2. Включить/выключить шаблон\n\n", studentMenuColumn);	// если список дисциплин пуст
	table("3. Изменить имя студента\n\n", studentMenuColumn);
	table("4. Удалить все дисциплины\n\n", studentMenuColumn);
	table("5. Удалить студента\n\n", studentMenuColumn);
	table("Enter. Редактировать название выбранной дисциплины\n\n", studentMenuColumn);
	table("Del. Удалить выбранную дисциплину\n\n", studentMenuColumn);
	table("Esc. Сохранить и выйти\n\n", studentMenuColumn);
	table("Стрелки Вверх и Вниз чтобы выбрать дисциплину\n\n", studentMenuColumn);
	table("Стрелки Влево и Вправо чтобы изменить оценку", studentMenuColumn);
	// пропарсить один раз вместо кучи таблиц
	table("", 0, tableVertical - 2);
	cout << i->first;		// выводим имя в верхнем левом углу

	if (sampleActive)
	{
		table("", studentTemplateColumn);	// выводим текущий шаблон
		cout << sample;		//
	}

	table("", 0, tableVertical);		//	ставим в начало таблицы

	if (!i->second.empty())		// если есть дисциплины, то выводит их 
	{
		for (auto j : i->second)
		{
			table("");
			cout << j.subject;
			table("", studentScoreColumn);
			cout << j.score << endl;
		}
	}
}

multimap <string, vector <discipline>>::iterator changeStudentName(multimap <string, vector <discipline>>::iterator i)	// изменение имени студента
{
	clean();
	center("Введите новое имя студента: ", -1, 1);

	vectorbuf = i->second;		// сохраняем вектор
	list.erase(i);			// удаляем текущее слово

	i = list.insert(make_pair(nameException(), vectorbuf));		// создаем слово с новым именем
	vectorbuf.clear();		// чистим вектор

	return i;	// возвращаем новый указатель для продолжения работы с ним
}

bool deleteStudent(multimap <string, vector <discipline>>::iterator i)	// запрос подтверждения на удаление
{
	printRectangle(warningLength, warningHeigh);		// закрашиваем область уведомления

	center("Вы действительно хотите удалить", 0, 2);
	GotoXY((WidthOfWindow() - i->first.length()) / 2, getConcoleY() + 1);	// выводим имя по центру

	cout << i->first << "?";

	center("1. Да", 9, -4);
	center(" 2. Нет", -3, -4);

	char key = _getch();

	if (key == '1')		// если да, удаляем
	{
		list.erase(i);	// удаляет студента из словаря	
		return 1;	// истина, если элемент был удален
	}
	return 0;
}

void drawStudentLine(int line, vector <discipline> container, short color)	// принимает указатель,
{												// вектор и цвет, затем закрашивает указанную линию
	if (container.empty())	// выходим, если вектор пустой
	{
		setTheme();		// не забываем поставить тему обратно
		return;
	}

	setTheme(color);
	rainbowTable(line + tableVertical, line + tableVertical, table("") * studentMenuColumn + tableAlign - 1, WidthOfWindow());
	// закрашиваем указанную строчку до списка функций
	table("", 0, line + tableVertical);
	cout << container[line].subject;
	table("", studentScoreColumn);
	cout << container[line].score;
	setTheme();
}

bool deleteAccept()		// запрашивает подтверждение удаления
{
	printRectangle(warningLength, warningHeigh);
	center("Подтвердите удаление", 10, 2, true);
	center("1. Удалить", 13, -2, true);
	center("2. Отмена", -5, -2, true);

	char key = _getch();
	if (key == '1')		// если подтверждено
		return 1;
	return 0;
}

bool actionResult(bool result)
{
	printRectangle(warningLength, warningHeigh);

	if (result)
		center("Выполненение...", 5, 2, true);
	else
		center("Отмена...", 5, 2, true);
	Sleep(sleepTimer);		// сон 
	return result;
}

bool setTemplate(multimap <string, vector <discipline>>::iterator i, string name)		// логически заполняет дисциплины стуента шаблоном
{
	if (samples.empty())			// если список пуст, то выключаем подстановку
	{
		sample = "";
		return false;
	}
	else if (sample == "")		// если элемент был удален, то берем первый шаблон
	{
		sample = samples.begin()->first;
		name = sample;
	}
	auto s = samples.find(name);	// если у нас уже есть шаблон, то перемещаем итератор на него


	if (sampleActive)		// если подстановка шаблона активна, то сохраняем текущий вектор в буфере и вставляем шаблон
	{
		discipline buf;

		i->second.clear();		// чистим текущий вектор
		for (auto j : s->second)
		{
			buf.subject = j;
			i->second.push_back(buf);	// заполняем вектором из шаблона
		}
	}
	return sampleActive;	// возвращает текущее состояние подстановки
}

void drawSampleSelect(string name, short color)
{
	setTheme(color);
	auto i = samples.find(name);
	rainbowTable(tableVertical - 2, tableVertical - 2, 0, table("") * studentTemplateColumn + tableAlign);		// закрасить область
	table("", studentTemplateColumn, tableVertical - 2);
	cout << i->first;
	setTheme();
}

string selectSample(string name)		// смена шаблона
{
	if (samples.empty())
		return "";

	char key;
	auto i = samples.find(name);

	while (true)
	{
		drawSampleSelect(i->first, setColor());
		key = _getch();

		switch (key)
		{
		case 75:		// влево
			if (i != samples.begin())	// начальная граница
				i--;
			break;

		case 77:		// вправо
			if (i != --samples.end())	// конечная граница
				i++;
			break;

		case 80:
		case 27:		// выход
			return i->first;

		case 13:		// sample edit
			return sampleEditor(i->first);		// сразу редактируем шаблон, если нужно

		default:
			break;
		}
	}
}

void StudentEditor(multimap <string, vector <discipline>>::iterator i)
{
	vectorbuf = i->second;	// сохраняем текущий вектор. Предположительно, он будет храниться, пока мы не выйдем из редактирования
	setTemplate(i, sample);	// если включен шаблон, то использует его
	char key;		// значения с клавиатуры

start:
	discipline add;		// рабочая структура
	int line = 0;		// хранит указатель вектора
	auto j = i->second.begin();	// итератор вектора, понадобился для удаления

	printStudent(i);

	while (true)
	{
		if (!i->second.empty())		// вектор не пустой
			drawStudentLine(line, i->second, setColor());

		key = _getch();

		switch (key)
		{
		case '1':		// добавляем дисциплину студенту
			clean();

			// надо сделать проверку на уникальность имени дисциплины

			center("Введите название дисциплины: ", -1, 2);
			add.subject = nameException();

			center("Стрелки Влево и Вправо чтобы изменить оценку", 22, -6);
			center("Enter. Сохранить", 15, -10);
			center("Esc. Выйти", -5);
			center("Введите оценку дисциплины: ", -1, -2);

			while (true)
			{
				cout << "          ";	// закрашиваем старое значение
				center("", 0, -2);

				cout << " " << add.score;	// выводим новое
				key = _getch();
				if (key == 13)		// Enter, то выходим из цикла к добавлению дисциплины
					break;

				switch (key)
				{
				case 27:
					goto start;

				case 75:		//left
					if (add.score != minScore)	// если оценка не минимальна, то декрементируем
						add.score--;
					break;

				case 77:		//right
					if (add.score != maxScore)	// если оценка не максимальна, то инкрементируем
						add.score++;
					break;

				default:
					break;
				}
			}

			i->second.push_back(add);
			vectorbuf = i->second;		// обновляем буфер
			goto start;

		case '2':		// выключить автозаполнение шаблонов

			sampleActive = !sampleActive;		// инвертируем логическое значение

			if (!sampleActive)		// если шаблоны уже были вставлены
				i->second = vectorbuf;		//то возвращаем сохраненный вектор обратно
			else
				setTemplate(i, sample);			// иначе ставим шаблон	
			goto start;

		case '3':	// меняет имя студента
			i = changeStudentName(i);
			goto start;

		case '5':		// удалить студента
			if (actionResult(deleteStudent(i)))
				return;
			goto start;

		case 27:		// Escape - выход из редактора
			return;

		default:
			break;
		}

		if (!i->second.empty())	// выполняем, если вектор не пуст
			switch (key)
			{
			case '4':		// удалить все дисциплины студента

				if (actionResult(deleteAccept()))	// запрашивает подтверждение и выводит результат
				{
					i->second.clear();	// чистит вектор
					vectorbuf.clear();		// обновляем буфер
				}
				goto start;

			case 72:	// кнопка вверх, перемещение вверх по списку

				drawStudentLine(line, i->second, setTheme());	// убираем раскраску текущего
				if (line != 0)	// если итератор не на первой дисциплине
				{
					line--;		// перемещаем указатель вверх
					j--;
					drawStudentLine(line, i->second, setColor());	// раскрашиваем верхний
				}
				else
				{
					if (sampleActive)
					{
						//s = samples.find(selectSample(sample));
						sample = selectSample(sample);
						setTemplate(i, sample);
						goto start;
					}
				}
				break;

			case 80:	// кнопка вниз, перемещение вниз по списку

				if (line == i->second.size() - 1)	// если итератор на последнем студенте
					break;

				drawStudentLine(line, i->second, setTheme());// убираем раскраску текущего
				line++;
				j++;
				drawStudentLine(line, i->second, setColor());	// раскрашиваем нижний
				break;

			case 75:		// 75 == left // меняем оценку

				if (j->score != minScore)
				{
					j->score--;
					vectorbuf = i->second;		// обновляем буфер
				}
				break;

			case 77:		// 77 == right

				if (j->score != maxScore)
				{
					j->score++;
					vectorbuf = i->second;		// обновляем буфер
				}
				break;

			case 83:	// Delete	
			case 8:		// backspace

				if (actionResult(deleteAccept()))	// запрашивает подтверждение и выводит результат
				{
					i->second.erase(j);			// удаляет выбранную дисциплину
					vectorbuf = i->second;		// обновляем буфер
				}
				goto start;

			case 13:	// Enter - изменить название дисциплины
				clean();
				center("Введите новое название дисциплины: ", -1, 1);
				// сделать редактирование старого значения
				j->subject = nameException();		// сделать проверку уникальности дисциплины
				goto start;

			default:
				break;
			}
	}
	vectorbuf.clear();		// чистим вехтор (подбираем за собой, да и чего ему в памяти хламиться)
}

bool addStudentScene()		// добавляет имя нового студента и переходит к редактору
{
	clean();

	center("Введите имя студента: ", -1, 1);		// запрашиваем имя нового студента

	auto i = list.insert(pair <string, vector <discipline>>(nameException(), NULL));		// добавляет студента без дисциплин в наш словарь
	StudentEditor(i);
	return 1;
}

void parseSampleDisciplines(vector <string> i, int j)	// выводит краткую строку дисциплин шаблона
{
	if (!i.empty())		// если наш вектор вообще существует, то выводим его в ограниченное поле
	{
		int length = i[0].length() + 5;	// получаем длину первой дисциплины
		int k = 0;			// простой итератор для вектора
		while (length < j)		// пока длина строки не превышает ширину столбца
		{
			cout << i[k];

			if (k == i.size() - 1)	// если вектор закончился, то выводим последний и выходим				
				break;

			length += i[k + 1].length() + 5;		// проверяем длину следующего. Если следующий
															// не влезает, то выводим троеточие в конце
			if (length < j)
				cout << ", ";
			else
				cout << "...";
			k++;		// не забываем переместить указатель				
		}

	}
}

void printSampleList()
{
	clean();
	setTableAlign(tableAlign, mainTableColumns);

	table("1. Добавить шаблон\n\n", mainMenuColumn, tableVertical);
	table("2. Изменить имя шаблона\n\n", mainMenuColumn);
	table("Enter. Редактировать выбранный шаблон\n\n", mainMenuColumn);
	table("Del. Удалить выбранный шаблон\n\n", mainMenuColumn);
	table("Стрелки Вверх и Вниз чтобы выбрать шаблон\n\n", mainMenuColumn);
	table("Esc. Сохранить и выйти\n\n", mainMenuColumn);

	table("Поиск:", mainSearchColumn, tableVertical - 4);
	table("Название шаблона", 0, tableVertical - 2);
	table("Дисциплины", mainInfoColumn);

	table("", 0, tableVertical);

	for (auto i : samples)	// вывод всей таблицы шаблонов
	{
		table("");
		cout << i.first;
		table("", mainInfoColumn);

		parseSampleDisciplines(i.second, maxLength);

		cout << endl;
	}
}

void printSample(string name)
{
	clean();
	setTableAlign(tableAlign, studentTableColumns);

	table("", 0, tableVertical - 2);
	cout << name;

	table("1. Добавить дисциплину\n\n", studentMenuColumn, tableVertical);
	table("2. Изменить имя шаблона\n\n", studentMenuColumn);
	table("3. Удалить шаблон\n\n", studentMenuColumn);
	table("4. Удалить все дисциплины\n\n", studentMenuColumn);
	table("Enter. Изменить дисциплину\n\n", studentMenuColumn);
	table("Del. Удалить дисциплину\n\n", studentMenuColumn);
	table("Стрелки Вверх и Вниз чтобы выбрать дисциплину", studentMenuColumn);

	table("", 0, tableVertical);

	if (!samples[name].empty())		// если наш вектор вообще существует, то выводим его в виде списка
	{
		for (auto j : samples[name])
		{
			table("", 0);
			cout << j << endl;
		}
	}
}

void drawSampleSelected(string disc, int line, short color)
{

	setTheme(color);
	rainbowTable(line + tableVertical, line + tableVertical, table("") * studentMenuColumn + tableAlign - 1, WidthOfWindow());
	// закрашиваем указанную строчку до списка функций
	table("", 0, line + tableVertical);
	cout << disc;
	setTheme();

}

string sampleEditor(string name)
{
	char key;
start:
	int line = 0;
	printSample(name);
	while (true)

	{
		if (!samples[name].empty())
			drawSampleSelected(samples[name][line], line, setColor());		//вывод строк

		key = _getch();

		switch (key)
		{
		case '1':		// добавление дисциплины
			clean();
			center("Введите название дисциплины: ", -1, 1);
			samples[name].push_back(nameException());
			goto start;

		case '3':		// удаление шаблона
			if (actionResult(deleteAccept()))	// если удален, то обновляем список
			{
				samples.erase(name);	// удаляем шаблон и выходим
				sample = "";	// удаляем последний использованный шаблон
				return sample;
			}
			printSample(name);	// если шаблон не удален, печатаем лист заново
			break;

		case 27:		// выход
			return name;

		default:
			break;
		}

		if (!samples[name].empty())
			switch (key)
			{
				if (!samples[name].empty())
			case '2':		// изменение имени шаблона
				clean();
				center("Введите новое имя шаблона: ", -1, 1);
				samples[nameException()] = samples[name];	// создает слово с новым именем и присваивает предыдущий вектор
				samples.erase(name);	// удаляем шаблон и выходим
				sample = "";	// удаляем последний использованный шаблон
				return sample;

			case 13:		// Enter редактировать дисциплину
				clean();
				center("Введите новое название дисциплины: ", -1, 1);
				samples[name][line] = nameException();
				goto start;

			case 72:	// кнопка вверх, перемещение вверх по списку

				drawSampleSelected(samples[name][line], line, setTheme());		// убираем раскраску текущего
				if (line != 0)					// если указатель не на первой дисциплине
				{
					line--;
					drawSampleSelected(samples[name][line], line, setColor());		// раскрашиваем верхний
				}
				break;

			case 80:	// кнопка вниз, перемещение вниз по списку

				if (line == samples[name].size() - 1)	// если указатель на последней дисциплине
					break;

				drawSampleSelected(samples[name][line], line, setTheme());		// убираем раскраску текущего

				line++;
				drawSampleSelected(samples[name][line], line, setColor());		// раскрашиваем нижний
				break;

			case 83:	// Delete	
			case 8:		// backspace
				if (actionResult(deleteAccept()))	// если удален, то обновляем список
				{
					auto i = samples[name].begin();		// ставим итератор в начало
					samples[name].erase(i + line);		// и наконец удаляем как и полагается		
					goto start;
				}
				printSample(name);	// если дисциплина не удалена, печатаем лист заново
				break;

			case '4':
				if (actionResult(deleteAccept()))	// если удален, то обновляем список
				{
					samples[name].clear();	// удаляем все дисциплины
					goto start;
				}
				printSample(name);	// если дисциплины не удалена, печатаем лист заново
				break;

			default:
				break;
			}
	}
}

bool addSampleScene()	// добавляет новый шаблон
{
	clean();

	center("Введите имя шаблона: ", -1, 1);
	string name = nameException();
	// сделать проверку на уникальность
	samples[name];	// добавляет новый пустой шаблон в словарь

	auto i = samples[name];

	if (samples[name].empty())	// если добавлен новый (точнее если нет элементов)
	{
		sampleEditor(name);		//переходит к редактированию
		return true;
	}
	return false;
}

void drawSampleLine(string name, int line, short color)
{
	line += tableVertical;
	setTheme(color);
	rainbowTable(line, line, table("") * mainMenuColumn + tableAlign - 1, WidthOfWindow());
	// закрашиваем указанную строчку до списка функций
	table("", 0, line);
	cout << name;
	table("", mainInfoColumn);
	parseSampleDisciplines(samples[name], maxLength);	// выводим что было
	setTheme();

}

void mainSampleList()	// редактор шаблонов
{
	char key;
start:
	auto i = samples.begin();	// ставит указатель в начало списка
	int line = 0;				// указатель для закраски строк

	printSampleList();

	while (true)
	{

		if (!samples.empty())
			drawSampleLine(i->first, line, setColor());		// закрасить линию

		key = _getch();

		switch (key)
		{
		case '1':		// добавить шаблон
			if (!addSampleScene())
			{
				printSampleList();
				printRectangle(warningLength, warningHeigh);
				center("Шаблон с таким именем уже существует", 18, 1, true);
				cout << endl;
				pause();
			}
			goto start;

		case 27:		// Escape - выход из редактора шаблонов
			return;

		default:
			break;
		}

		if (!samples.empty())	// если лист шаблонов не пустой, то редактируем
			switch (key)
			{
			case '2':		// переименовать шаблон
				clean();
				center("Введите новое имя шаблона: ", -1, 1);
				samples[nameException()] = i->second;	// создает слово с новым именем и присваивает предыдущий вектор
				samples.erase(i);
				goto start;

			case 72:	// кнопка вверх, перемещение вверх по списку

				drawSampleLine(i->first, line, setTheme());		// убираем раскраску текущего
				if (i != samples.begin())					// если итератор не на первом шаблоне
				{
					i--;
					line--;
					drawSampleLine(i->first, line, setColor());		// раскрашиваем верхний
				}
				break;

			case 80:	// кнопка вниз, перемещение вниз по списку

				if (i == --samples.end())	// если итератор на последнем студенте
					break;

				drawSampleLine(i->first, line, setTheme());		// убираем раскраску текущего
				i++;
				line++;
				drawSampleLine(i->first, line, setColor());		// раскрашиваем нижний
				break;

			case 83:	// Delete	
			case 8:		// backspace
				if (actionResult(deleteAccept()))	// если удален, то обновляем список
				{
					samples.erase(i);	// удаляем выбранный шаблон
					sample = "";	// удаляем последний использованный шаблон
					goto start;
				}
				printSampleList();	// если шаблон не удален, печатаем лист заново
				break;


			case 13:	// редактировать шаблон
				sampleEditor(i->first);
				goto start;

			default:
				break;
			}
	}
}

void parseInShortString(vector <discipline> info, int j)	// принимает вектор и ширину для вывода всех
{																	// элементов в строку заданной ширины
	table("", mainInfoColumn);

	if (!info.empty())		// если наш вектор вообще существует, то выводим его в ограниченное поле
	{
		int length = info[0].subject.length() + 5;	// получаем длину первой дисциплины
		int k = 0;			// простой итератор для вектора
		while (length < j)		// пока длина строки не превышает ширину столбца
		{
			cout << info[k].subject << " " << info[k].score;

			if (k == info.size() - 1)	// если вектор закончился, то выводим последний и выходим
				break;

			length += info[k + 1].subject.length() + 5;		// проверяем длину следующего. Если следующий
															// не влезает, то выводим троеточие в конце
			if (length < j)
				cout << ", ";
			else
				cout << "...";
			k++;		// не забываем переместить указатель				
		}
	}
}

void printMainList()		// основной вывод программы
{
	clean();
	setTableAlign(tableAlign, mainTableColumns);

	table("1. Добавить студента\n\n", mainMenuColumn, tableVertical);
	table("2. Редактировать шаблоны\n\n", mainMenuColumn);
	table("3. Изменить имя студента\n\n", mainMenuColumn);
	table("4. Поиск студентов\n\n", mainMenuColumn);
	table("5. Настройки\n\n", mainMenuColumn);
	table("Enter. Редактировать выбранного студента\n\n", mainMenuColumn);
	table("Del. Удалить выбранного студента\n\n", mainMenuColumn);
	table("Стрелки Вверх и Вниз чтобы выбрать студента\n\n", mainMenuColumn);

	if (finding == "")
		table("Esc. Выход\n\n", mainMenuColumn);
	else
		table("Esc. Закрыть поиск\n\n", mainMenuColumn);

	table("Поиск:", mainSearchColumn, tableVertical - 4);
	table("", mainSearchColumn + 1, tableVertical - 4);
	cout << finding;

	table("Имя студента", 0, tableVertical - 2);
	table("Дисциплины", mainInfoColumn);

	maxLength = table("", mainInfoColumn) * (mainMenuColumn - mainInfoColumn);	//подсчитаем ее здесь

	table("", 0, tableVertical);		// установить указатель в начало таблицы

	for (auto i : list)	// вывод всей таблицы студентов
	{
		table("");
		cout << i.first;
		table("", mainInfoColumn);
		parseInShortString(i.second, maxLength);
		cout << endl;
	}
}

void drawListLine(short line, multimap <string, vector <discipline>>::iterator i, short color)
{
	line += tableVertical;
	setTheme(color);
	rainbowTable(line, line, table("") * mainMenuColumn + tableAlign - 1, WidthOfWindow());
	// закрашиваем указанную строчку до списка функций
	table("", 0, line);
	cout << i->first;
	table("", mainInfoColumn);
	parseInShortString(i->second, maxLength);	// выводим что было
	setTheme();
}

void getBufferList()		// заполняет лист сохраненными значениями в буфере
{
	if (!findbuf.empty())	// если буфер не пустой, то извлекаем все оттуда
	{
		for (auto i : findbuf)
		{
			list.insert(i);
		}
		findbuf.clear();
	}
}

bool findStudent()
{
	finding = "";		// глобальная переменная

	getBufferList();		// возвращаем изначальный список

	setTheme(setColor());
	rainbowTable(tableVertical - 4, tableVertical - 4);		// раскрашиваем полоску поиска

	table("Поиск:", mainSearchColumn, tableVertical - 4);
	table("Введите искомое слово и нажмите Enter", mainMenuColumn, tableVertical - 4);		// пишем уведомление для пользователя
	table("", mainSearchColumn + 1, tableVertical - 4);		// то переходим к поиску

	setTheme();

	ShowCursor();
	getline(cin, finding);
	HideCursor();

	if (finding == "")
		return 0;		// если строка пустая, то выходим

	for (auto i = list.begin(); i != list.end();)
	{
		if (i->first.find(finding) == -1)			// возвращает -1, если совпадений не найдено
		{			// таким образом, отправляем в буфер те, в которых нет искомой строки
			findbuf.insert(make_pair(i->first, i->second));
			i = list.erase(i);
		}
		else
			i++;
	}
	return 1;
}

void mainTableControl()
{
	bool search = false;	// логическая переменная для работы с поиском
	char key;
start:			// здесь очень удобно использовать метку для вывода обновленного списка и 
				// обнуления параметров, ну дублируя их. Просто используем ее вместо break
	printMainList();		// выводим изначальну таблицу

	auto i = list.begin();		// получаем начало словаря
	int line = 0;			// указатель строки для раскраски

	while (true)
	{

		if (!list.empty())
			drawListLine(line, i, setColor());

		key = _getch();

		switch (key)
		{
		case '1':		// добавить студента
			addStudentScene();
			goto start;

		case '2':		// редактировать шаблоны
			mainSampleList();
			goto start;

		case '5':		// меню настроек
			settings();
			goto start;

		case 27:		// Escape - выход из программы
			if (search)		// если мы хотим закрыть функцию поиска, то возвращаем прежний список и обновляем вывод
			{
				getBufferList();
				finding = "";
				search = false;
				goto start;
			}
			return;

		case '4':		// переход к поиску кнопкой
			if (list.empty() && findbuf.empty())	// выходим, нечего искать
				break;
			if (!list.empty())	// если текущий вывод не пустой, то закрашиваем строку
				drawListLine(line, i, setTheme());	// убираем раскраску текущего

			search = findStudent();		// переходим к функции поиска студентов
			goto start;			// после любого выхода обновляем список и красим начало

		default:
			break;
		}

		if (!list.empty())
			switch (key)
			{
			case '3':		// изменить имя студента
				changeStudentName(i);
				goto start;

			case 83:	// Delete	
			case 8:		// backspace
				if (actionResult(deleteStudent(i)))	// если удален, то обновляем список
					goto start;

				printMainList();	// если студент не удален, печатаем лист заново
				break;

			case 72:	// кнопка вверх, перемещение вверх по списку
				drawListLine(line, i, setTheme());	// убираем раскраску текущего
				if (i == list.begin())	// если итератор на первом студенте
				{
					search = findStudent();		// переходим к функции поиска студентов
					goto start;			// после любого выхода обновляем список и красим начало
				}
				else	// перемещаем указатели вверх
				{
					i--;
					line--;
					drawListLine(line, i, setColor());	// раскрашиваем верхний
				}
				break;	// 75 == left, 77 == right;

			case 80:	// кнопка вниз, перемещение вниз по списку

				if (i == --list.end())	// если итератор на последнем студенте
					break;		// то выходим

				drawListLine(line, i, setTheme());	// убираем раскраску текущего
				i++;
				line++;
				drawListLine(line, i, setColor());	// раскрашиваем нижний
				break;

			case 13:	// Enter - редактирование дисциплин студента
				StudentEditor(i);
				goto start;

			default:
				break;
			}
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

	if (checkDataBase(externalfile) && checkDataBase(samplefile))	// проверка существвания базы данных
	{
		importStudents();	// если есть, то импортируем
		importSamples();
	}
	else
	{
		cout << "Ошибка подключения файла";		// иначе выходим, нам что, заняться нечем? (ну вообще да, хех)
		pause();
		return 0;
	}

	if (!samples.empty())			// если список не пуст, то поставим первый шаблон
		sample = samples.begin()->first;

	HideCursor();		// скрываем курсор, он нам не понадобится
	mainTableControl();

	exportStudents();
	exportSamples();
	saveSettings();
	return 0;
}

bool importStudents()	// заполнение словаря из файла
{
	ifstream file(externalfile);
	if (file)
	{
		while (true)
		{
			string buf = "";	// временная строка для чтения буфера
			file >> buf;
			if (file.eof())	// выходим, если достигнут конец файла
				break;


			string line = "";	// строка для считывания нескольких слов

			while (buf != "&")	// считываем имя студента до разделителя
			{
				line += buf + " ";	// собираем строку
				file >> buf;	// считываем слово			
			}
			auto i = list.insert(make_pair(line, NULL));	// добавляем нового студента
			discipline structbuf;	// структура для заполнения вектора

			file >> buf;
			while (buf != "|")		// пока не достигнут символ, считываем дисциплины студента
			{
				line = "";
				while (buf != "&")	// считываем дисциплину
				{
					line += buf + " ";
					file >> buf;
				}
				structbuf.subject = line;	// записываем дисциплину в структуру
				file >> structbuf.score;	// считываем оценку в структуру
				i->second.push_back(structbuf);	// добавляем структуру в вектор

				file >> buf;
			}
		}
		file.close();
		return 1;
	}
	return 0;
}

bool exportStudents()	// отправка словаря в файл
{
	ofstream file(externalfile);
	if (file)
	{
		for (auto i : list)		// перебираем весь словарь
		{
			file << i.first << " & ";		// отделяем имя знаком &
			for (auto j : i.second)		// перебираем весь вектор одного слова
			{
				file << j.subject << " & ";	//отделяем дисциплину от оценки
				file << j.score << " ";		// отделяем дисциплины
			}
			file << " | \n";		// отделяем студентов
		}

		file.close();
		return 1;
	}
	return 0;
}

bool importSamples()	// заполнение словаря из файла
{
	ifstream file(samplefile);
	if (file)
	{
		while (true)
		{
			string buf = "";	// временная строка для чтения буфера
			file >> buf;
			if (file.eof())	// выходим, если достигнут конец файла
				break;


			string name = "";	// строка для имени шаблона
			string disc;	// строка для названия дисциплины

			while (buf != "&")	// считываем имя шаблона до разделителя
			{
				name += buf + " ";	// собираем строку
				file >> buf;	// считываем слово			
			}
			samples[name];	// добавляем новый шаблон

			file >> buf;
			while (buf != "|")		// пока не достигнут символ, считываем дисциплины шаблона
			{
				disc = "";
				while (buf != "&")	// считываем дисциплину
				{
					disc += buf + " ";
					file >> buf;
				}
				samples[name].push_back(disc);	// добавляем структуру в вектор
				file >> buf;
			}
		}
		file.close();
		return 1;
	}
	return 0;
}

bool exportSamples()	// отправка словаря в файл
{
	ofstream file(samplefile);
	if (file)
	{
		for (auto i : samples)		// перебираем весь словарь
		{
			file << i.first << " & ";		// отделяем имя знаком &
			for (auto j : i.second)		// перебираем весь вектор одного слова
			{
				file << j << " & ";		//отделяем дисциплину
			}
			file << " | \n";		// отделяем шаблоны
		}
		file.close();
		return 1;
	}
	return 0;
}

void clean()
{
	setTheme();
	system("cls");
}