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



#ifndef HDR_dbOASISDumper
#define HDR_dbOASISDumper

#include "tlException.h"
#include "tlStream.h"
#include "dbTypes.h"
#include "dbPoint.h"

#include <map>
#include <set>

namespace db
{

/**
 *  @brief Generic base class of OASIS reader exceptions
 */
class KLAYOUT_DLL OASISDumperException
  : public tl::Exception
{
public:
  OASISDumperException (const std::string &msg, size_t p, const std::string &cell)
    : tl::Exception (tl::sprintf (tl::translate ("%s (position=%ld, cell=%s)"), msg, p, cell))
  { }
};

/**
 *  @brief The OASIS format stream reader
 */
class KLAYOUT_DLL OASISDumper
{
public: 
  /**
   *  @brief Construct a stream reader object
   *
   *  @param s The stream delegate from which to read stream data from
   */
  OASISDumper (tl::InputStreamBase &s);

  /**  
   *  @brief Destructor
   */
  ~OASISDumper ();

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
   *  Reimplements OASISDiagnostics
   */
  void error (const std::string &txt);

  /**
   *  @brief Issue a warning with positional informations
   *
   *  Reimplements OASISDiagnostics
   */
  void warn (const std::string &txt);

private:
  tl::InputStream m_stream;
  size_t m_last_emit;
  size_t m_width;
  bool m_short_mode;

  void do_read ();
  void do_read_cell ();
  void do_read_placement (unsigned int r);

  void do_read_text ();
  void do_read_rectangle ();
  void do_read_polygon ();
  void do_read_path ();
  void do_read_trapezoid (unsigned char r);
  void do_read_ctrapezoid ();
  void do_read_circle ();

  void read_repetition ();
  void read_pointlist ();
  void read_properties ();
  void read_element_properties ();

  void emit (const std::string &msg);

  unsigned char get_byte ()
  {
    unsigned char *b = (unsigned char *) m_stream.get (1);
    if (! b) {
      error (tl::translate ("Unexpected end-of-file"));
      return 0;
    } else {
      return *b;
    }
  }

  long long get_long_long ();
  unsigned long long get_ulong_long ();
  long get_long ();
  unsigned long get_ulong ();
  int get_int ();
  unsigned int get_uint ();

  void get (long long &l)
  {
    l = get_long_long ();
  }

  void get (unsigned long long &l)
  {
    l = get_ulong_long ();
  }

  void get (long &l)
  {
    l = get_long ();
  }

  void get (unsigned long &l)
  {
    l = get_ulong ();
  }

  void get (int &l)
  {
    l = get_int ();
  }

  void get (unsigned int &l)
  {
    l = get_uint ();
  }

  void get (double &d)
  {
    d = get_real ();
  }

  std::string get_str ();
  void get_str (std::string &s);
  double get_real ();
  db::Point get_gdelta (long grid = 1);
  db::Point get_3delta (long grid = 1);
  db::Point get_2delta (long grid = 1);
  db::Coord get_coord (long grid = 1);
  db::Coord get_ucoord (unsigned long grid = 1);
};

}

#endif

