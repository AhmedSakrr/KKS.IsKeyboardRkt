#pragma once
#include <Windows.h>
namespace Console 
{
	void InitConsole();
	void SetWarningConsoleTextAttributes();
	void SetNormalConsoleTextAttributes();
	void SetGoodConsoleTextAttributes();
	COORD GetCursorPosition();
	void SetCursorPosition(COORD pos);

	//
	// Function: InitConsole
	//
	void InitConsole()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Set Title
		SetConsoleTitle(L"IT Software - Is Keyboard Rootkitted?");

		SetNormalConsoleTextAttributes();
	}


	//
	// Function: SetWarningConsoleTextAttributes
	//
	void SetWarningConsoleTextAttributes()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}

	//
	// Function: SetNormalConsoleTextAttributes
	//
	void SetNormalConsoleTextAttributes()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	}

	//
	// Function: SetGoodConsoleTextAttributes
	//
	void SetGoodConsoleTextAttributes()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	}

	COORD GetCursorPosition()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO info = { 0 };
		GetConsoleScreenBufferInfo(hConsole, &info);
		return info.dwCursorPosition;
	}

	void SetCursorPosition(COORD pos)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(hConsole, pos);
	}
}