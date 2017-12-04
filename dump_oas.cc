/*

  KLayout Layout Viewer
  Copyright (C) 2013-2018 Matthias Koefferlein

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#include "dbOASISDumper.h"

#include <iostream>

const char *version = "0.2";

/**
 *  @brief Print usage
 */
void syntax ()
{
  std::cout << 
    "dump_oas - An OASIS file disassembly tool" << std::endl <<
    std::endl <<
    "Usage: dump_oas [options] [OASIS file]" << std::endl <<
    std::endl <<
    "Options:" << std::endl <<
    "  -n <width>     number of bytes to print per line" << std::endl <<
    "  -s             short: abbreviate hex dump with more than \"width\" bytes" << std::endl <<
    std::endl <<
    "Version " << version << std::endl <<
    std::endl <<
    "Author: Matthias Köfferlein, 2013" << std::endl <<
    "Distributed under GPL V2 or later" << std::endl;
}

/**
 *  @brief The main function
 */
int main (int argc, const char *argv[])
{
  try {

    bool short_mode = false;
    int width = 8;
    std::string input;

    for (int i = 1; i < argc; ++i) {
      std::string a = argv [i];
      if (a == "-h" || a == "--help") {
        syntax ();
        return 1;
      } else if (a == "-n" && i < argc - 1) {
        ++i;
        tl::from_string (argv [i], width);
        if (width < 1 || width > 100000) {
          throw tl::Exception (tl::translate ("Invalid width specification for -n command line option"));
        }
      } else if (a == "-s") {
        short_mode = true;
      } else if (a [0] == '-') {
        throw tl::Exception (tl::translate ("Unknown option ") + a);
      } else {
        input = a;
      }
    }

    if (input.empty ()) {
      throw tl::Exception (tl::translate ("Input file missing"));
    }

    tl::InputZLibFile file (input);

    db::OASISDumper dumper (file);
    dumper.short_mode (short_mode);
    dumper.set_width (width);
    dumper.dump ();

  } catch (tl::Exception &ex) {
    std::cerr << "*** ERROR: " << ex.msg () << std::endl;
    return 2;
  }

  return 0;
}

