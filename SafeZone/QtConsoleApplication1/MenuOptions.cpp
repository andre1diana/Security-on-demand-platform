#include "MenuOptions.h"
#include"Codes.h"
#include"App.h"
#include"Server.h"
#include"Logger.h"

#include <assert.h>
#include<iostream>
#include<fstream>
#include <openssl/pem.h>


void MenuOptions::loginOption()
{
    char usernameBuff[256];
    int size = Server::getInstance().receiveData(usernameBuff);
    usernameBuff[size] = '\0';
    std::string username(usernameBuff);

    char passwordBuff[256];
    size = Server::getInstance().receiveData(passwordBuff);
    passwordBuff[size] = '\0';
    std::string password(passwordBuff);

    if (App::getInstance().verifyExistentClient(username, password)) {
        Logger::getInstance().logMessage(LOGIN_ACCEPT, "Login accepted!", username);
        Server::getInstance().sendInt(LOGIN_ACCEPT);
    }
    else {
        Logger::getInstance().logMessage(LOGIN_DECLINE, "Login declined!", username);
        Server::getInstance().sendInt(LOGIN_DECLINE);
    }
}

void MenuOptions::signupOption()
{
    char usernameBuff[256];
    int size = Server::getInstance().receiveData(usernameBuff);
    usernameBuff[size] = '\0';
    std::string username(usernameBuff);

    char passwordBuff[256];
    size = Server::getInstance().receiveData(passwordBuff);
    passwordBuff[size] = '\0';
    std::string password(passwordBuff);

    if (App::getInstance().signupNewClient(username, password)) {
        Logger::getInstance().logMessage(SIGNUP_ACCEPT, "Sign up accepted!", username);
        Server::getInstance().sendInt(SIGNUP_ACCEPT);
    }
    else {
        Logger::getInstance().logMessage(SIGNUP_DECLINE, "Sign up declined!", username);
        Server::getInstance().sendInt(SIGNUP_DECLINE);
    }
}

//for received files the outfile will be like 'inFile.enc'
void MenuOptions::encryption(int option)
{
    Logger::getInstance().logMessageAndInt(1, "Execute Encrypt on file with mode ", option);
    
    //receive the file
    std::string inFile = Server::getInstance().receiveFile();
    std::fstream in(inFile, std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        Logger::getInstance().logError(1, "Error MenuOptions (encryption) : Input file could not be opened!");
        Server::getInstance().sendInt(SEND_FILE_ERROR);
        return;
    }
    else {
        Server::getInstance().sendInt(SEND_FILE_SUCCESS);
    }

    //the output file
    std::string outFile;
    std::size_t dotPosition = inFile.find_last_of('.');
    if (dotPosition == std::string::npos) {
        outFile =  inFile + ".enc";
    }
    else{
        outFile = inFile.substr(0, dotPosition) + ".enc";
    }
 
    std::remove(outFile.c_str());
    std::fstream out(outFile, std::ios::out | std::ios::binary);
    if (!out.is_open())
    {
        Logger::getInstance().logError(1, "Error MenuOptions (encryption) : Output file could not be opened!");
        return;
    }

    //begin encryption
    unsigned char* key = nullptr;
    unsigned char* iv = nullptr;
    int key_length = 0, iv_length = 0;
    key = new unsigned char[key_length];
    iv = new unsigned char[iv_length];
    operation.generateRandBytes(&key, key_length);
    operation.generateRandBytes(&iv, iv_length);

    switch (option)
    {
    case AES_CBC_ENC:
        App::getInstance().addFileToLoggedClient(inFile, outFile, key, iv, "AES_CBC");
        key_length = EVP_CIPHER_key_length(EVP_aes_256_cbc());
        iv_length = EVP_CIPHER_iv_length(EVP_aes_256_cbc());
        break;

    case AES_OFB_ENC:
        App::getInstance().addFileToLoggedClient(inFile, outFile, key, iv, "AES_OFB");
        key_length = EVP_CIPHER_key_length(EVP_aes_256_ofb());
        iv_length = EVP_CIPHER_iv_length(EVP_aes_256_ofb());
        break;

    case _3DES_ENC:
        App::getInstance().addFileToLoggedClient(inFile, outFile, key, iv, "3DES");
        key_length = EVP_CIPHER_key_length(EVP_des_ede3_cbc());
        iv_length = EVP_CIPHER_iv_length(EVP_des_ede3_cbc());
        break;
    }

    unsigned char plaintext[1024];
    unsigned char ciphertext[4096];
    while (in) {

        in.read(reinterpret_cast<char*>(plaintext), sizeof(plaintext));
        std::streamsize readLen = in.gcount();
        if (readLen > 0) {
            int outLen;
            operation.encrypt(option, plaintext, readLen, ciphertext, outLen, key, iv);
            assert(outLen <= 4096);
            out.write((const char*)(ciphertext), outLen);
        }
    }

    in.close();
    out.close();

    //send the file to client
    Server::getInstance().sendFile(outFile.c_str());
}

