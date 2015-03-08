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

#include "passtoolconf.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define EXIT_FAILURE 1

Config::Config(string filename) 
{
	m_sFilename = filename;
	ifstream configfile(filename.c_str(), ios::in);
	if (configfile.is_open())
	{
		string parameter, value;
		if (!configfile.eof())
		{
			configfile >> parameter >> value;
			put(parameter, value);
		}
		configfile.close();
	}
	m_bRandom = false;
	set_defaults();
	check_consistency();

}

void Config::set_defaults()
{
	if (get("PasswordLength").compare("") == 0)
		put("PasswordLength", "8");
	if (get("MinLower").compare("") == 0)
		put("MinLower", "2");
	if (get ("MinUpper").compare("") == 0)
		put("MinUpper", "2");
	if (get ("MinNumber").compare("") == 0)
		put("MinNumber", "1");
	if (get ("MinOther").compare("") == 0)
		put("MinOther", "1");
	if (get ("MaxLower").compare("") == 0)
		put("MaxLower", "100");
	if (get ("MaxUpper").compare("") == 0)
		put("MaxUpper", "100");
	if (get ("MaxOther").compare("") == 0)
		put("MaxOther", "100");
	if (get ("MaxNumber").compare("") == 0)
		put("MaxNumber", "100");
	if (get ("KeyFile").compare("") == 0)
		put("KeyFile", "/etc/.passtool");
	if (get ("OtherCharacters").compare("") == 0)
		put ("OtherCharacters", "!()-_.?[]'~");
	if (get ("KeyFileRetention").compare("") == 0)
		put ("KeyFileRetention", "168");
}

void Config::set_RandomGenerator()
{
	put("StrictParameters", "1");
	m_bRandom = true;
	put("MaxLower", "0");
	put("MinLower", "0");
	put("MaxUpper", "0");
	put("MinUpper", "0");
	put("MaxOther", "0");
	put("MinOther", "0");
	put("MaxNumber", "0");
	put("MinNumber", "0");
}

void Config::check_consistency()
{
	int allconstraints = geti("MinLower") + geti("MinUpper") + geti("MinNumber") + geti("MinNumber") ;
	if (geti("PasswordLength") < allconstraints)
	{
		printf("Error: To many constraints for password size\n");
		exit (EXIT_FAILURE);
	}
	if (m_bRandom)
	{
		if ( (geti("MaxLower") == 0) && (geti("MaxUpper") == 0) && (geti("MaxOther") == 0) && (geti("MaxNumber") == 0) )
		{
			printf("Error: No allowed characters defined\n");
			exit (EXIT_FAILURE);
		}
	}
	if ( (geti("MinLength") > 0 ) && (geti("MaxLength") == -1 ) ) 
	{
		put("MaxLength", get("PasswordLength"));
	}
	if ( (geti("MinLength") > 0) && (geti("MaxLength") > 0) ) 
	{
		int password_length;
		if (geti("MaxLength") > geti("MinLength") )
		{
			int random ;
			if (m_bRandom)
			{
				srand(time(NULL));
				random=rand();
			}
			else
			{
				std::string server = get("ServerName");
				random=0;
				for (std::size_t i = 0; i < server.length(); i++)
				{
					int ascii = server[i];
					random += ascii;
				}
			}
			int dif = 1 + geti("MaxLength") - geti("MinLength") ;
			password_length = geti("MinLength") + (random%dif);
		}
		else
		{
			password_length = geti("MinLength");
		}
		stringstream passwd;
		passwd << password_length;
		put ("PasswordLength", passwd.str() );
	}
}

int Config::geti(std::string parameter)
{
	if (m_vParams.find(parameter) != m_vParams.end())
	{
		return atoi(m_vParams[parameter].c_str());
	}
	else
	{
		/*Not part of the map, so return something known*/
		return -1;
	}
}

std::string Config::get(std::string parameter)
{
	if (m_vParams.find(parameter) != m_vParams.end())
	{
		return m_vParams[parameter];
	}
	else
	{
		/*Not part of the map, so return something known*/
		return "";
	}
}

void Config::put(std::string parameter, std::string value)
{
	m_vParams[parameter] = value;
}

void Config::write_to_file()
{
	ofstream configfile(m_sFilename.c_str(), ios::out);
	if (configfile.is_open())
	{
		for ( map<string,string>::const_iterator iter = m_vParams.begin();iter != m_vParams.end(); ++iter )
		{
			string tabs = "";
			for (int i = iter->first.length(); i < 22 ; i++)
				tabs += " ";
			configfile << iter->first << tabs << iter->second << '\n';
		}
		configfile.close();
	}
}

bool Config::isRandom()
{
	return m_bRandom;
}
