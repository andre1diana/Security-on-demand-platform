#include "GeneratedKeys.h"

std::string GeneratedKeys::getName() const
{
    return name;
}

EVP_PKEY* GeneratedKeys::getPrivKey() const
{
    return private_key;
}

EVP_PKEY* GeneratedKeys::getPubKey() const
{
    return public_key;
}
