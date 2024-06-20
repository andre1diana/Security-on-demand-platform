#pragma once
#include<string>

class Hashes
{
private:
	std::string filename;
	std::string hash;

public:
	Hashes(std::string f, std::string h) : filename(f), hash(h) {}
	~Hashes() {}

	void setFilename(std::string file);
	void setHash(std::string hash);
	std::string getFilename() const;
	std::string getHash() const;
};

