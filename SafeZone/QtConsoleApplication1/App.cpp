#include "App.h"
#include<iostream>


App* App::instance = nullptr;

App::~App()
{
	fileOperations.loadClientsToFile(clients);
	for (Client* client : clients)
	{
		delete client;
	}
}

App& App::getInstance()
{
	if (!App::instance)
	{
		App::instance = new App();
	}
	return (*App::instance);
}

void App::destroyInstance()
{
	if (App::instance)
	{
		delete App::instance;
		App::instance = nullptr;
	}
}


void App::addClient(std::string name, std::string password)
{
	Client* client = new Client;
	client->setUsername(name);
	client->setPassword(password);
	clients.push_back(client);
}

std::vector<Client*> App::getClients()
{
	return clients;
}

Client* App::getLoggedClient()
{
	return loggedClient;
}

void App::addFileToLoggedClient(std::string originalFile, std::string encFile, unsigned char* key, unsigned char* iv, std::string mode)
{
	loggedClient->addFile(originalFile, encFile, key, iv, mode);
}

void App::addHashToLoggedClient(std::string filename, std::string hash)
{
	loggedClient->addHash(filename, hash);
}

void App::addKeyPairToLoggedClient(std::string name, EVP_PKEY* priv, EVP_PKEY* pub)
{
	loggedClient->addKeyPair(name, priv, pub);
}

MenuOptions App::getMenu()
{
	return menu;
}

// This function verifies if the client that want to authenticate exists and if so, set the 
// logged client with this client
bool App::verifyExistentClient(std::string username, std::string password)
{
	for (auto client : clients)
	{
		if (client->getUsername() == username && client->getPassword() == password)
		{
			loggedClient = client;
			return true;
		}
	}
	return false;
}

bool App::signupNewClient(std::string username, std::string password)
{
	for (auto client : clients)
	{
		if (client->getUsername() == username)
		{
			return false;
		}
	}
	addClient(username, password);
	return true;
}