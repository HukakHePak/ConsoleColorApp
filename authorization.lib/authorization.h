#pragma once

struct form				// ��������� �������� ���������� � ������������
{
	char name[15];
	char login[15];
	char password[15];
};

bool changeFileName(const char* filename);
bool checkFile();		// ��������� ������������� �����
bool createFile();		// �������� �����
int findLogin(const char* log);		// ����� ������� ��������� �� ������
bool authorization(int position, const char* password);	// ���������� ������ ��� ���������� ������� � ������� �� �������, ������ � ������
form returnFormByLogin(int position);	// ���������� ��� ������������ �� �������