void MenuOptions::decryption()
{
    Logger::getInstance().logMessage(1, "Execute Decrypt on file.", App::getInstance().getLoggedClient()->getUsername());
    //receive the file
    std::string inFile = Server::getInstance().receiveFile();
    std::fstream in(inFile, std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        Logger::getInstance().logError(1, "Error MenuOptions (encryption) : Input file could not be opened!");
        Server::getInstance().sendInt(SEND_FILE_ERROR);
        return;
    }
    else {
        Server::getInstance().sendInt(SEND_FILE_SUCCESS);
    }

    //the output file
    std::string outFile;
    std::size_t dotPosition = inFile.find_last_of('.');
    if (dotPosition == std::string::npos) {
        outFile = inFile + "_dec.txt";
    }
    else {
        outFile = inFile.substr(0, dotPosition) + "_dec.txt";
    }

    std::remove(outFile.c_str());
    std::fstream out(outFile, std::ios::out | std::ios::binary);
    if (!out.is_open())
    {
        Logger::getInstance().logError(1, "Error MenuOptions (decryption) : Output file could not be opened!");
        return;
    }
    std::string originalFile = inFile;
    std::size_t Position = originalFile.find_last_of('.');
    if (Position == std::string::npos) {
        originalFile = originalFile + ".txt";
    }
    else {
        originalFile = originalFile.substr(0, Position) + ".txt";
    }

    EncFiles* file;
    bool found = false;
    for (const auto& file1 : App::getInstance().getLoggedClient()->getFiles())
    {
        if (file1->getOriginalFile() == originalFile)
        {
            file = file1;
            found = true;
            break;
        }
    }
    if (!found)
    {
        Logger::getInstance().logError(1, "Error MenuOptions (decryption) : File does not exist in your database!");
        return;
    }

    unsigned char plaintext[1024 + 16];
    unsigned char ciphertext[1024];

    if (file->getEncMode() == "AES_CBC\0")
    {
        while (in) {
            in.read(reinterpret_cast<char*>(ciphertext), sizeof(ciphertext));
            std::streamsize readLen = in.gcount();
            int outLen = 0;
            operation.decrypt(AES_CBC_ENC, ciphertext, readLen, plaintext, outLen, file->getKey(), file->getIV()); //todo verifica ordinea variabilelor
            if (readLen > 0) {
                out.write((const char*)(plaintext), outLen);
            }
        }
    }
    else if (file->getEncMode() == "AES_OFB\0")
    {
        while (in) {
            in.read(reinterpret_cast<char*>(ciphertext), sizeof(ciphertext));
            std::streamsize readLen = in.gcount();
            int outLen = 0;
            operation.decrypt(AES_OFB_ENC, ciphertext, readLen, plaintext, outLen, file->getKey(), file->getIV());
            if (readLen > 0) {
                out.write((const char*)(ciphertext), outLen);
            }
        }
    }
    else if (file->getEncMode() == "3DES\0")
    {
        while (in) {
            in.read(reinterpret_cast<char*>(ciphertext), sizeof(ciphertext));
            std::streamsize readLen = in.gcount();
            int outLen = 0;
            operation.decrypt(_3DES_ENC, ciphertext, readLen, plaintext, outLen, file->getKey(), file->getIV());
            if (readLen > 0) {
                out.write((const char*)(ciphertext), outLen);
            }
        }
        
    }
    else {
        Logger::getInstance().logError(1, "Error MenuOptions (decryption) : Error to find encryption algorithm!");
    }

    in.close();
    out.close();

    //send file to client
    Server::getInstance().sendFile(outFile.c_str());
}

