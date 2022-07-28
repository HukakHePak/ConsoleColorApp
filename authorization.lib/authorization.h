#pragma once

struct form				// Структура хранения информации о пользователе
{
	char name[15];
	char login[15];
	char password[15];
};

bool changeFileName(const char* filename);
bool checkFile();		// проверяет существование файла
bool createFile();		// создание файла
int findLogin(const char* log);		// поиск позиции структуры по логину
bool authorization(int position, const char* password);	// возвращает истину при совпадении логинов и паролей по позиции, логину и паролю
form returnFormByLogin(int position);	// возвращает имя пользователя по позиции