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

#include "tlException.h"
#include "tlString.h"

#include <limits>
#include <iostream>

namespace db
{

// ---------------------------------------------------------------
//  OASISDumper

OASISDumper::OASISDumper (tl::InputStreamBase &s)
  : m_stream (s), m_last_emit (0), m_width (8), m_short_mode (false)
{
  m_stream.start_recording ();
}

OASISDumper::~OASISDumper ()
{
  //  .. nothing yet ..
}

inline long long 
OASISDumper::get_long_long ()
{
  unsigned long long u = get_ulong_long ();
  if ((u & 1) != 0) {
    return -(long long) (u >> 1);
  } else {
    return (long long) (u >> 1);
  }
}

inline unsigned long long 
OASISDumper::get_ulong_long ()
{
  unsigned long long v = 0;
  unsigned long long vm = 1;
  char c;
  
  do {
    unsigned char *b = (unsigned char *) m_stream.get (1);
    if (! b) {
      error (tl::translate ("Unexpected end-of-file"));
      return 0;
    }
    c = *b;
    if (vm > std::numeric_limits <unsigned long long>::max () / 128 && 
        (unsigned long long) (c & 0x7f) > (std::numeric_limits <unsigned long long>::max () / vm)) {
      warn (tl::translate ("Unsigned long value overflow"));
    }
    v += (unsigned long long) (c & 0x7f) * vm;
    vm <<= 7;
  } while ((c & 0x80) != 0);

  return v;
}

inline long 
OASISDumper::get_long ()
{
  unsigned long u = get_ulong ();
  if ((u & 1) != 0) {
    return -long (u >> 1);
  } else {
    return long (u >> 1);
  }
}

inline unsigned long 
OASISDumper::get_ulong ()
{
  unsigned long v = 0;
  unsigned long vm = 1;
  char c;
  
  do {
    unsigned char *b = (unsigned char *) m_stream.get (1);
    if (! b) {
      error (tl::translate ("Unexpected end-of-file"));
      return 0;
    }
    c = *b;
    if (vm > std::numeric_limits <unsigned long>::max () / 128 && 
        (unsigned long) (c & 0x7f) > (std::numeric_limits <unsigned long>::max () / vm)) {
      warn (tl::translate ("Unsigned long value overflow"));
    }
    v += (unsigned long) (c & 0x7f) * vm;
    vm <<= 7;
  } while ((c & 0x80) != 0);

  return v;
}

inline int 
OASISDumper::get_int ()
{
  unsigned int u = get_uint ();
  if ((u & 1) != 0) {
    return -int (u >> 1);
  } else {
    return int (u >> 1);
  }
}

inline unsigned int 
OASISDumper::get_uint ()
{
  unsigned int v = 0;
  unsigned int vm = 1;
  char c;
  
  do {
    unsigned char *b = (unsigned char *) m_stream.get (1);
    if (! b) {
      error (tl::translate ("Unexpected end-of-file"));
      return 0;
    }
    c = *b;
    if (vm > std::numeric_limits <unsigned int>::max () / 128 && 
        (unsigned int) (c & 0x7f) > (std::numeric_limits <unsigned int>::max () / vm)) {
      warn (tl::translate ("Unsigned integer value overflow"));
    }
    v += (unsigned int) (c & 0x7f) * vm;
    vm <<= 7;
  } while ((c & 0x80) != 0);

  return v;
}

std::string 
OASISDumper::get_str ()
{
  std::string s;
  get_str (s);
  return s;
}

void
OASISDumper::get_str (std::string &s)
{
  size_t l = 0;
  get (l);

  char *b = (char *) m_stream.get (l);
  if (b) {
    s.assign (b, l);
  } else {
    s = std::string ();
  }
}

double
OASISDumper::get_real ()
{
  unsigned int t = get_uint ();

  if (t == 0) {

    return double (get_ulong ()); 

  } else if (t == 1) {

    return -double (get_ulong ()); 

  } else if (t == 2) {

    return 1.0 / double (get_ulong ()); 

  } else if (t == 3) {

    return -1.0 / double (get_ulong ()); 

  } else if (t == 4) {

    double d = double (get_ulong ());
    return d / double (get_ulong ()); 

  } else if (t == 5) {

    double d = double (get_ulong ());
    return -d / double (get_ulong ()); 

  } else if (t == 6) {

    union {
      float f;
      uint32_t i;
    } i2f;

    unsigned char *b = (unsigned char *) m_stream.get (sizeof (i2f.i));
    if (! b) {
      error (tl::translate ("Unexpected end-of-file"));
    }
    i2f.i = 0;
    b += sizeof (i2f.i);
    for (unsigned int i = 0; i < sizeof (i2f.i); ++i) {
      i2f.i = (i2f.i << 8) + uint32_t (*--b);
    }

    return double (i2f.f);

  } else if (t == 7) {

    union {
      double d;
      uint64_t i;
    } i2f;

    unsigned char *b = (unsigned char *) m_stream.get (sizeof (i2f.i));
    if (! b) {
      error (tl::translate ("Unexpected end-of-file"));
    }
    i2f.i = 0;
    b += sizeof (i2f.i);
    for (unsigned int i = 0; i < sizeof (i2f.i); ++i) {
      i2f.i = (i2f.i << 8) + uint64_t (*--b);
    }

    return double (i2f.d);

  } else {
    error (tl::sprintf (tl::translate ("Invalid real type %d"), t));
    return 0.0;
  }
}

db::Coord
OASISDumper::get_ucoord (unsigned long grid)
{
  unsigned long long lx = 0;
  get (lx);
  lx *= grid;
  if (lx > (unsigned long long) (std::numeric_limits <db::Coord>::max ())) {
    warn (tl::translate ("Coordinate value overflow"));
  }
  return db::Coord (lx);
}

db::Coord
OASISDumper::get_coord (long grid)
{
  long long lx = 0;
  get (lx);
  lx *= grid;
  if (lx < (long long) (std::numeric_limits <db::Coord>::min ()) ||
      lx > (long long) (std::numeric_limits <db::Coord>::max ())) {
    warn (tl::translate ("Coordinate value overflow"));
  }
  return db::Coord (lx);
}

db::Point
OASISDumper::get_2delta (long grid)
{
  unsigned long long l1 = 0;
  get (l1);

  long long lx = l1 >> 2;
  lx *= grid;
  if (lx > (long long) (std::numeric_limits <db::Coord>::max ())) {
    warn (tl::translate ("Coordinate value overflow"));
  }
  db::Coord x = lx;

  switch (l1 & 3) {
  case 0:
    return db::Point (x, 0);
  case 1:
    return db::Point (0, x);
  case 2:
    return db::Point (-x, 0);
  case 3:
  default:
    return db::Point (0, -x);
  }
}

db::Point
OASISDumper::get_3delta (long grid)
{
  unsigned long long l1 = 0;
  get (l1);

  long long lx = l1 >> 3;
  lx *= grid;
  if (lx > (long long) (std::numeric_limits <db::Coord>::max ())) {
    warn (tl::translate ("Coordinate value overflow"));
  }
  db::Coord x = lx;

  switch (l1 & 7) {
  case 0:
    return db::Point (x, 0);
  case 1:
    return db::Point (0, x);
  case 2:
    return db::Point (-x, 0);
  case 3:
    return db::Point (0, -x);
  case 4:
    return db::Point (x, x);
  case 5:
    return db::Point (-x, x);
  case 6:
    return db::Point (-x, -x);
  case 7:
  default:
    return db::Point (x, -x);
  }
}

db::Point
OASISDumper::get_gdelta (long grid)
{
  unsigned long long l1 = 0;
  get (l1);

  if ((l1 & 1) != 0) {

    long long lx = ((l1 & 2) == 0 ? (long long) (l1 >> 2) : -(long long) (l1 >> 2));
    lx *= grid;
    if (lx < (long long) (std::numeric_limits <db::Coord>::min ()) ||
        lx > (long long) (std::numeric_limits <db::Coord>::max ())) {
      warn (tl::translate ("Coordinate value overflow"));
    }

    long long ly;
    get (ly);
    ly *= grid;
    if (ly < (long long) (std::numeric_limits <db::Coord>::min ()) ||
        ly > (long long) (std::numeric_limits <db::Coord>::max ())) {
      warn (tl::translate ("Coordinate value overflow"));
    }
    
    return db::Point (db::Coord (lx), db::Coord (ly));

  } else {

    long long lx = l1 >> 4;
    lx *= grid;
    if (lx > (long long) (std::numeric_limits <db::Coord>::max ())) {
      warn (tl::translate ("Coordinate value overflow"));
    }
    db::Coord x = lx;

    switch ((l1 >> 1) & 7) {
    case 0:
      return db::Point (x, 0);
    case 1:
      return db::Point (0, x);
    case 2:
      return db::Point (-x, 0);
    case 3:
      return db::Point (0, -x);
    case 4:
      return db::Point (x, x);
    case 5:
      return db::Point (-x, x);
    case 6:
      return db::Point (-x, -x);
    case 7:
    default:
      return db::Point (x, -x);
    }

  }
}

void 
OASISDumper::error (const std::string &msg)
{
  throw OASISDumperException (msg, m_stream.pos (), "UNKNOWN_CELL");
}

void 
OASISDumper::warn (const std::string &msg) 
{
  std::cerr << msg 
           << tl::translate (" (position=") << m_stream.pos ()
           << ")"
           << std::endl;
}

void
OASISDumper::emit (const std::string &msg)
{
  size_t last_pos = m_last_emit;
  m_last_emit = m_stream.pos ();

  const char *r = m_stream.recorded ();
  std::cout << tl::sprintf ("%09d   ", last_pos);
  for (size_t i = 0; i < m_width; ++i) {
    if (last_pos + i < m_last_emit) {
      char c = *r++;
      std::cout << tl::sprintf ("%02x ", int ((unsigned char) c));
    } else {
      std::cout << "   ";
    }
  }

  m_stream.reset_recording ();

  std::cout << " " << msg << std::endl;

  last_pos += m_width;
  while (last_pos < m_last_emit) {

    std::cout << tl::sprintf ("%09d + ", last_pos);
    if (m_short_mode) {

      std::cout << "..." << std::endl;
      break;

    } else {

      for (size_t i = 0; i < m_width; ++i) {
        if (last_pos + i < m_last_emit) {
          char c = *r++;
          std::cout << tl::sprintf ("%02x ", int ((unsigned char) c));
        } else {
          break;
        }
      }

      last_pos += m_width;
      std::cout << std::endl;

    }
  }
}

static const char magic_bytes[] = { "%SEMI-OASIS\015\012" };

void 
OASISDumper::dump ()
{
  unsigned char r;
  char *mb;

  //  read magic bytes
  mb = (char *) m_stream.get (sizeof (magic_bytes) - 1);
  if (! mb) {
    error (tl::translate ("File too short"));
    return;
  }
  if (strncmp (mb, magic_bytes, sizeof (magic_bytes) - 1) != 0) {
    error (tl::translate ("Format error (missing magic bytes)"));
  }

  emit ("magic bytes");

  //  read first record
  r = get_byte ();
  if (r != 1) {
    error (tl::translate ("Format error (START record expected)"));
  }

  emit ("START");

  std::string v = get_str ();
  if (v != "1.0") {
    error (tl::sprintf (tl::translate ("Format error (only version 1.0 is supported, file has version %s)"), v));
  }

  emit ("version (\"" + v + "\")");

  double res = get_real ();
  if (res < 1e-6) {
    error (tl::sprintf (tl::translate ("Invalid resolution of %g"), res));
  }

  emit ("resolution (" + tl::to_string (res) + ")");

  //  read over table offsets if required
  bool table_offsets_at_end = get_uint ();
  emit ("table flag (" + std::string (table_offsets_at_end ? "at end" : "here") + ")");

  if (! table_offsets_at_end) {
    for (unsigned int i = 0; i < 12; ++i) {
      unsigned long t = get_ulong ();
      emit ("tables entry (" + tl::to_string (t) + ")");
    }
  }

  //  read next record
  while (true) {

    r = get_byte ();

    if (r == 0 /*PAD*/) {

      emit ("PAD");

    } else if (r == 2 /*END*/) {

      emit ("END");

      if (table_offsets_at_end) {
        for (unsigned int i = 0; i < 12; ++i) {
          unsigned long t = get_ulong ();
          emit ("tables entry (" + tl::to_string (t) + ")");
        }
      }

      std::string padding = get_str ();
      emit ("padding string (\"" + padding + "\")");

      unsigned int vs = get_uint ();
      emit ("validation scheme (" + tl::to_string (vs) + ")");

      if (vs == 1 || vs == 2) {
        for (unsigned int i = 0; i < 4; ++i) {
          get_byte ();
        }
        emit ("validation signature");
      }

      break;

    } else if (r == 3 || r == 4 /*CELLNAME*/) {

      //  read a cell name
      std::string name = get_str ();

      //  and the associated id
      if (r == 3) {
        emit ("CELLNAME (\"" + name + "\")");
      } else {
        unsigned long id = 0;
        get (id);
        emit ("CELLNAME (\"" + name + "\", id=" + tl::to_string (id) + ")");
      }

    } else if (r == 5 || r == 6 /*TEXTSTRING*/) {

      //  read a text string
      std::string name = get_str ();

      //  and the associated id
      if (r == 5) {
        emit ("TEXTSTRING (\"" + name + "\")");
      } else {
        unsigned long id = 0;
        get (id);
        emit ("TEXTSTRING (\"" + name + "\", id=" + tl::to_string (id) + ")");
      }

    } else if (r == 7 || r == 8 /*PROPNAME*/) {

      //  read a property name
      std::string name = get_str ();

      //  and the associated id
      if (r == 7) {
        emit ("PROPNAME (\"" + name + "\")");
      } else {
        unsigned long id = 0;
        get (id);
        emit ("PROPNAME (\"" + name + "\", id=" + tl::to_string (id) + ")");
      }

    } else if (r == 9 || r == 10 /*PROPSTRING*/) {

      //  read a property string
      std::string name = get_str ();

      //  and the associated id
      if (r == 9) {
        emit ("PROPSTRING (\"" + name + "\")");
      } else {
        unsigned long id = 0;
        get (id);
        emit ("PROPSTRING (\"" + name + "\", id=" + tl::to_string (id) + ")");
      }

    } else if (r == 11 || r == 12 /*LAYERNAME*/) {

      //  read a layer name 
      std::string name = get_str ();

      unsigned int dt1 = 0, dt2 = std::numeric_limits<unsigned int>::max () - 1;
      unsigned int l1 = 0, l2 = std::numeric_limits<unsigned int>::max () - 1;
      unsigned int it;

      it = get_uint ();
      if (it == 0) {
        //  keep limits
      } else if (it == 1) {
        l2 = get_uint ();
      } else if (it == 2) {
        l1 = get_uint ();
      } else if (it == 3) {
        l1 = get_uint ();
        l2 = l1;
      } else if (it == 4) {
        l1 = get_uint ();
        l2 = get_uint ();
      } else {
        error (tl::translate ("Invalid LAYERNAME interval mode (layer)"));
      }

      it = get_uint ();
      if (it == 0) {
        //  keep limits
      } else if (it == 1) {
        dt2 = get_uint ();
      } else if (it == 2) {
        dt1 = get_uint ();
      } else if (it == 3) {
        dt1 = get_uint ();
        dt2 = dt1;
      } else if (it == 4) {
        dt1 = get_uint ();
        dt2 = get_uint ();
      } else {
        error (tl::translate ("Invalid LAYERNAME interval mode (datatype)"));
      }

      //  and the associated id
      emit ("LAYERNAME (\"" + name + "\", layers=" + tl::to_string (l1) + ".." + tl::to_string (l2) + ", datatypes=" + tl::to_string (dt1) + ".." + tl::to_string (dt2) + ")");
      
    } else if (r == 28 || r == 29 /*PROPERTY*/) {

      if (r == 28) {
        read_properties ();
      } else {
        emit ("PROPERTY (repeat)");
      }

    } else if (r == 30 || r == 31 /*XNAME*/) {

      emit ("XNAME");

      //  read a XNAME: it is simply ignored
      get_ulong ();
      get_str ();
      if (r == 31) {
        get_ulong ();
      }

      emit ("data"); //  TODO: refine

    } else if (r == 13 || r == 14 /*CELL*/) {

      db::cell_index_type cell_index = 0;

      //  read a cell
      if (r == 13) {

        unsigned long id = 0;
        get (id);

        emit ("CELL (" + tl::to_string (id) + ")");

      } else {

        std::string name = get_str ();
        emit ("CELL (\"" + name + "\")");

      }

      do_read_cell ();

    } else if (r == 34 /*CBLOCK*/) {

      emit ("CBLOCK (data will be expanded)");

      unsigned int type = get_uint ();
      if (type != 0) {
        error (tl::sprintf (tl::translate ("Invalid CBLOCK compression type %d"), type));
      }

      size_t uncomp_bytes = 0, comp_bytes = 0;
      get (uncomp_bytes);
      get (comp_bytes);
      emit ("cblock-info (type=" + tl::to_string (type) + ", uncomp-bytes=" + tl::to_string (uncomp_bytes) + ", comp_bytes=" + tl::to_string (comp_bytes) + ")");

      //  put the stream into deflating mode
      m_stream.inflate ();

    } else {
      error (tl::sprintf (tl::translate ("Invalid record type on global level %d"), int (r)));
    }

  }

  emit ("tail");

  //  check if there are no more bytes
  mb = (char *) m_stream.get (254);
  if (mb) {
    error (tl::translate ("Format error (too many bytes after END record)"));
  }
}

void
OASISDumper::read_element_properties ()
{
  while (true) {

    unsigned char m = get_byte ();

    if (m == 28) {
      read_properties ();
    } else if (m != 29) {
      m_stream.unget (1);
      break;
    } else {
      emit ("PROPERTY (repeat)");
    }

  } 
}

void 
OASISDumper::read_properties ()
{
  unsigned char m = get_byte ();

  if (m & 0x04) {
    if (m & 0x02) {
      unsigned long id;
      get (id);
      emit ("PROPERTY (id=" + tl::to_string (id) + ")");
    } else {
      std::string name;
      get_str (name);
      emit ("PROPERTY (name=" + name + ")");
    }
  } else {
    emit ("PROPERTY (same id)");
  }

  if (! (m & 0x08)) {

    unsigned long n = ((unsigned long) (m >> 4)) & 0x0f;
    if (n == 15) {
      get (n);
    }

    int index = 0;
    while (n > 0) {

      unsigned char t = get_byte ();
      if (t < 8) {

        m_stream.unget (1);
        double v = get_real ();
        emit (std::string ("value[") + tl::to_string (index) + "]=" + tl::to_string(v) + " (type " + tl::to_string (int (t)) + ")");

      } else if (t == 8) {

        unsigned long l;
        get (l);
        emit (std::string ("value[") + tl::to_string (index) + "]=" + tl::to_string(l) + " (type " + tl::to_string (int (t)) + ")");

      } else if (t == 9) {

        long l;
        get (l);
        emit (std::string ("value[") + tl::to_string (index) + "]=" + tl::to_string(l) + " (type " + tl::to_string (int (t)) + ")");

      } else if (t == 10 || t == 11 || t == 12) {

        std::string name;
        get_str (name);
        emit (std::string ("value[") + tl::to_string (index) + "]=" + name + " (type " + tl::to_string (int (t)) + ")");

      } else if (t == 13 || t == 14 || t == 15) {

        unsigned long id;
        get (id);
        emit (std::string ("value[") + tl::to_string (index) + "]=" + tl::to_string(id) + " (propstring-ref, type " + tl::to_string (int (t)) + ")");

      } else {
        error (tl::sprintf (tl::translate ("Invalid property value type %d"), int (t)));
      }

      --n;
      ++index;

    }

  }
}

      
void 
OASISDumper::read_pointlist ()
{
  unsigned int type = get_uint ();

  emit ("pointlist (type=" + tl::to_string (type) + ")");
  
  unsigned long n = 0;
  get (n);
  if (n == 0) {
    error (tl::translate ("Invalid point list: length is zero").c_str ());
  }

  if (type == 0 || type == 1) {

    bool h = (type == 0);

    db::Point pos;
    for (unsigned long i = 0; i < n; ++i) {
      db::Coord d = get_coord ();
      if (h) {
        pos += db::Point (d, 0);
      } else {
        pos += db::Point (0, d);
      }
      emit ("  xy=" + pos.to_string ());
      h = ! h;
    }

  } else if (type == 2) {

    db::Point pos;
    for (unsigned long i = 0; i < n; ++i) {
      pos += get_2delta ();
      emit ("  xy=" + pos.to_string ());
    }

  } else if (type == 3) {

    db::Point pos;
    for (unsigned long i = 0; i < n; ++i) {
      pos += get_3delta ();
      emit ("  xy=" + pos.to_string ());
    }

  } else if (type == 4) {

    db::Point pos;
    for (unsigned long i = 0; i < n; ++i) {
      pos += get_gdelta ();
      emit ("  xy=" + pos.to_string ());
    }

  } else if (type == 5) {

    db::Point pos;
    db::Point delta;
    for (unsigned long i = 0; i < n; ++i) {
      delta += get_gdelta ();
      pos += delta;
      emit ("  xy=" + pos.to_string ());
    }

  } else {
    error (tl::sprintf (tl::translate ("Invalid point list type %d"), type));
  }
}

void
OASISDumper::read_repetition ()
{
  unsigned int type = get_uint ();
  emit ("repetition (type=" + tl::to_string (type) + ")");
  
  if (type == 0) {
    
    //  reuse modal variable

  } else if (type == 1) {

    unsigned long nx = 0, ny = 0;
    get (nx); 
    emit ("  nx=" + tl::to_string (nx));
    get (ny);
    emit ("  ny=" + tl::to_string (ny));

    db::Coord dx = get_ucoord ();
    emit ("  dx=" + tl::to_string (dx));
    db::Coord dy = get_ucoord ();
    emit ("  dy=" + tl::to_string (dy));

  } else if (type == 2) {

    unsigned long nx = 0;
    get (nx); 
    emit ("  nx=" + tl::to_string (nx));

    db::Coord dx = get_ucoord ();
    emit ("  dx=" + tl::to_string (dx));

  } else if (type == 3) {

    unsigned long ny = 0;
    get (ny);
    emit ("  ny=" + tl::to_string (ny));

    db::Coord dy = get_ucoord ();
    emit ("  dy=" + tl::to_string (dy));

  } else if (type == 4 || type == 5) {
    
    unsigned long n = 0;
    get (n);
    emit ("  n=" + tl::to_string (n));

    unsigned long lgrid = 1;
    if (type == 5) {
      get (lgrid);
      emit ("  grid=" + tl::to_string (lgrid));
    }

    db::Coord x = 0;
    for (unsigned long i = 0; i <= n; ++i) {
      x += get_ucoord (lgrid);
      emit ("  x=" + tl::to_string (x));
    }

  } else if (type == 6 || type == 7) {
    
    unsigned long n = 0;
    get (n);
    emit ("  n=" + tl::to_string (n));

    unsigned long lgrid = 1;
    if (type == 7) {
      get (lgrid);
      emit ("  grid=" + tl::to_string (lgrid));
    }

    db::Coord y = 0;
    for (unsigned long i = 0; i <= n; ++i) {
      y += get_ucoord (lgrid);
      emit ("  y=" + tl::to_string (y));
    }

  } else if (type == 8) {

    unsigned long n = 0, m = 0;

    get (n); 
    emit ("  n=" + tl::to_string (n));
    get (m);
    emit ("  m=" + tl::to_string (m));
    db::Point dn = get_gdelta (); 
    emit ("  dn=" + tl::to_string (dn));
    db::Point dm = get_gdelta (); 
    emit ("  dm=" + tl::to_string (dm));

  } else if (type == 9) {

    unsigned long n = 0;
    get (n); 
    emit ("  n=" + tl::to_string (n));
    db::Point dn = get_gdelta (); 
    emit ("  dn=" + tl::to_string (dn));

  } else if (type == 10) {

    unsigned long n = 0;
    get (n);
    emit ("  n=" + tl::to_string (n));

    db::Point p;
    for (unsigned long i = 0; i <= n; ++i) {
      p += get_gdelta ();
      emit ("  xy=" + p.to_string ());
    }

  } else if (type == 11) {

    unsigned long n = 0;
    get (n);
    emit ("  n=" + tl::to_string (n));

    unsigned long grid = 0;
    get (grid);
    emit ("  grid=" + tl::to_string (grid));

    db::Point p;
    for (unsigned long i = 0; i <= n; ++i) {
      p += get_gdelta (grid);
      emit ("  xy=" + p.to_string ());
    }

  } else {
    error (tl::sprintf (tl::translate ("Invalid repetition type %d"), type));
  }
}

void 
OASISDumper::do_read_placement (unsigned int r)
{
  unsigned char m = get_byte ();
  emit ("PLACEMENT");

  //  locate cell
  if (m & 0x80) {

    if (m & 0x40) {

      //  cell by id
      unsigned long id;
      get (id);

      emit ("id=" + tl::to_string (id));

    } else {

      //  cell by name
      std::string name;
      get_str (name);
      emit ("name=" + name);

    }

  } 

  if (r == 18) {

    if (m & 0x04) {
      double mag = get_real ();
      emit ("mag=" + tl::to_string (mag));
    }

    if (m & 0x02) {
      double angle_deg = get_real ();
      emit ("angle=" + tl::to_string (angle_deg));
    }

  }
      
  // bool mirror = (m & 0x01) != 0;

  if (m & 0x20) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x10) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x8) {
    read_repetition ();
  } 

  read_element_properties ();
}

