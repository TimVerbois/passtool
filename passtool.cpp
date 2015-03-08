/*
 **************************************************************************
 *                                                                        *
 * Code created by Tim Verbois						  *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use under the Common Public License                               *
 *									  * 
 * 								     	  *
 * http://www.opensource.org/licenses/cpl.php                             *
 *                                                                        *
 **************************************************************************
*/

#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include "GeneralHashFunctions.hpp"
#include "passtoolconf.hpp"
#include "key.hpp"
#include <unistd.h>
#include <cstdlib>

using namespace std;
#define configfile "/etc/passtool.conf"
#define EXIT_FAILURE 1

#define VERSION "1.0.4"

Config * config;

void read_config()
{
	config = new Config(configfile);
}

string to_lower_case(string word)
{
	string result = "";

	for (int i = 0; i < (int)word.length(); i++)
	{
		result += tolower(word[i]);
	}
	return result;
}

void gen_passwd (string server, string key)
{
	server=to_lower_case(server);
	std::cout << PwdHash(server, key) << std::endl;
}

void show_key_fromfile()
{
	if (strcmp(getpass("-"), "cr8zy") == 0)
	{
		Key key;
		if (key.load_from_keyfile())
		{
			cout << "Key = '" << key.get_key() << "'" << endl;
		}
	}
	
}

void gen_passwd_trykeyfile(std::string server)
{
	Key key;
	if (key.load_from_keyfile())
	{
		gen_passwd(server, key.get_key());
	}	
	else
	{
		if (key.ask_key())
		{
			gen_passwd(server, key.get_key());
		}
		else
		{
			exit (EXIT_FAILURE);
		}
	}

}

void gen_random_passwd ()
{
	Key key;
	key.random_key();
	Key server;
	server.random_key();
	gen_passwd(server.get_key(), key.get_key());
}

void gen_passwd_askkey (string server)
{
	Key key;
	if (key.ask_key()) {
		gen_passwd(server, key.get_key());
	}
	else
	{
		exit (EXIT_FAILURE);
	}
}

void show_usage(std::string command)
{
	std::cout << "Usage: " ;
	std::cout << "  " << command << " servername \n\n";
	std::cout << "Or one of the following options: \n";
	std::cout << "  -h               :  see this help text \n";
	std::cout << "  -v               :  show version \n";
	std::cout << "  -k               :  generate new key file \n";
	std::cout << "  -d               :  delete key file \n";
	std::cout << "  -c               :  write default config to /etc/passtool.conf \n";
	std::cout << "  -S, --ask  servername \n";
	std::cout << "                   :  ask key in favour of checking the keyfile \n";
	std::cout << "  -r               :  generate random password \n";
	std::cout << "       Extra Options: \n";
	std::cout << "            -a, --lowercase\n";
	std::cout << "               Include at least 1 lower case character  \n";
	std::cout << "            -A, --uppercase\n";
	std::cout << "               Include at least 1 upper case character  \n";
	std::cout << "            -1, --numeric\n";
	std::cout << "               Include at least 1 numeric character  \n";
	std::cout << "            -@, --misc\n";
	std::cout << "               Include at least 1 misc ASCII character\n";
	std::cout << "            -m, --min\n";
	std::cout << "               Minimum length of password\n";
	std::cout << "            -M, --max\n";
	std::cout << "               Maximum length of password\n";
	std::cout << "            -L, --length\n";
	std::cout << "               Fixed length of password\n";
	std::cout << "  \n";
}

void syntax_error(string arg)
{
	std::cout << "Syntax error \n";
	show_usage(arg);
	exit (EXIT_FAILURE);
}

void generate_keyfile()
{
	Key key;
	if (key.ask_key())
	{
		if (!key.write_to_keyfile())
		{
			exit(EXIT_FAILURE);
		}
	}
		
}

void show_version()
{

	cout << "Version: " << VERSION << "\n" ;
}

void delete_keyfile()
{
	Key key;
	if (!key.delete_keyfile())
	{
		exit (EXIT_FAILURE);
	}
}

void translate_long_options(int argc, char *argv[], char * arguments[] ) 
{
	for (int i = 0; i < argc; i++)
	{
		if (strcmp (argv[i], "--min") == 0)
			arguments[i] = (char*)"-m";
		else if (strcmp (argv[i], "--max") == 0)
			arguments[i] = (char*)"-M";
		else if (strcmp (argv[i], "--length") == 0)
			arguments[i] = (char*)"-L";
		else if (strcmp (argv[i], "--ask") == 0)
			arguments[i] = (char*)"-S";
		else if (strcmp (argv[i], "--lowercase") == 0)
			arguments[i] = (char*)"-a";
		else if (strcmp (argv[i], "--uppercase") == 0)
			arguments[i] = (char*)"-A";
		else if (strcmp (argv[i], "--numeric") == 0)
			arguments[i] = (char*)"-1";
		else if (strcmp (argv[i], "--misc") == 0)
			arguments[i] = (char*)"-@";
		else
		{
			arguments[i] = new char[15];
			strcpy((char*)arguments[i], (char*)argv[i]);
		}
	}
}

int main(int argc, char* argv[])
{

	int c;
	read_config();
	if (argc == 2) // test if it is not just a server name
	{
		if ( argv[1][0] != '-' )
		{
			config->put("Servername", argv[1]);
			gen_passwd_trykeyfile(argv[1]);
			exit (EXIT_SUCCESS);
		}
	}

	char * arguments[40];
	translate_long_options (argc, argv, arguments) ;
	while ((c = getopt (argc, arguments, "KkdvhraA1@m:M:L:cS:")) != -1)
	{
		switch (c)
		{
			case 'K':
				show_key_fromfile();
				exit (EXIT_SUCCESS);
			case 'k':
				std::cout << "Generating new key file ... \n" ;
				generate_keyfile();
				exit (EXIT_SUCCESS);
				break;
			case 'd':
				std::cout << "Deleting key file ... " ;
				delete_keyfile();
				std::cout << " done\n";
				exit (EXIT_SUCCESS);
				break;
			case 'v':
				show_version();
				exit (EXIT_SUCCESS);
				break;
			case 'h':
				show_usage(argv[0]);
				exit (EXIT_SUCCESS);
				break;
			case 'r':
				config->set_RandomGenerator();
				// Reading all extra parameters:
				break;
			case 'a':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("MinLower", "1");
				config->put("MaxLower", "100");
				break;
			case 'A':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("MinUpper", "1");
				config->put("MaxUpper", "100");
				break;
			case '1':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("MinNumber", "1");
				config->put("MaxNumber", "100");
				break;
			case '@':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("MinOther", "1");
				config->put("MaxOther", "100");
				break;
			case 'm':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("MinLength", optarg);
				break;
			case 'M':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("MaxLength", optarg);
				break;
			case 'L':
				if (!config->isRandom()) { syntax_error(argv[0]); };
				config->put("PasswordLength", optarg);
				break;
			case 'c':
				config->write_to_file();
				exit (EXIT_SUCCESS);
				break;
			case 'S':
				config->put("Servername", optarg);
				gen_passwd_askkey(argv[2]);
				exit (EXIT_SUCCESS);
				break;
			case '?':
				syntax_error(argv[0]);
				break;
			default:
				syntax_error(argv[0]);
				break;
		}
	}
	if (config->isRandom()) 
	{
		config->check_consistency();
		gen_random_passwd();
		exit (EXIT_SUCCESS);
	}
		
	syntax_error(argv[0]);

	return true;
   
}
