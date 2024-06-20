#pragma once
#include<string>
#include<vector>
#include"Hashes.h"
#include"EncFiles.h"
#include"GeneratedKeys.h"

class Client
{
private:
	std::string username;
	std::string password;
	int encFilesNumber;
	int hashesNumber;
	int keysNumber;
	int certificatesNumber;
	std::vector<EncFiles*> files;
	std::vector<Hashes*> hashes;
	std::vector<GeneratedKeys*> keys;
	std::vector<std::string> certificatesFiles;

public:
	Client() { 
		encFilesNumber = 0; 
		hashesNumber = 0;
		keysNumber = 0;
		certificatesNumber = 0;
	};
	Client(std::string user, std::string pass) :
		username(user), password(pass) {}
	~Client() {}

	void setUsername(std::string user);
	void setPassword(std::string password);
	std::string getUsername();
	std::string getPassword();

	int getFilesNumber();
	int getHashNumber();
	int getKeysNumber();
	int getCertificatesNumber();

	std::vector<EncFiles*> getFiles();
	std::vector<Hashes*> getHashes();
	std::vector<GeneratedKeys*> getKeyPairs();
	std::vector<std::string> getCertificates();
	
	void addFile(std::string originalFile, std::string encFile, unsigned char* key, unsigned char* iv, std::string mode);
	void addHash(std::string filename, std::string hash);
	void addKeyPair(std::string name, EVP_PKEY* priv, EVP_PKEY* pub);
	void addCertificate(std::string certificateFile);
};

