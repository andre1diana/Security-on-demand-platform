#pragma once
#include<fstream>
#include<vector>
#include<openssl/evp.h>
#include"Client.h"

class FileOperations
{
private:
	const char* filename;

	unsigned char* readBinaryData(std::ifstream& f, size_t length);
	EVP_PKEY* readPEMKey(std::ifstream& f);
	void writePEMKey(EVP_PKEY* pkey, std::ofstream& f);

public:
	FileOperations() : filename("Clients_new.txt") {}
	~FileOperations() {}

	void loadClientsFromFile(std::vector<Client*>& clients);
	void loadClientsToFile(std::vector<Client*>& clients);
};

