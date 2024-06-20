#include "Client.h"

void Client::setUsername(std::string user)
{
	this->username = user;
}

void Client::setPassword(std::string password)
{
	this->password = password;
}

std::string Client::getUsername()
{
	return username;
}

std::string Client::getPassword()
{
	return password;
}

int Client::getFilesNumber()
{
	return encFilesNumber;
}

int Client::getHashNumber()
{
	return hashesNumber;
}

int Client::getKeysNumber()
{
	return keysNumber;
}

int Client::getCertificatesNumber()
{
	return certificatesNumber;
}

std::vector<EncFiles*> Client::getFiles()
{
	return files;
}

std::vector<Hashes*> Client::getHashes()
{
	return hashes;
}

std::vector<GeneratedKeys*> Client::getKeyPairs()
{
	return keys;
}

std::vector<std::string> Client::getCertificates()
{
	return certificatesFiles;
}

void Client::addFile(std::string originalFile, std::string encFile, unsigned char* key, unsigned char* iv, std::string mode)
{
	EncFiles* file = new EncFiles(originalFile, encFile, key, iv, mode);
	files.push_back(file);
	encFilesNumber++;
}

void Client::addHash(std::string filename, std::string hash)
{
	Hashes* hash1 = new Hashes(filename, hash);
	hashes.push_back(hash1);
	hashesNumber++;
}

void Client::addKeyPair(std::string name, EVP_PKEY* priv, EVP_PKEY* pub)
{
	GeneratedKeys* key = new GeneratedKeys(name, priv, pub);
	keys.push_back(key);
	keysNumber++;
}

void Client::addCertificate(std::string certificateFile)
{
	certificatesFiles.push_back(certificateFile);
	certificatesNumber++;
}