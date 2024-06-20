#include "Logger.h"
#include <chrono>
#include <ctime>    

Logger* Logger::instance = nullptr;

Logger::~Logger()
{
	file.close();
}

Logger& Logger::getInstance()
{
	if (!Logger::instance)
	{
		Logger::instance = new Logger();
	}
	return *Logger::instance;
}

void Logger::destroyInstance()
{
	if (Logger::instance)
	{
		delete Logger::instance;
		Logger::instance = nullptr;
	}
}

void Logger::logMessage(int code, std::string message, std::string username)
{
	time_t now = time(0);
	char* dt = ctime(&now);
	if (dt[strlen(dt) - 1] == '\n') {
		dt[strlen(dt) - 1] = '\0';
	}
	file << dt;
	file << " - " << username << " : ";
	file << code;
	file << " : " << message << std::endl;
}

void Logger::logError(int code, std::string message)
{
	file << "ERORR -- " << code << " : " << message << std::endl;
}

void Logger::logMessageAndInt(int code, std::string message, int number)
{
	time_t now = time(0);
	char* dt = ctime(&now);
	if (dt[strlen(dt) - 1] == '\n') {
		dt[strlen(dt) - 1] = '\0';
	}
	file << dt << " - ";
	file << code << " : ";
	file << message << " " << number << std::endl;
}
