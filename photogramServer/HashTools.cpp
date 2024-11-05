#include "HashTools.h"
#include <sodium.h>
#include <iostream>

HashTools::HashTools() {
	if (sodium_init() == -1) {
		throw std::runtime_error("Can't run HashTools");
	}
}

std::string hashPassword(const std::string password) {
	unsigned char hash[crypto_generichash_BYTES];
	crypto_generichash(hash, sizeof(hash), (const unsigned char*)password.c_str(), password.size(), NULL, 0);
	
	char hex_hash[crypto_generichash_BYTES * 2 + 1];
	sodium_bin2hex(hex_hash, sizeof(hex_hash), hash, sizeof(hash));

	return std::string(hex_hash);
}

bool HashTools::verify(std::string &password, std::string &hashPassword) {
	if (crypto_pwhash_str_verify(hashPassword.c_str(), password.c_str(), password.size()) == -1)
		return false;
	return true;
}