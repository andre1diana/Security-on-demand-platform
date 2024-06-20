#pragma once
#include<string>
#include<openssl/evp.h>
#include<iostream>

class CriptoOperations
{
private:
    EVP_CIPHER_CTX* ctx_enc = nullptr;
    EVP_MD_CTX* ctx_hash = nullptr;
    EVP_PKEY_CTX* ctx_key = nullptr;

public:
    CriptoOperations() {}
    ~CriptoOperations() {}

    void generateRandBytes(unsigned char** buff, int Length);
    void saveToFile(const unsigned char* key,
        const unsigned char* iv, const std::string& filename);

    //encrypt/decrypt
    void encrypt(int mode, const unsigned char* plaintext, int pl_len,
        unsigned char* ciphertext, int& cp_len, const unsigned char* key, const unsigned char* iv);
    void decrypt(int mode, const unsigned char* ciphertext, int cp_len, unsigned char* plaintext,
        int& pl_len, const unsigned char* key, const unsigned char* iv);

    //generate keys
    int generateKeyPair(int mode, int bits, EVP_PKEY*& private_key, EVP_PKEY*& public_key);
    
   //generate certificate 
    X509* generateCertificate(EVP_PKEY* pkey, const std::string& country, const std::string& organization,
        const std::string& commonName, int days);

    //hash
    void hash(int mode, const unsigned char* plaintext, unsigned int& inLen, unsigned char* hash_output,
        unsigned int& lengthOfHash);
    void generateHMAC_SHA256(const unsigned char* key, const std::string& plaintext, unsigned char* mac_output, unsigned int& mac_length);
};