void 
OASISDumper::do_read_text ()
{
  unsigned char m = get_byte ();

  emit ("TEXT");

  if (m & 0x40) {
    if (m & 0x20) {
      unsigned long id;
      get (id);
      emit ("id=" + tl::to_string (id));
    } else {
      std::string t = get_str ();
      emit ("Text=" + t);
    }
  } 

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("texttype=" + tl::to_string (dt));
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void 
OASISDumper::do_read_rectangle ()
{
  unsigned char m = get_byte ();

  emit ("RECTANGLE");

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("datatype=" + tl::to_string (dt));
  }

  if (m & 0x40) {
    db::Coord w = get_ucoord ();
    emit ("width=" + tl::to_string (w));
  } 
  if (m & 0x80) {
    //  ..
  } else {
    if (m & 0x20) {
      db::Coord h = get_ucoord ();
      emit ("height=" + tl::to_string (h));
    } 
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void  
OASISDumper::do_read_polygon ()
{
  unsigned char m = get_byte ();
  emit ("POLYGON");

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("datatype=" + tl::to_string (dt));
  }

  if (m & 0x20) {
    read_pointlist ();
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void  
OASISDumper::do_read_path ()
{
  unsigned char m = get_byte ();
  emit ("PATH");

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("datatype=" + tl::to_string (dt));
  }

  if (m & 0x40) {
    db::Coord w = get_ucoord ();
    emit ("half_width=" + tl::to_string (w));
  }

  if (m & 0x80) {

    unsigned int e = get_uint ();
    emit ("extensions (type=" + tl::to_string (e) + ")");
    db::Coord ext1 = 0, ext2 = 0;
    if ((e & 0x0c) == 0x0c) {
      db::Coord e1 = get_coord ();
      emit ("  e1=" + tl::to_string (e1));
    }
    if ((e & 0x03) == 0x03) {
      db::Coord e2 = get_coord ();
      emit ("  e2=" + tl::to_string (e2));
    }

  }

  if (m & 0x20) {
    read_pointlist ();
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void  
OASISDumper::do_read_trapezoid (unsigned char r)
{
  unsigned char m = get_byte ();
  emit ("TRAPEZOID");

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("datatype=" + tl::to_string (dt));
  }

  if (m & 0x40) {
    db::Coord w = get_ucoord ();
    emit ("w=" + tl::to_string (w));
  }

  if (m & 0x20) {
    db::Coord h = get_ucoord ();
    emit ("h=" + tl::to_string (h));
  }

  if (r == 23 || r == 24) {
    db::Coord a = get_coord ();
    emit ("a=" + tl::to_string (a));
  }
  if (r == 23 || r == 25) {
    db::Coord b = get_coord ();
    emit ("b=" + tl::to_string (b));
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void  
OASISDumper::do_read_ctrapezoid ()
{
  unsigned char m = get_byte ();
  emit ("CTRAPEZOID");

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("datatype=" + tl::to_string (dt));
  }

  if (m & 0x80) {
    unsigned int type = get_uint ();
    emit ("type=(" + tl::to_string (type));
  }

  if (m & 0x40) {
    db::Coord w = get_ucoord ();
    emit ("w=" + tl::to_string (w));
  }

  if (m & 0x20) {
    db::Coord h = get_ucoord ();
    emit ("h=" + tl::to_string (h));
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void  
OASISDumper::do_read_circle ()
{
  unsigned char m = get_byte ();
  emit ("CIRCLE");

  if (m & 0x1) {
    unsigned int l = get_uint ();
    emit ("layer=" + tl::to_string (l));
  }

  if (m & 0x2) {
    unsigned int dt = get_uint ();
    emit ("datatype=" + tl::to_string (dt));
  }

  if (m & 0x20) {
    db::Coord r = get_ucoord ();
    emit ("r=" + tl::to_string (r));
  }

  if (m & 0x10) {
    db::Coord x;
    get (x);
    emit ("x=" + tl::to_string (x));
  }

  if (m & 0x8) {
    db::Coord y;
    get (y);
    emit ("y=" + tl::to_string (y));
  }

  if (m & 0x4) {
    read_repetition ();
  }

  read_element_properties ();
}

void 
OASISDumper::do_read_cell ()
{
  bool xy_absolute = true;

  //  read next record
  while (true) {

    unsigned char r = get_byte ();

    if (r == 0 /*PAD*/) {

      //  simply skip.

    } else if (r == 15 /*XYABSOLUTE*/) {

      //  switch to absolute mode
      xy_absolute = true;
      emit ("XYABSOLUTE");

    } else if (r == 16 /*XYRELATIVE*/) {

      //  switch to relative mode
      xy_absolute = false;
      emit ("XYRELATIVE");

    } else if (r == 17 || r == 18 /*PLACEMENT*/) {

      do_read_placement (r);

    } else if (r == 19 /*TEXT*/) {

      do_read_text ();

    } else if (r == 20 /*RECTANGLE*/) {

      do_read_rectangle ();

    } else if (r == 21 /*POLYGON*/) {

      do_read_polygon ();

    } else if (r == 22 /*PATH*/) {

      do_read_path ();

    } else if (r == 23 || r == 24 || r == 25 /*TRAPEZOID*/) {

      do_read_trapezoid (r);

    } else if (r == 26 /*CTRAPEZOID*/) {

      do_read_ctrapezoid ();

    } else if (r == 27 /*CIRCLE*/) {

      do_read_circle ();

    } else if (r == 28 || r == 29 /*PROPERTY*/) {

      if (r == 28) {
        read_properties ();
      } else {
        emit ("PROPERTY (repeat)");
      }

    } else if (r == 32 /*XELEMENT*/) {

      //  read over
      get_ulong ();
      get_str ();
      emit ("XELEMENT");

    } else if (r == 33 /*XGEOMETRY*/) {

      //  read over.

      unsigned char m = get_byte ();
      emit ("XGEOMTERY");

      unsigned int a = get_uint ();
      emit ("attribute=" + tl::to_string (a));

      if (m & 0x1) {
        unsigned int l = get_uint ();
        emit ("layer=" + tl::to_string (l));
      }

      if (m & 0x2) {
        unsigned int dt = get_uint ();
        emit ("datatype=" + tl::to_string (dt));
      }

      //  data payload:
      get_str ();
      emit ("data");

      if (m & 0x10) {
        db::Coord x;
        get (x);
        emit ("x=" + tl::to_string (x));
      }

      if (m & 0x8) {
        db::Coord y;
        get (y);
        emit ("y=" + tl::to_string (y));
      }

      if (m & 0x4) {
        read_repetition ();
      }

    } else if (r == 34 /*CBLOCK*/) {

      emit ("CBLOCK (data will be expanded)");

      unsigned int type = get_uint ();
      if (type != 0) {
        error (tl::sprintf (tl::translate ("Invalid CBLOCK compression type %d"), type));
      }

      size_t uncomp_bytes = 0, comp_bytes = 0;
      get (uncomp_bytes);
      get (comp_bytes);
      emit ("cblock-info (type=" + tl::to_string (type) + ", uncomp-bytes=" + tl::to_string (uncomp_bytes) + ", comp_bytes=" + tl::to_string (comp_bytes) + ")");

      //  put the stream into deflating mode
      m_stream.inflate ();

    } else {
      //  put the byte back into the stream
      m_stream.unget (1);
      break;
    }

  }
}

}