void MenuOptions::hash(int option)
{
    Logger::getInstance().logMessageAndInt(1, "Execute Hash on file with mode ", option);
    //receive the file
    std::string inFile = Server::getInstance().receiveFile();
    std::fstream in(inFile, std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        Logger::getInstance().logError(1, "Error MenuOptions (encryption) : Input file could not be opened!");
        Server::getInstance().sendInt(SEND_FILE_ERROR);
        return;
    }
    else {
        Server::getInstance().sendInt(SEND_FILE_SUCCESS);
    }

    size_t pos = inFile.find(".");
    std::string outFile = inFile.substr(0, pos) + "_hash.txt";

    std::fstream out(outFile, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        Logger::getInstance().logError(1, "Error MenuOptions (hash) : Output file could not be opened!");
        return;
    }
    
    unsigned int hash_length = 0;
    switch (option) {
    case SHA1_OPT:
    	hash_length = EVP_MD_size(EVP_sha1());
    	break;
    case SHA2_OPT:
    	hash_length = EVP_MD_size(EVP_sha256());
    	break;
    case SHA3_OPT:
    	hash_length = EVP_MD_size(EVP_sha3_256());
    	break;
    default:
        Logger::getInstance().logError(1, "Error MenuOptions (hash) : Unknown hash type.");
    	return;
    }
    unsigned char plaintext[1024];
    unsigned char* hash_output = new unsigned char[hash_length];
    unsigned int outLen = 0;
    while (in)
    {
    	in.read((char*)plaintext, 1024);
    	unsigned int readLen = in.gcount();
    	if (readLen > 0) {
    		operation.hash(option, plaintext, readLen, hash_output, outLen);
    	}
    	out.write((char*)hash_output, outLen);
    }
    std::string hash_str(reinterpret_cast<char*>(hash_output), hash_length);
    App::getInstance().addHashToLoggedClient(inFile, hash_str);

    delete[] hash_output;
    in.close();
    out.close();

    Server::getInstance().sendFile(outFile.c_str());
}

void MenuOptions::mac()
{
    Logger::getInstance().logMessage(1, "Execute MAC on file!", "");

    //receive the file
    std::string inFile = Server::getInstance().receiveFile();
    std::fstream in(inFile, std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        Logger::getInstance().logError(1, "Error MenuOptions (encryption) : Input file could not be opened!");
        Server::getInstance().sendInt(SEND_FILE_ERROR);
        return;
    }
    else {
        Server::getInstance().sendInt(SEND_FILE_SUCCESS);
    }

    size_t pos = inFile.find(".");
    std::string outFile = inFile.substr(0, pos) + "_mac.txt";
    std::fstream out(outFile, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        Logger::getInstance().logError(1, "Error MenuOptions (mac) : Output file could not be opened!");
        return;
    }
    
    int key_length = 64;
    unsigned char* key = new unsigned char[key_length];
    operation.generateRandBytes(&key, key_length);
    unsigned char plaintext[1024];
    unsigned char mac[1024];
    unsigned int mac_length = 0;
    out.write("MAC : ", 6);
    while (in)
    {
    	in.read((char*)plaintext, 1024);
    	operation.generateHMAC_SHA256(key, reinterpret_cast<char*>(plaintext), mac, mac_length);
    	out.write((char*)mac, mac_length);
    }
    out.write("\nKEY : ", 7);
    out.write((char*)key, key_length);
    
    in.close();
    out.close();

    Server::getInstance().sendFile(outFile.c_str());
}

