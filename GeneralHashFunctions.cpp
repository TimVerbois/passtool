#include "GeneralHashFunctions.hpp"
#include <iostream>
#include "passtoolconf.hpp"

// This code is based on the code provided on the website:
// http://www.partow.net/programming/hashfunctions/
// Edited and changed by Tim Verbois

extern Config * config;

std::vector<int> fillPrimes()
{
   std::vector<int> primes;
   primes.push_back(5);
   primes.push_back(67);
   primes.push_back(13);
   primes.push_back(47);
   primes.push_back(127);
   primes.push_back(29);
   primes.push_back(31);
   primes.push_back(11);
   primes.push_back(37);
   primes.push_back(131);
   primes.push_back(7);
   primes.push_back(41);
   primes.push_back(101);
   primes.push_back(43);
   primes.push_back(107);
   primes.push_back(59);
   primes.push_back(2);
   primes.push_back(61);
   primes.push_back(109);
   primes.push_back(3);
   primes.push_back(71);
   primes.push_back(19);
   primes.push_back(79);
   primes.push_back(17);
   primes.push_back(113);
   primes.push_back(83);
   primes.push_back(23);
   primes.push_back(97);
   primes.push_back(73);
   primes.push_back(89);
   primes.push_back(103);
   primes.push_back(53);
   primes.push_back(137);
	return primes;
}

long mangleKey(long hash, std::vector<int> primes, int password_length)
{
	long last2=hash;
	long factor=password_length*10;
	for (int i = 0; i < password_length; i+=2)
	{
		int prime=(int)last2;
		for (int j = 0; j < i; j+=2)
		{
			prime=primes[prime%primes.size()];
		}
		hash+=prime*factor;
		factor=factor/100;
	}
	return hash;
}

int get_char_listid(std::vector<std::vector <int> > pos, int position)
{
	for (int i = 0 ; i < (int)pos.size(); i++)
	{
		for (int j = 0; j < (int)pos.at(i).size(); j++)
		{
			if (pos.at(i).at(j) == position)
			{
				return i;
			}	
		}
	}
	return pos.size()-1;
}

std::string Replace(std::string& context, const std::string& from, const std::string& to) 
{
	size_t lookHere = 0;
	size_t foundHere;
	while((foundHere = context.find(from, lookHere)) != string::npos)
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}
	return context;


}

std::string PwdHash(const std::string& str, const std::string& key)
{
	unsigned long hash = 0xAAAAAAAA;
	unsigned long hash_key = 0xAAAAAAAA;

	std::vector<std::string> chars;
	std::string temp_chars;
	chars.push_back("abcdefghijklmnopqrstuvwxyz");
	chars.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	chars.push_back("!()-_.?[]'~");
	chars.push_back("0123456789");
	if (config->geti("MaxLower") > 0)
		temp_chars += chars.at(0);
	if (config->geti("MaxUpper") > 0)
		temp_chars += chars.at(1);
	if (config->geti("MaxOther") > 0)
		temp_chars += chars.at(2);
	if (config->geti("MaxNumber") > 0)
		temp_chars += chars.at(3);
	chars.push_back(temp_chars);

	std::vector<int> mins;
	mins.push_back(config->geti("MinLower"));  // min small letters
	mins.push_back(config->geti("MinUpper"));  // min capital letters
	mins.push_back(config->geti("MinOther"));  // Min signs
	mins.push_back(config->geti("MinNumber"));  // min numbers
	mins.push_back(0);  // min rest
	int password_length = config->geti("PasswordLength"); // Password length

	std::vector<int> positions;
	for (int i = 0; i < password_length; i ++)
	{
		positions.push_back(i);
	}

	std::vector<int> primes = fillPrimes();
	for(std::size_t i = 0; i < str.length(); i++)
	{
		hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ str[i] ^ (hash >> 3)) :
								(~((hash << 11) ^ str[i] ^ (hash >> 5)));
	}

	for(std::size_t i = 0; i < key.length(); i++)
	{
		hash_key ^= ((i & 1) == 0) ? (  (hash_key <<  7) ^ key[i] ^ (hash_key >> 3)) :
								(~((hash_key << 11) ^ key[i] ^ (hash_key >> 5)));
	}

	std::vector<std::vector<int> > pos;
	hash=mangleKey(hash, primes, password_length);
	hash_key=mangleKey(hash_key,primes, password_length);

	// Determine the place of the obliged characters
	for (int i = 0; i < (int)chars.size(); i++)
	{
		std::vector<int> temp;
		for (int j = 0; j < mins.at(i); j++)
		{
			int position = (primes.at(hash%primes.size()) + hash_key)%positions.size();
			temp.push_back( positions.at(position));
			positions.erase(positions.begin()+position);
		}
		pos.push_back(temp);
	}

	long starthash=hash;
	long starthash_key = hash_key;
	std::string password = "";
	for (int i = 0; i < password_length; i++)
	{
		int listid = get_char_listid(pos, i);
		int position=(primes[hash%primes.size()]*hash_key)%chars.at(listid).length();
		password += chars.at(listid).at(position);
		if (hash > 10)
			hash=hash/10;
		else
			hash=starthash;
		if (hash_key > 10)
			hash_key=hash_key/10;
		else
			hash_key=starthash_key;
   }

   password = Replace(password, "~.", "t.");
   password = Replace(password, "~?", "t?");
   password = Replace(password, "~B", "tB");
   password = Replace(password, "~C", "tC");
   password = Replace(password, "~R", "tR");
   password = Replace(password, "~#", "t#");
   password = Replace(password, "~&", "t&");
   password = Replace(password, "~~", "tt");

   return password;
}

