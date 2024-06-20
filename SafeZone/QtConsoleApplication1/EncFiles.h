#pragma once
#include<string>	
class EncFiles
{
private:
	std::string originalFile;
	std::string encryptedFile;
	unsigned char* key;
	unsigned char* iv;
	std::string encriptionMode;

public:
	EncFiles() {}
	EncFiles(std::string of, std::string ef, unsigned char* k, unsigned char* i, std::string encm) :
		originalFile(of), encryptedFile(ef),key(k), iv(i), encriptionMode(encm) {
	}
	~EncFiles() {}
	//EncFiles(const EncFiles& t);

	std::string getOriginalFile() const;
	std::string getEncFile() const;
	unsigned char* getKey() const;
	unsigned char* getIV() const;
	std::string getEncMode() const;

};