void MenuOptions::generateKeyOption(int option)
{
    Logger::getInstance().logMessageAndInt(1, "Execute Key generation with mode ", option);
    char nameBuff[256];
    int size = Server::getInstance().receiveData(nameBuff);
    nameBuff[size] = '\0';
    std::string name(nameBuff);

    int bits = Server::getInstance().receiveInt();

    EVP_PKEY* priv;
    EVP_PKEY* pub;

    if (operation.generateKeyPair(option, bits, priv, pub) == -1)
    {
        Server::getInstance().sendInt(OPERATION_FAILED);
        Logger::getInstance().logError(1, "Error MenuOptions (key) : failed to generate key.");
        return;
    }

    App::getInstance().addKeyPairToLoggedClient(name, priv, pub);
    
    Server::getInstance().sendInt(OPERATION_SUCCESS);
}

void MenuOptions::downloadKey()
{
    char nameBuff[256];
    int size = Server::getInstance().receiveData(nameBuff);
    nameBuff[size] = '\0';
    std::string name(nameBuff);

    GeneratedKeys* neededKey;
    bool found = false;
    for (const auto key : App::getInstance().getLoggedClient()->getKeyPairs())
    {
        if (key->getName() == name)
        {
            neededKey = key;
            found = true;
            break;
        }
    }
    if (found == false)
    {
        Logger::getInstance().logError(1, "Error MenuOptions (download key) : Key not found.");
        return;
    }

    // Serialize private key
    BIO* privBio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(privBio, neededKey->getPrivKey(), nullptr, nullptr, 0, nullptr, nullptr);
    int privLen = BIO_pending(privBio);
    char* privKey = new char[privLen];
    BIO_read(privBio, privKey, privLen);

    // Serialize public key
    BIO* pubBio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(pubBio, neededKey->getPubKey());
    int pubLen = BIO_pending(pubBio);
    char* pubKey = new char[pubLen];
    BIO_read(pubBio, pubKey, pubLen);

    // Send private key length and data
    Server::getInstance().sendData(privKey, privLen);
    // Send public key length and data
    Server::getInstance().sendData(pubKey, pubLen);

    // Clean up
    BIO_free(privBio);
    BIO_free(pubBio);
    delete[] privKey;
    delete[] pubKey;

    Logger::getInstance().logMessage(1, "Client downloaded the key.", "");
}

void MenuOptions::generateCertificateOption()
{
    Logger::getInstance().logMessage(1, "Execute certificate geneartion.", "");

    //first get the filename for certificate
    char nameForFile[256];
    int size = Server::getInstance().receiveData(nameForFile);
    nameForFile[size] = '.';
    nameForFile[size + 1] = 'p';
    nameForFile[size + 2] = 'e';
    nameForFile[size + 3] = 'm';
    nameForFile[size + 4] = '\0';

    //then the information about it-- key: 
    char kn[256];
    size = Server::getInstance().receiveData(kn);
    kn[size] = '\0';
    std::string keyName(kn);
   //country:
    char c[256];
    size = Server::getInstance().receiveData(c);
    c[size] = '\0';
    std::string country(c);
    //organization:
    char* o = new char[256];
    size = Server::getInstance().receiveData(o);
    o[size] = '\0';
    std::string organization(o);
    //common name:
    char* cn = new char[256];
    size = Server::getInstance().receiveData(cn);
    cn[size] = '\0';
    std::string commonName(cn);
    //validity:
    int valability = Server::getInstance().receiveInt();


    EVP_PKEY* cert_key = nullptr;
    for (const auto key : App::getInstance().getLoggedClient()->getKeyPairs())
    {
    	if (key->getName() == keyName)
        {
            cert_key = key->getPrivKey();
            break;
        }
    }
    if (cert_key == nullptr)
    {
        Server::getInstance().sendInt(OPERATION_FAILED);
        return;
    }
    X509* cert = operation.generateCertificate(cert_key, country, organization, commonName, valability);
    if (cert)
    {
        Server::getInstance().sendInt(OPERATION_SUCCESS);
    }
    else
    {
        Server::getInstance().sendInt(OPERATION_FAILED);
        return;
    }
    
    // Write certificate
    FILE* certFile = fopen(nameForFile, "wb");
    PEM_write_X509(certFile, cert);
    fclose(certFile);
    Server::getInstance().sendFile(nameForFile);
}