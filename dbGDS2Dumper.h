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



#ifndef HDR_dbGDS2Dumper
#define HDR_dbGDS2Dumper

#include "tlException.h"
#include "tlStream.h"
#include "dbTypes.h"
#include "dbPoint.h"

#include <map>
#include <set>
#include <stdint.h>

namespace db
{

class RecordDefinition;

/**
 *  @brief Generic base class of GDS2 reader exceptions
 */
class KLAYOUT_DLL GDS2DumperException
  : public tl::Exception
{
public:
  GDS2DumperException (const std::string &msg, size_t p, const std::string &cell)
    : tl::Exception (tl::sprintf (tl::translate ("%s (position=%ld, cell=%s)"), msg, p, cell))
  { }
};

/**
 *  @brief The GDS2 format stream reader
 */
class KLAYOUT_DLL GDS2Dumper
{
public: 
  /**
   *  @brief Construct a stream reader object
   *
   *  @param s The stream delegate from which to read stream data from
   */
  GDS2Dumper (tl::InputStreamBase &s);

  /**  
   *  @brief Destructor
   */
  ~GDS2Dumper ();

  /**
   *  @brief Set short mode
   */
  void short_mode (bool s)
  {
    m_short_mode = s;
  }

  /**
   *  @brief Set the number of bytes to show per line
   */
  void set_width (size_t w)
  {
    m_width = w;
  }

  /** 
   *  @brief The basic dumper method 
   */
  void dump ();

  /**
   *  @brief Issue an error with positional informations
   *
   *  Reimplements GDS2Diagnostics
   */
  void error (const std::string &txt);

  /**
   *  @brief Issue a warning with positional informations
   *
   *  Reimplements GDS2Diagnostics
   */
  void warn (const std::string &txt);

  //  public dumper targets
  void header (const RecordDefinition *record_def, uint16_t len);
  void layer (const RecordDefinition *record_def, uint16_t len);
  void datatype (const RecordDefinition *record_def, uint16_t len);
  void timestamp (const RecordDefinition *record_def, uint16_t len);
  void xy (const RecordDefinition *record_def, uint16_t len);
  void generic (const RecordDefinition *record_def, uint16_t len);

private:
  tl::InputStream m_stream;
  size_t m_last_emit;
  size_t m_width;
  bool m_short_mode;

  void emit (const std::string &msg);

  int32_t get_int32 ();
  uint32_t get_uint32 ();
  int16_t get_int16 ();
  uint16_t get_uint16 ();
  uint8_t get_uint8 ();
  std::string get_str (uint32_t len);
  double get_double ();
};

}

#endif

