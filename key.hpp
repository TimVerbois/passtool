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

#include <string>
using namespace std;

class Key
{
	public:
		Key();
		bool load_from_keyfile();
		bool write_to_keyfile();
		string get_key();
		bool ask_key();
		bool random_key();
		void set_key(string key);
		bool delete_keyfile();

	private:
		bool keyfile_is_valid();
		string encrypt_key (string msg, string key);
		string decrypt_key (string msg, string key);
		int get_keyfile_info(string param);
		string make_correct_length(string str, int length);
		string make_encrypt_key();
		string m_sKey;
		string m_sKeyFile;
		int m_iKeyFileInode;

};
