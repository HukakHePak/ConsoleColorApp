
extern bool drawAll;		// раскрашивать строки целиком
extern bool inverseDraw;// раскрашивать значения вместо текста
extern short mainColor;		// основной цвет фона. Цвет текста задается читаемым (черный или белый)
extern bool drawing;		// включить раскраску вывода
extern bool rainbow;		// включить разноцветную раскраску

void GotoXY(short x, short y);		// принимает координаты и перемещает указатель
void SetTextColor(short color);		// принимает цвет от 0 до f и меняет цвет текста
void SetTextBackground(short color);		// принимает цвет от 0 до f и меняет цвет заливки текста
void HideCursor();		// скрывает курсор
void ShowCursor();		// потображает курсор
short WidthOfWindow();		// возвращает ширину окна консоли
short HeightOfWindow();		// возвращает длину окна консоли
short getConcoleY();		// возвращает текущую координату Y указателя
short getConsoleX();		// возвращает текущую координату X указателя

void center(const char* text, short x = 0, short y = 0, bool line = false, short color = mainColor, bool all = drawAll, bool inverse = inverseDraw);

// Выравнивание по центру, принимает строку для вывода, оступ по ширине и отступ по высоте.
// Если отступ ширины short x не задан, использует центр, иначе сдвигает влево на заданную длину.														
// Если отступ высоты hort y не задан, использует текущий, иначе сдвигает вверх на половину заданного количества

// Если мы хотим отключить перенос по строкам, то посленим параметром отправляем false в endl
// Если мы хотим сдвигать текст влево от центра на длину строки, отправляем -1 в short x
// Если мы хотим вывести текст по центру от верхней границы, отправляем 1 в short y

// Если мы хотим раскрасить весь вывод, то отправляем в draw значение true
// Если мы хотим назначить особы цвет, отправляем его в color
// Если мы хотим раскрашивать все строки, то отправляем true в all
// Если мы хотим раскрасить следующую за функцией строку, отправляем inverse true


short rainbowTable(short startY, short finishY, short leftX = 0, short rightX = 0);
// заполнение строк от startY до finishY, возвращает исходный Y
// если не задана область для исключения, закрашивает всю строку
void printRectangle(int length, int heigh);
// принимает размер окна и цвет, затем выводит прямоугольную область по центру

short randomColor();		// устанавливает типо рандомные цвета фона и черный либо белый для текста, возвращает установленный цвет
short setTheme(short color = mainColor);		// устанавливает цвета главной темы
void setFontColor(short color = mainColor);	// устанавливает черный или белый цвет текста
											//в зависимости от цвета фона (цвет фона по умолчанию
void logicOfDraw(const char* text, short color, bool all = drawAll, bool inverse = inverseDraw);
short inverseColor(short color);	// принимает цвет фона, устанавливает противоположный ему. по умолчанию цвет темы
short setColor();		// устанавливает и возвращает цвет в зависимости от констант
								// следует использовать совместно с функцией randomColor() для цветной раскраски и стабильной работы
								// используется везде, где мы хотим раскрасить вывод. Просто отрправляем в качестве цвета, и он выберет
								// нужный цвет
void printColor(short color = mainColor);	// выводит два пикселя цвета
void printBool(bool pin);		// выводит логическое значение словами
void settings();			// окно настроек
//void setSettings(short* mColor, bool* mBool);	// применяет выбранные настройки
//int GoUp(short Up_Y, int sel);		// перемещение вверх по списку
//int GoDown(short Down_Y, int sel);		// перемещение вниз по списку
//void GoLeft(short* mColor, bool* mBool, int sel);	// смена значения влево
//void GoRight(short* mColor, bool* mBool, int sel);	// смена значения вправо
bool loadSettings();	// подгружает настройки
bool saveSettings();	// сохраняет настройки

short table(const char* text, int col = 0, short y = 0);	
// Табличное выравнивание. Принимает строку вывода, номер колонки и первичный вертикальный отступ

short parseInColumn(const char* text, short width = 20, bool print = false);
void setTableAlign(short align, short columns);	// задать отступ от краев и количество столбцов таблицы

bool checkDataBase(const char* filename);
void pause();			// пауза
void printNull();		// уведомление, если нет результата