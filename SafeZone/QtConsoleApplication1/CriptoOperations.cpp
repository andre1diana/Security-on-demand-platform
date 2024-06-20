#include "CriptoOperations.h"
#include "Logger.h"

#include <string>
#include <iostream>
#include <fstream>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/hmac.h>
#include "Codes.h"

void CriptoOperations::generateRandBytes(unsigned char** buff, int Length){
    RAND_bytes(*buff, Length);
}

void CriptoOperations::saveToFile(const unsigned char* key,
    const unsigned char* iv, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        Logger::getInstance().logError(1, "Error CriptoOperations (saveToFile) : Failed to open file for writing.");
        return;
    }
    file.write((const char*)key, strlen((const char*)key));
    file.write((const char*)iv, strlen((const char*)iv));
}

void CriptoOperations::encrypt(int mode, const unsigned char* plaintext, int pl_len,
    unsigned char* ciphertext, int& cp_len, const unsigned char* key, const unsigned char* iv)
{
    Logger* log = &Logger::getInstance();
    ctx_enc = EVP_CIPHER_CTX_new();
    if (ctx_enc == nullptr) {
        log->logError(1, "Error CriptoOperations (encrypt) : Failed to create EVP_CIPHER_CTX.");
    }

    const EVP_CIPHER* cipher = nullptr;
    switch (mode) {
    case AES_CBC_ENC: //AES_CBC
        cipher = EVP_aes_256_cbc();
        break;
    case AES_OFB_ENC: //AES_OFB_enc:
        cipher = EVP_aes_256_ofb();
        break;
    case _3DES_ENC: //_3DES_enc:
        cipher = EVP_des_ede3_cbc();
        break;
    default:
        log->logError(1, "Error CriptoOperations (encrypt) : Unknown encryption type.");
        return;
    }

    if (!EVP_EncryptInit(ctx_enc, cipher, key, iv)) {
        EVP_CIPHER_CTX_free(ctx_enc);
        log->logError(1, "Error CriptoOperations(encrypt) : Failed encrypt init.");
        return;
    }

    cp_len = 0;
    int len;

    if (!EVP_EncryptUpdate(ctx_enc, ciphertext, &len, plaintext, pl_len)) {
        EVP_CIPHER_CTX_free(ctx_enc);
        log->logError(1, "Error CriptoOperations (encrypt) : Failed encrypt update.");
        return;
    }
    cp_len += len;

    if (!EVP_EncryptFinal(ctx_enc, ciphertext + cp_len, &len)) {
        EVP_CIPHER_CTX_free(ctx_enc);
        log->logError(1, "Error CriptoOperations (encrypt) : Failed encrypt final.");
        return;
    }
    cp_len += len;
    EVP_CIPHER_CTX_free(ctx_enc);

}

void CriptoOperations::decrypt(int mode, const unsigned char* ciphertext, int cp_len, unsigned char* plaintext, int& pl_len, const unsigned char* key, const unsigned char* iv)
{
    Logger* log = &Logger::getInstance();
    ctx_enc = EVP_CIPHER_CTX_new();
    if (ctx_enc == nullptr) {
        log->logError(1, "Error CriptoOperations (decrypt) : Failed to create EVP_CIPHER_CTX");
    }

    const EVP_CIPHER* cipher = nullptr;
    switch (mode) {
    case AES_CBC_ENC: //AES_CBC
        cipher = EVP_aes_256_cbc();
        break;
    case AES_OFB_ENC: //AES_OFB_enc:
        cipher = EVP_aes_256_ofb();
        break;
    case _3DES_ENC: //_3DES_enc:
        cipher = EVP_des_ede3_cbc();
        break;
    default:
        log->logError(1, "Error CriptoOperations (decrypt) : Unknown encryption type.");
        return;
    }

    if (!EVP_DecryptInit(ctx_enc, cipher, key, iv)) {
        EVP_CIPHER_CTX_free(ctx_enc);
        log->logError(1, "Error CriptoOperations (decrypt) : EVP_DecryptInit failed.");
        return;
    }

    pl_len = 0;
    int len;

    if (!EVP_DecryptUpdate(ctx_enc, plaintext, &len, ciphertext, cp_len)) {
        EVP_CIPHER_CTX_free(ctx_enc);
        log->logError(1, "Error CriptoOperations (decrypt) : EVP_DecryptUpdate failed.");
        return;
    }
    pl_len += len;

    if (!EVP_DecryptFinal(ctx_enc, plaintext + pl_len, &len)) {
        EVP_CIPHER_CTX_free(ctx_enc);
        ERR_print_errors_fp(stderr);
        log->logError(1, "Error CriptoOperations (decrypt) : EVP_DecryptFinal failed.");
    }
    pl_len += len;

    EVP_CIPHER_CTX_free(ctx_enc);
}

