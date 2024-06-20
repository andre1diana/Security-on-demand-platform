#include "FilesOperations.h"
#include<string>
#include <openssl/pem.h>

unsigned char* FileOperations::readBinaryData(std::ifstream& f, size_t length) {
	unsigned char* buffer = new unsigned char[length];
	f.read(reinterpret_cast<char*>(buffer), length);
	return buffer;
}

EVP_PKEY* FileOperations::readPEMKey(std::ifstream& f) {
	std::string pemKey;
	std::string line;
	while (std::getline(f, line)) {
		pemKey += line + "\n";
		if (line == "-----END PRIVATE KEY-----" || line == "-----END PUBLIC KEY-----") {
			break;
		}
	}

	BIO* bio = BIO_new_mem_buf(pemKey.c_str(), -1);
	EVP_PKEY* key = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	if (!key) {
		BIO_free(bio);
		bio = BIO_new_mem_buf(pemKey.c_str(), -1);
		key = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
	}
	BIO_free(bio);
	return key;
}

void FileOperations::writePEMKey(EVP_PKEY* pkey, std::ofstream& f) {
	BIO* bio = BIO_new(BIO_s_mem());
	if (PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
		PEM_write_bio_PUBKEY(bio, pkey);
	}

	char* pemData;
	long pemLen = BIO_get_mem_data(bio, &pemData);
	f.write(pemData, pemLen);
	f << "\n";

	BIO_free(bio);
}

/* ---sertucture of the file
####
username
password
number of encrypted files
number of hashes
number of keys
number of certificates
------------------
filename original 1
filename encrypted 1
mode of encryption
key
iv
filename original 2
filename encrypted 2
...
------------------
filename of original file 1
hash of file 1
filename of original file 2
hash of file 2
...
------------------
name of key
priv key
pub key
------------------
filename of certificate 1
filename of certifiacte 2
...
####
*/

// function to help removing "\r" from strings
std::string removeCarriageReturn(const std::string& str) {
	std::string result = str;
	if (!result.empty() && result.back() == '\r') {
		result.pop_back();
	}
	return result;
}

void FileOperations::loadClientsFromFile(std::vector<Client*>& clients) {
	std::ifstream f(filename, std::ios::binary);
	if (!f.is_open())
		return;

	std::string username;
	std::string password;
	int nb_files;
	int nb_keys;
	int nb_hashes;
	int nb_certificates;

	while (!f.eof()) {
		std::getline(f, username);
		username = removeCarriageReturn(username);
		std::getline(f, password);
		password = removeCarriageReturn(password);
		f >> nb_files >> nb_hashes >> nb_keys >> nb_certificates;
		f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		Client* client = new Client(username, password);

		// Reading encrypted files
		std::string originalFile;
		std::string encryptedFile;
		std::string encryptionMode;
		unsigned char* key = nullptr;
		unsigned char* iv = nullptr;

		for (int i = 0; i < nb_files; i++) {
			std::getline(f, originalFile);
			originalFile = removeCarriageReturn(originalFile);
			std::getline(f, encryptedFile);
			encryptedFile = removeCarriageReturn(encryptedFile);
			std::getline(f, encryptionMode);
			encryptionMode = removeCarriageReturn(encryptionMode);

			int keyLen = 0;
			int ivLen = 0;

			if (encryptionMode == "AES_CBC") {
				keyLen = EVP_CIPHER_key_length(EVP_aes_256_cbc());
				ivLen = EVP_CIPHER_iv_length(EVP_aes_256_cbc());
			}
			else if (encryptionMode == "AES_OFB") {
				keyLen = EVP_CIPHER_key_length(EVP_aes_256_ofb());
				ivLen = EVP_CIPHER_iv_length(EVP_aes_256_ofb());
			}
			else if (encryptionMode == "3DES") {
				keyLen = EVP_CIPHER_key_length(EVP_des_ede3_cbc());
				ivLen = EVP_CIPHER_iv_length(EVP_des_ede3_cbc());
			}
			key = new unsigned char[keyLen];
			iv = new unsigned char[ivLen];
			key = readBinaryData(f, keyLen);
			iv = readBinaryData(f, ivLen);

			f.ignore(); // To consume the newline character after binary data

			client->addFile(originalFile, encryptedFile, key, iv, encryptionMode);
		}

		// Reading hashes
		std::string filename;
		std::string hash;
		for (int i = 0; i < nb_hashes; i++) {
			std::getline(f, filename);
			filename = removeCarriageReturn(filename);
			std::getline(f, hash);
			hash = removeCarriageReturn(hash);
			client->addHash(filename, hash);
		}

		// Reading key pairs
		std::string name;
		EVP_PKEY* private_key = nullptr;
		EVP_PKEY* public_key = nullptr;
		for (int i = 0; i < nb_keys; i++) {
			std::getline(f, name);
			name = removeCarriageReturn(name);
			private_key = readPEMKey(f);
			public_key = readPEMKey(f);

			client->addKeyPair(name, private_key, public_key);

			if (private_key) EVP_PKEY_free(private_key);
			if (public_key) EVP_PKEY_free(public_key);
		}

		std::string certificateFile;
		for (int i = 0; i < nb_certificates; i++) {
			std::getline(f, certificateFile);
			certificateFile = removeCarriageReturn(certificateFile);
			client->addCertificate(certificateFile);
		}

		clients.push_back(client);
	}

	f.close();
}

