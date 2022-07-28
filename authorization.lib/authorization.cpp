
#include <fstream>
#include "authorization.h"

using namespace std;
char authfilename[] = "../authorization.dat";

bool changeFileName(const char* filename)
{
	if(!strcpy_s(authfilename, filename))
		return true;
	return false;
}

bool checkFile()		// �������� ������� �����
{
	ifstream file(authfilename);
	if (file)
	{
		file.close();
		return true;
	}
	return false;
}

bool createFile()		// �������� �����
{
	ofstream file(authfilename);
	if (file)
	{
		file.close();
		return true;
	}
	return false;
}

int findLogin(const char* log)		// ����� ������������ �� ������
{
	ifstream file(authfilename, ios::binary);
	if (file)
	{
		int count = 0;
		form element;

		while (!file.read((char*)&element, sizeof(form)).eof())
		{
			if (strcmp(log, element.login) == 0)
			{
				file.close();
				return count;		// ���������� ������� ���������
			}
			count++;	// ���������� ������ ���, ����� �������� ���������
		}
		file.close();
	}
	return -1;		// ���� �� ������
}

bool authorization(int position, const char* password)	// �����������
{
	ifstream imp(authfilename, ios::binary);

	if (imp)
	{
		form element;

		imp.seekg(position * sizeof(form));
		imp.read((char*)&element, sizeof(form));

		if (strcmp(element.password, password) == 0)	// ���� ������ ���������, �� ����������� ������ �������
		{
			imp.close();
			return true;
		}
	}
	return false;		// ���� ����������� �� ������
}

form returnFormByLogin(int position)
{
	ifstream imp(authfilename, ios::binary);
	form element = { 0 };

	if (imp)
	{
		imp.seekg(position * sizeof(form));
		imp.read((char*)&element, sizeof(form));

		imp.close();
		return element;
	}
	return element;		// ���� ����������� �� ������
}