void CriptoOperations::hash(int mode, const unsigned char* plaintext, unsigned int& inLen, unsigned char* hash_output,
    unsigned int& lengthOfHash)
{
    Logger* log = &Logger::getInstance();
    ctx_hash = EVP_MD_CTX_new();
    if (ctx_hash == nullptr) {
        log->logError(1, "Error CriptoOperations (hash) : Failed to create EVP_MD_CTX.");
    }

    const EVP_MD* md = nullptr;
    switch (mode) {
    case SHA1_OPT: // SHA1
        md = EVP_sha1();
        break;
    case SHA2_OPT: // SHA256
        md = EVP_sha256();
        break;
    case SHA3_OPT: // SHA3_256
        md = EVP_sha3_256();
        break;
    default:
        log->logError(1, "Error CriptoOperations (hash) : Unknown hash type.");
        return;
    }

    if (EVP_DigestInit_ex(ctx_hash, md, nullptr) != 1) {
        EVP_MD_CTX_free(ctx_hash);
        log->logError(1, "Error CriptoOperations (hash) : Failed to initialize digest.");
    }

    if (EVP_DigestUpdate(ctx_hash, plaintext, inLen) != 1) {
        EVP_MD_CTX_free(ctx_hash);
        log->logError(1, "Error CriptoOperations (hash) : Failed to update digest.");
    }

    if (EVP_DigestFinal_ex(ctx_hash, hash_output, &lengthOfHash) != 1) {
        EVP_MD_CTX_free(ctx_hash);
        log->logError(1, "Error CriptoOperations (hash) : Failed to finalize digest.");
    }
    EVP_MD_CTX_free(ctx_hash);
}

void CriptoOperations::generateHMAC_SHA256(const unsigned char* key, const std::string& plaintext, unsigned char* mac_output, unsigned int& mac_length)
{
    int key_length = strlen((char*)key);

    if (!HMAC(EVP_sha256(), key, key_length,
        reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length(),
        mac_output, &mac_length)) {
        Logger::getInstance().logError(1, "Error CriptoOperations (MAC) : Failed to generate HMAC");
    }
}

