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


#ifndef HDR_tlInternational
#define HDR_tlInternational

#include "config.h"

#include <string>
#include <QtCore/QString>

namespace tl
{

/**
 *  @brief Translate a string into the current locale
 *
 *  The intention of this method is to translate the
 *  given string into the current locale.
 * 
 *  @param s The string to translate
 *
 *  @return The translated string
 */
KLAYOUT_DLL const std::string &translate (const std::string &s);

/**
 *  @brief Convert a UTF8 std::string to a QString
 */
KLAYOUT_DLL QString to_qstring (const std::string &s);

/**
 *  @brief Convert a QString to a UTF8 std::string
 */
KLAYOUT_DLL std::string to_string (const QString &s);

#ifndef _WIN32
/**
 *  @brief Convert a system encoding std::string to a UTF8 std::string
 */
KLAYOUT_DLL std::string system_to_string (const std::string &s);

/**
 *  @brief Convert a UTF8 string to a system encoding string
 */
KLAYOUT_DLL std::string string_to_system (const std::string &s);
#endif

/**
 *  @brief Initialize the codecs
 */
KLAYOUT_DLL void initialize_codecs ();

}

#endif

