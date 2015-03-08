/*
 **************************************************************************
 *                                                                        *
 * Code created by Tim Verbois                                            *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use under the Common Public License since parts of open source    *
 * code has been used                                                     *
 *                                                                        *
 * http://www.opensource.org/licenses/cpl.php                             *
 *                                                                        *
 **************************************************************************
*/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime> 
#include <sys/stat.h>
#include <fcntl.h>
#include "passtoolconf.hpp"
#include "key.hpp"


extern Config * config;
#include <cstdlib>
#include <ctime> 

Key::Key()
{
//	string home = getenv("HOME");
	m_sKeyFile = config->get("KeyFile");
}

string Key::encrypt_key(std::string msg, std::string key)
{
	// Make sure the key is at least as long as the message
	std::string tmp(key);
	while (key.size() < msg.size())
	key += tmp;
    	// And now for the encryption part
    	for (std::string::size_type i = 0; i < msg.size(); ++i)
    		msg[i] ^= key[i];
	return msg;
}

string Key::decrypt_key(std::string msg, std::string key) 
{
    return encrypt_key(msg, key); // lol
}

bool Key::load_from_keyfile()
{
	if (!keyfile_is_valid())
		return false;

    std::ifstream keyfile(m_sKeyFile.c_str(), ios::in|ios::binary|ios::ate);
	if (keyfile.is_open())
	{
		char * lengthb;
		int length;
//		int size = keyfile.tellg();
		char * memblock = new char [8];
		lengthb = new char[1];
		keyfile.seekg (8, ios::beg);
		keyfile.read (lengthb, 1);
		length=atoi(lengthb);
		if (length == 0)
			length=8;
		keyfile.seekg (0, ios::beg);
		keyfile.read (memblock, length);
		keyfile.close();
		string todecrypt;
		todecrypt.assign(memblock);
//		cout << "key=" << memblock << "\n";
		string decrypted = decrypt_key(todecrypt, make_encrypt_key());
		if (decrypted.length() == 0)
		{
//			cout << "bufferkey=" << memblock << " size=" << length << endl;
			set_key(decrypted);
		}
		else
		{
			cout << "Error decoding file\n" ;
			return false;
		}
	}
	else
	{
		std::cout << "No keyfile exists, using interactive mode\n" ;
		return false;
	}
	return true;
}

string Key::make_correct_length(string str, int length)
{
	for (int i = str.length(); i < length; i ++)
	{
		str+=" ";
	}
	return str.substr(0,length);
}

string Key::make_encrypt_key()
{
	string userstr;
	string encryptkey;
//	userstr=getenv("LOGNAME");
	stringstream ss; 
	ss << m_iKeyFileInode;
	encryptkey=ss.str();
//	encryptkey+=userstr;
	encryptkey=make_correct_length(encryptkey, 8);
	return encryptkey;

}

bool Key::ask_key()
{
	string key, key2;
	key = getpass("Key: ");
	key2 = getpass("Retype Key: ");
	if ( key != key2 )
	{
		cout << "Error: Retype of key not the same as first time \n" ;
		return false;
	}
	if (key.compare("") == 0)
	{
		cout << "Error: Key cannot be empty\n";
		return false;
	}
	if (key.length() > 8)
	{
		cout << "Error: Key cannot be longer than 8 characters\n";
		return false;
	}
	set_key(key);
	return true;
	
}

bool Key::random_key()
{
	char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	srand(time(0));
	int length = strlen(chars);

	string key="";
	key += chars[rand()%length];
	key += chars[rand()%length];
	key += chars[rand()%length];
	key += chars[rand()%length];
	key += chars[rand()%length];
	key += chars[rand()%length];
	key += chars[rand()%length];
	key += chars[rand()%length];

	set_key(key);
	return true;

}

bool Key::write_to_keyfile()
{
    ofstream keyfile(m_sKeyFile.c_str(), ios::out|ios::binary);
	if (keyfile.is_open())
	{
		string key = m_sKey;
		int fd;
		if ((fd = creat(m_sKeyFile.c_str(), S_IWUSR)) < 0 )
		{
			cout << "Error opening file " << m_sKeyFile ;
			return false;
		}
		else
		{
			if (fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP) != 0 )
			{
				cout << "Error putting the correct permissions" ;
				return false;
			}
			
			m_iKeyFileInode = get_keyfile_info("inode");
//			cout << "inode= " << m_iKeyFileInode << "\n";
			close (fd);
		}

		int length = key.length();
		for (int i = key.length(); i < 8; i++)
		{
			key +=  " ";
		}
//		cout << "key = /" << key << "/\n";

		cout << "Generating keyfile ... ";

		string encrypted = encrypt_key(key, make_encrypt_key());
		if ( encrypted.length() != 0 ) 
		{
//			cout << "key=/" << toencrypt << "/\n";
			keyfile << encrypted;
			keyfile << length;
			cout << "Done\n";
		}
		else
		{
			cout << "Error";
			return false;
		}
		keyfile.close();
	}
	else
	{
		cout << "Error creating keyfile " << m_sKeyFile <<"\n";
		return false;
	}

	return true;

}

string Key::get_key()
{
	return m_sKey;
}

void Key::set_key(string key)
{
	m_sKey = key;
}

int Key::get_keyfile_info(string param)
{
    int fd;
	int value;
	struct stat info;
	if ((fd = open(m_sKeyFile.c_str(), O_RDONLY)) >= 0 )
	{
		stat(m_sKeyFile.c_str(), &info);
		if ( param.compare("inode") == 0 )
		{
			value=info.st_ino;
		}
		else
			if ( param.compare("mode") == 0 )
			{
				value=info.st_mode;
			}
			else
				if ( param.compare("mtime") == 0 )
				{
					value=info.st_mtime;
				}


		close (fd);
	}
	return value;

}

bool Key::keyfile_is_valid()
{
	int mode;
	if ((mode=get_keyfile_info("mode")) != 0 )
	{
		if (mode != 33184)
		{
			cout << "Error: incorrect permissions on keyfile!!\n\n" ;
			return false;
		}

/*		time_t lastweek = time(NULL) - config->geti("KeyFileRetention")*60*60;

		if (get_keyfile_info("mtime") < lastweek)
		{
			std::cout << "Keyfile is to old, please regenerate!!\n";
			return false;
		}
*/
		m_iKeyFileInode = get_keyfile_info("inode");
	}
	return true;
}

bool Key::delete_keyfile()
{
	if ((remove (m_sKeyFile.c_str())) != 0 )
	{
		cout << "\nError removing keyfile\n" ;
        return false;
	}
	else
		return true;
}

