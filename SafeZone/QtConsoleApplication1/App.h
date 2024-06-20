#pragma once

#include "Client.h"
#include "FilesOperations.h"
#include "MenuOptions.h"

#include<vector>

class App
{
private:
	std::vector<Client*> clients;
	FileOperations fileOperations;
	Client* loggedClient;
	MenuOptions menu;
	static App* instance;
	App() { fileOperations.loadClientsFromFile(clients); }
	~App();

public:
	static App& getInstance();
	static void destroyInstance();

	void addClient(std::string name, std::string password);

	std::vector<Client*> getClients();

	Client* getLoggedClient();

	void addFileToLoggedClient(std::string originalFile, std::string encFile, unsigned char* key, unsigned char* iv, std::string mode);

	void addHashToLoggedClient(std::string filename, std::string hash);

	void addKeyPairToLoggedClient(std::string name, EVP_PKEY* priv, EVP_PKEY* pub);

	MenuOptions getMenu();

	bool verifyExistentClient(std::string username, std::string password);

	bool signupNewClient(std::string username, std::string password);
};