void FileOperations::loadClientsToFile(std::vector<Client*>& clients) {
	std::ofstream f(this->filename, std::ios::binary);
	if (!f.is_open())
		return;

	for (auto client : clients) {
		f << client->getUsername() << "\n";
		f << client->getPassword() << "\n";
		f << client->getFiles().size() << "\n";
		f << client->getHashes().size() << "\n";
		f << client->getKeyPairs().size() << "\n";
		f << client->getCertificates().size() << "\n";

		for (const auto& file : client->getFiles()) {

			f << file->getOriginalFile() << "\n";
			f << file->getEncFile() << "\n";
			f << file->getEncMode() << "\n";
			int keyLen;
			int ivLen;
			if (file->getEncMode() == "AES_CBC") {
				keyLen = EVP_CIPHER_key_length(EVP_aes_256_cbc());
				ivLen = EVP_CIPHER_iv_length(EVP_aes_256_cbc());
			}
			else if (file->getEncMode() == "AES_OFB") {
				keyLen = EVP_CIPHER_key_length(EVP_aes_256_ofb());
				ivLen = EVP_CIPHER_iv_length(EVP_aes_256_ofb());
			}
			else if (file->getEncMode() == "3DES") {
				keyLen = EVP_CIPHER_key_length(EVP_des_ede3_cbc());
				ivLen = EVP_CIPHER_iv_length(EVP_des_ede3_cbc());
			}
			f.write((char*)file->getKey(), keyLen);
			f.write((char*)file->getIV(), ivLen);
			f << "\n";
		}

		for (const auto& hash : client->getHashes()) {
			f << hash->getFilename() << "\n";
			f << hash->getHash() << "\n";
		}


		for (const auto& keyPair : client->getKeyPairs()) {
			f << keyPair->getName() << "\n";
			writePEMKey(keyPair->getPrivKey(), f);
			long pos = f.tellp();
			f.seekp(pos - 2);
			f.write("", 1);
			writePEMKey(keyPair->getPubKey(), f);
			pos = f.tellp();
			f.seekp(pos - 1);
			f.write("", 1);
		}

		std::vector<std::string>::iterator iter;
		iter = client->getCertificates().begin();
		for (const auto& certificate : client->getCertificates()) {
			f << certificate;
			iter++;
			if (iter != client->getCertificates().end())
				f << std::endl;
		}
	}

	f.close();
}