//keys + certificates
int CriptoOperations::generateKeyPair(int mode, int bits, EVP_PKEY*& private_key, EVP_PKEY*& public_key)
{
    private_key = EVP_PKEY_new();
    switch (mode) {
    case RSA_OPT: // RSA
        ctx_key = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        break;
    case DSA_OPT: // DSA
        ctx_key = EVP_PKEY_CTX_new_id(EVP_PKEY_DSA, nullptr);
        break;
    default:
        Logger::getInstance().logError(1, "Error CriptoOperations (Key) : Unknown key generation mode.");
        return -1;
    }

    if (!ctx_key) {
        Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_CTX_new_id error: ");
        Logger::getInstance().logError(1, ERR_error_string(ERR_get_error(), nullptr));
        return -1;
    }

    if (EVP_PKEY_keygen_init(ctx_key) <= 0) {
        Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_keygen_init error: ");
        Logger::getInstance().logError(1, ERR_error_string(ERR_get_error(), nullptr));
        EVP_PKEY_CTX_free(ctx_key);
        return -1;
    }

    if ((mode == RSA_OPT && EVP_PKEY_CTX_set_rsa_keygen_bits(ctx_key, bits) <= 0) ||
        (mode == DSA_OPT && EVP_PKEY_CTX_set_dsa_paramgen_bits(ctx_key, bits) <= 0)) {
        Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_CTX_set_keygen_bits error: ");
        Logger::getInstance().logError(1, ERR_error_string(ERR_get_error(), nullptr));
        EVP_PKEY_CTX_free(ctx_key);
        return -1;
    }

    if (mode == DSA_OPT) {
        EVP_PKEY_CTX* param_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_DSA, nullptr);
        if (!param_ctx || EVP_PKEY_paramgen_init(param_ctx) <= 0) {
            Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_paramgen_init error: ");
            Logger::getInstance().logError(1, ERR_error_string(ERR_get_error(), nullptr));
            if (param_ctx) EVP_PKEY_CTX_free(param_ctx);
            EVP_PKEY_CTX_free(ctx_key);
            return -1;
        }

        if (EVP_PKEY_CTX_set_dsa_paramgen_bits(param_ctx, bits) <= 0) {
            Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_CTX_set_dsa_paramgen_bits error: ");
            Logger::getInstance().logError(1, ERR_error_string(ERR_get_error(), nullptr));
            EVP_PKEY_CTX_free(param_ctx);
            EVP_PKEY_CTX_free(ctx_key);
            return -1;
        }

        EVP_PKEY* params = nullptr;
        if (EVP_PKEY_paramgen(param_ctx, &params) <= 0) {
            Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_paramgen error: ");
            Logger::getInstance().logError(1, ERR_error_string(ERR_get_error(), nullptr));
            EVP_PKEY_CTX_free(param_ctx);
            EVP_PKEY_CTX_free(ctx_key);
            return -1;
        }

        EVP_PKEY_CTX_free(param_ctx);

        EVP_PKEY_CTX* keygen_ctx = EVP_PKEY_CTX_new(params, nullptr);
        EVP_PKEY_free(params);
        if (!keygen_ctx || EVP_PKEY_keygen_init(keygen_ctx) <= 0) {
            Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_keygen_init (DSA).");
            if (keygen_ctx) EVP_PKEY_CTX_free(keygen_ctx);
            EVP_PKEY_CTX_free(ctx_key);
            return -1;
        }

        if (EVP_PKEY_keygen(keygen_ctx, &private_key) <= 0) {
            Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_keygen.");
            EVP_PKEY_CTX_free(keygen_ctx);
            EVP_PKEY_CTX_free(ctx_key);
            return -1;
        }

        EVP_PKEY_CTX_free(keygen_ctx);
    }
    else {
        if (EVP_PKEY_keygen(ctx_key, &private_key) <= 0) {
            Logger::getInstance().logError(1, "Error CriptoOperations (Key) : EVP_PKEY_keygen.");
            EVP_PKEY_CTX_free(ctx_key);
            return -1;
        }
    }

    // Generate the public key from the private key
    public_key = EVP_PKEY_new();
    if (mode == RSA_OPT) {
        EVP_PKEY_set1_RSA(public_key, EVP_PKEY_get1_RSA(private_key));
    }
    else if (mode == DSA_OPT) {
        EVP_PKEY_set1_DSA(public_key, EVP_PKEY_get1_DSA(private_key));
    }

    EVP_PKEY_CTX_free(ctx_key);
    return 1;
}

X509* CriptoOperations::generateCertificate(EVP_PKEY* pkey, const std::string& country, const std::string& organization, const std::string& commonName, int days)
{
    X509* x509 = X509_new();
    if (!x509) {
        Logger::getInstance().logError(1, "Error CriptoOperations (Certificate) : Failed to create X509 structure");
        return nullptr;
    }
    //Version
    X509_set_version(x509, 2);
    //Serie
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    //Validity
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), days * 24 * 60 * 60);

    //subject
    X509_NAME* name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char*)country.c_str(), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (const unsigned char*)organization.c_str(), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char*)commonName.c_str(), -1, -1, 0);

    //issuer
    X509_set_issuer_name(x509, name);

    //public key
    X509_set_pubkey(x509, pkey);

    //signing
    if (!X509_sign(x509, pkey, EVP_sha256())) {
        Logger::getInstance().logError(1, "Error CriptoOperations (Certificate) : Failed to sign certificate");
        X509_free(x509);
        return nullptr;
    }

    return x509;
}