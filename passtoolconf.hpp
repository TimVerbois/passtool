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
#include <map>
using namespace std;


class Config
{
	public:
		Config(string filename);

		string get(string parameter);
		int geti(string parameter);
		void write_to_file();
		void put(string parameter, string value);
		void set_RandomGenerator();
		void check_consistency();
		bool isRandom();

	private:
		void set_defaults();
		map<string,string> m_vParams;
		string m_sFilename;
		bool m_bRandom;
};
