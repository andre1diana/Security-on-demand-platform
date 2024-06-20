#include "EncFiles.h"

std::string EncFiles::getOriginalFile() const
{
    return originalFile;
}

std::string EncFiles::getEncFile() const
{
    return encryptedFile;
}

unsigned char* EncFiles::getKey() const
{
    return key;
}

unsigned char* EncFiles::getIV() const
{
    return iv;
}

std::string EncFiles::getEncMode() const
{
    return encriptionMode;
}
