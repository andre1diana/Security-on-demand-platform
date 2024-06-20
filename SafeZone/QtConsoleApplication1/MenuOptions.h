#pragma once
#include "CriptoOperations.h"

class MenuOptions
{
private:
	CriptoOperations operation;

public:
	MenuOptions() {}
	~MenuOptions() {}

    void loginOption();

	void signupOption();

	void encryption(int option);

	void decryption();

	void hash(int option);

	void mac();

	void generateKeyOption(int option);

	void downloadKey();

	void generateCertificateOption();
};

