#pragma once
#include<string>
#include<openssl/evp.h>

class GeneratedKeys
{
private:
	std::string name;
	EVP_PKEY* private_key;
	EVP_PKEY* public_key;

public:
	GeneratedKeys() {}
	GeneratedKeys(std::string n, EVP_PKEY* priv, EVP_PKEY* pub) : 
		name(n), public_key(pub), private_key(priv) {}
	~GeneratedKeys() {}

	std::string getName() const;
	EVP_PKEY* getPrivKey() const;
	EVP_PKEY* getPubKey() const;
};

