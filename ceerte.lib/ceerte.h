
extern bool drawAll;		// ������������ ������ �������
extern bool inverseDraw;// ������������ �������� ������ ������
extern short mainColor;		// �������� ���� ����. ���� ������ �������� �������� (������ ��� �����)
extern bool drawing;		// �������� ��������� ������
extern bool rainbow;		// �������� ������������ ���������

void GotoXY(short x, short y);		// ��������� ���������� � ���������� ���������
void SetTextColor(short color);		// ��������� ���� �� 0 �� f � ������ ���� ������
void SetTextBackground(short color);		// ��������� ���� �� 0 �� f � ������ ���� ������� ������
void HideCursor();		// �������� ������
void ShowCursor();		// ����������� ������
short WidthOfWindow();		// ���������� ������ ���� �������
short HeightOfWindow();		// ���������� ����� ���� �������
short getConcoleY();		// ���������� ������� ���������� Y ���������
short getConsoleX();		// ���������� ������� ���������� X ���������

void center(const char* text, short x = 0, short y = 0, bool line = false, short color = mainColor, bool all = drawAll, bool inverse = inverseDraw);

// ������������ �� ������, ��������� ������ ��� ������, ����� �� ������ � ������ �� ������.
// ���� ������ ������ short x �� �����, ���������� �����, ����� �������� ����� �� �������� �����.														
// ���� ������ ������ hort y �� �����, ���������� �������, ����� �������� ����� �� �������� ��������� ����������

// ���� �� ����� ��������� ������� �� �������, �� �������� ���������� ���������� false � endl
// ���� �� ����� �������� ����� ����� �� ������ �� ����� ������, ���������� -1 � short x
// ���� �� ����� ������� ����� �� ������ �� ������� �������, ���������� 1 � short y

// ���� �� ����� ���������� ���� �����, �� ���������� � draw �������� true
// ���� �� ����� ��������� ����� ����, ���������� ��� � color
// ���� �� ����� ������������ ��� ������, �� ���������� true � all
// ���� �� ����� ���������� ��������� �� �������� ������, ���������� inverse true


short rainbowTable(short startY, short finishY, short leftX = 0, short rightX = 0);
// ���������� ����� �� startY �� finishY, ���������� �������� Y
// ���� �� ������ ������� ��� ����������, ����������� ��� ������
void printRectangle(int length, int heigh);
// ��������� ������ ���� � ����, ����� ������� ������������� ������� �� ������

short randomColor();		// ������������� ���� ��������� ����� ���� � ������ ���� ����� ��� ������, ���������� ������������� ����
short setTheme(short color = mainColor);		// ������������� ����� ������� ����
void setFontColor(short color = mainColor);	// ������������� ������ ��� ����� ���� ������
											//� ����������� �� ����� ���� (���� ���� �� ���������
void logicOfDraw(const char* text, short color, bool all = drawAll, bool inverse = inverseDraw);
short inverseColor(short color);	// ��������� ���� ����, ������������� ��������������� ���. �� ��������� ���� ����
short setColor();		// ������������� � ���������� ���� � ����������� �� ��������
								// ������� ������������ ��������� � �������� randomColor() ��� ������� ��������� � ���������� ������
								// ������������ �����, ��� �� ����� ���������� �����. ������ ����������� � �������� �����, � �� �������
								// ������ ����
void printColor(short color = mainColor);	// ������� ��� ������� �����
void printBool(bool pin);		// ������� ���������� �������� �������
void settings();			// ���� ��������
//void setSettings(short* mColor, bool* mBool);	// ��������� ��������� ���������
//int GoUp(short Up_Y, int sel);		// ����������� ����� �� ������
//int GoDown(short Down_Y, int sel);		// ����������� ���� �� ������
//void GoLeft(short* mColor, bool* mBool, int sel);	// ����� �������� �����
//void GoRight(short* mColor, bool* mBool, int sel);	// ����� �������� ������
bool loadSettings();	// ���������� ���������
bool saveSettings();	// ��������� ���������

short table(const char* text, int col = 0, short y = 0);	
// ��������� ������������. ��������� ������ ������, ����� ������� � ��������� ������������ ������

short parseInColumn(const char* text, short width = 20, bool print = false);
void setTableAlign(short align, short columns);	// ������ ������ �� ����� � ���������� �������� �������

bool checkDataBase(const char* filename);
void pause();			// �����
void printNull();		// �����������, ���� ��� ����������