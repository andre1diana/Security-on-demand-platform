#pragma once
#include<fstream>

class Logger
{
private:
	std::ofstream file;

	static Logger* instance;
	Logger() { file.open("LogFile.txt", std::ios::app); }
	~Logger();

public:
	static Logger& getInstance();
	static void destroyInstance();

	void logMessage(int code, std::string message, std::string username);
	void logError(int code, std::string message);
	void logMessageAndInt(int code, std::string message, int number);
};

