#include "Hashes.h"

void Hashes::setFilename(std::string file)
{
	filename = file;
}

void Hashes::setHash(std::string hash)
{
	this->hash = hash;
}

std::string Hashes::getFilename() const
{
	return filename;
}

std::string Hashes::getHash() const
{
	return hash;
}
