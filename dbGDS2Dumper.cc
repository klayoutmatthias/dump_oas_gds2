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



#include "dbGDS2Dumper.h"

#include "tlException.h"
#include "tlString.h"

#include <limits>
#include <iostream>

namespace db
{

// ------------------------------------------------------------------
//  Utilities for converting the bytes from the order GDS wants it to have

inline void gds2h (int16_t &s)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__
  //  swap required 
  char x;
  char *d = (char *)&s;
  x = d[0]; d[0] = d[1]; d[1] = x;
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__==__ORDER_BIG_ENDIAN__
  //  .. no action required
#else
  //  generic solution
  s = (int16_t (((unsigned char *)&s) [0]) << 8) | int16_t (((unsigned char *)&s) [1]);
#endif
}

inline void gds2h (int32_t &i)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__
  //  swap required 
  char x;
  char *d = (char *)&i;
  x = d[0]; d[0] = d[3]; d[3] = x;
  x = d[1]; d[1] = d[2]; d[2] = x;
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__==__ORDER_BIG_ENDIAN__
  //  .. no action required
#else
  i = (int32_t (((unsigned char *)&i) [0]) << 24) | (int32_t (((unsigned char *)&i) [1]) << 16) |
      (int32_t (((unsigned char *)&i) [2]) << 8)  |  int32_t (((unsigned char *)&i) [3]);
#endif
}

// ---------------------------------------------------------------
//  GDS2Dumper

GDS2Dumper::GDS2Dumper (tl::InputStreamBase &s)
  : m_stream (s), m_last_emit (0), m_width (8), m_short_mode (false)
{
  m_stream.start_recording ();
}

GDS2Dumper::~GDS2Dumper ()
{
  //  .. nothing yet ..
}

int32_t
GDS2Dumper::get_int32 ()
{
  return int32_t (get_uint32 ());
}

uint32_t
GDS2Dumper::get_uint32 ()
{
  unsigned char *b = (unsigned char *) m_stream.get (4);
  return (uint32_t (b[0]) << 24) | (uint32_t (b[1]) << 16) | (uint32_t (b[2]) << 8) | uint32_t (b[3]);
}

int16_t
GDS2Dumper::get_int16 ()
{
  return int16_t (get_uint16 ());
}

uint16_t
GDS2Dumper::get_uint16 ()
{
  unsigned char *b = (unsigned char *) m_stream.get (2);
  if (! b) {
    error (tl::translate ("Unexpected end of file"));
  }
  return (uint16_t (b[0]) << 8) | uint16_t (b[1]);
}

uint8_t
GDS2Dumper::get_uint8 ()
{
  unsigned char *b = (unsigned char *) m_stream.get (1);
  if (! b) {
    error (tl::translate ("Unexpected end of file"));
  }
  return *b;
}

std::string 
GDS2Dumper::get_str (uint32_t len)
{
  if (! len) {
    return std::string ();
  }

  unsigned char *b = (unsigned char *) m_stream.get (len);
  if (! b) {
    error (tl::translate ("Unexpected end of file"));
  }
  if (!b [len - 1]) {
    return std::string ((const char *) b, 0, size_t (len - 1));
  } else {
    return std::string ((const char *) b, 0, size_t (len));
  }
}

double 
GDS2Dumper::get_double ()
{
  unsigned char *b = (unsigned char *) m_stream.get (8);
  if (! b) {
    error (tl::translate ("Unexpected end of file"));
  }

  uint32_t l0 = ((uint32_t *)b) [0];
  gds2h ((int32_t &) l0);
  l0 &= 0xffffff;
  uint32_t l1 = ((uint32_t *)b) [1];
  gds2h ((int32_t &) l1);

  double x = 4294967296.0 * double (l0) + double (l1);

  if (b[0] & 0x80) {
    x = -x;
  }
  
  int e = int (b[0] & 0x7f) - (64 + 14);
  if (e != 0) {
    x *= pow (16.0, double (e));
  }

  return x;
}

void 
GDS2Dumper::error (const std::string &msg)
{
  throw GDS2DumperException (msg, m_stream.pos (), "UNKNOWN_CELL");
}

void 
GDS2Dumper::warn (const std::string &msg) 
{
  std::cerr << msg 
           << tl::translate (" (position=") << m_stream.pos ()
           << ")"
           << std::endl;
}

void
GDS2Dumper::emit (const std::string &msg)
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

struct RecordDefinition 
{
  uint8_t type;
  uint8_t datatype;
  const char *record_name;
  void (GDS2Dumper::*dump)(const RecordDefinition *, uint16_t);
};

static RecordDefinition s_record_defs[] = {
  { 0x00, 0x02, "HEADER", &GDS2Dumper::header },
  { 0x01, 0x02, "BGNLIB", &GDS2Dumper::timestamp },
  { 0x02, 0x06, "LIBNAME", &GDS2Dumper::generic },
  { 0x03, 0x05, "UNITS", &GDS2Dumper::generic },
  { 0x04, 0x00, "ENDLIB", &GDS2Dumper::generic },
  { 0x05, 0x02, "BGNSTR", &GDS2Dumper::timestamp },
  { 0x06, 0x06, "STRNAME", &GDS2Dumper::generic },
  { 0x07, 0x00, "ENDSTR", &GDS2Dumper::generic },
  { 0x08, 0x00, "BOUNDARY", &GDS2Dumper::generic },
  { 0x09, 0x00, "PATH", &GDS2Dumper::generic },
  { 0x0a, 0x00, "SREF", &GDS2Dumper::generic },
  { 0x0b, 0x00, "AREF", &GDS2Dumper::generic },
  { 0x0c, 0x00, "TEXT", &GDS2Dumper::generic },
  { 0x0d, 0x02, "LAYER", &GDS2Dumper::layer },
  { 0x0e, 0x02, "DATATYPE", &GDS2Dumper::datatype },
  { 0x0f, 0x03, "WIDTH", &GDS2Dumper::generic },
  { 0x10, 0x03, "XY", &GDS2Dumper::xy },
  { 0x11, 0x00, "ENDEL", &GDS2Dumper::generic },
  { 0x12, 0x06, "SNAME", &GDS2Dumper::generic },
  { 0x13, 0x02, "COLROW", &GDS2Dumper::generic },
  { 0x14, 0x00, "TEXTNODE", &GDS2Dumper::generic },
  { 0x15, 0x00, "NODE", &GDS2Dumper::generic },
  { 0x16, 0x02, "TEXTTYPE", &GDS2Dumper::datatype },
  { 0x17, 0x01, "PRESENTATION", &GDS2Dumper::generic },
  { 0x19, 0x06, "STRING", &GDS2Dumper::generic },
  { 0x1a, 0x01, "STRANS", &GDS2Dumper::generic },
  { 0x1b, 0x05, "MAG", &GDS2Dumper::generic },
  { 0x1c, 0x05, "ANGLE", &GDS2Dumper::generic },
  { 0x1f, 0x06, "REFLIBS", &GDS2Dumper::generic },
  { 0x20, 0x06, "FONTS", &GDS2Dumper::generic },
  { 0x21, 0x02, "PATHTYPE", &GDS2Dumper::generic },
  { 0x22, 0x02, "GENERATIONS", &GDS2Dumper::generic },
  { 0x23, 0x06, "ATTRTABLE", &GDS2Dumper::generic },
  { 0x24, 0x06, "STYPTABLE", &GDS2Dumper::generic },
  { 0x25, 0x02, "STRTYPE", &GDS2Dumper::generic },
  { 0x26, 0x01, "ELFLAGS", &GDS2Dumper::generic },
  { 0x27, 0x03, "ELKEY", &GDS2Dumper::generic },
  { 0x2a, 0x02, "NODETYPE", &GDS2Dumper::generic },
  { 0x2b, 0x02, "PROPATTR", &GDS2Dumper::generic },
  { 0x2c, 0x06, "PROPVALUE", &GDS2Dumper::generic },
  { 0x2d, 0x00, "BOX", &GDS2Dumper::generic },
  { 0x2e, 0x02, "BOXTYPE", &GDS2Dumper::datatype },
  { 0x2f, 0x03, "PLEX", &GDS2Dumper::generic },
  { 0x30, 0x03, "BGNEXTN", &GDS2Dumper::generic },
  { 0x31, 0x03, "ENDTEXTN", &GDS2Dumper::generic },
  { 0x32, 0x02, "TAPENUM", &GDS2Dumper::generic },
  { 0x33, 0x02, "TAPECODE", &GDS2Dumper::generic },
  { 0x34, 0x01, "STRCLASS", &GDS2Dumper::generic },
  { 0x35, 0x03, "RESERVED", &GDS2Dumper::generic },
  { 0x36, 0x02, "FORMAT", &GDS2Dumper::generic },
  { 0x37, 0x06, "MASK", &GDS2Dumper::generic },
  { 0x38, 0x00, "ENDMASKS", &GDS2Dumper::generic },
  { 0x39, 0x02, "LIBDIRSIZE", &GDS2Dumper::generic },
  { 0x3a, 0x06, "SRFNAME", &GDS2Dumper::generic }
};

static const std::string s_indent = "  ";

void
GDS2Dumper::header (const RecordDefinition *record_def, uint16_t len)
{
  while (len > 0) {
    emit (s_indent + tl::sprintf ("%d", get_int16 ()));
    len -= 2;
  }
}

void
GDS2Dumper::layer (const RecordDefinition *record_def, uint16_t len)
{
  if (len != 2) {
    error (tl::translate ("There must be one layer number only"));
  }

  uint16_t n = get_uint16 ();
  if (n >= 0x8000) {
    warn (tl::translate ("Layer number treated as unsigned int"));
  }
  emit (s_indent + tl::sprintf ("%d", int (n)));
}

void
GDS2Dumper::datatype (const RecordDefinition *record_def, uint16_t len)
{
  if (len != 2) {
    error (tl::translate ("There must be one datatype number only"));
  }

  uint16_t n = get_uint16 ();
  if (n >= 0x8000) {
    warn (tl::translate ("Datatype number treated as unsigned int"));
  }
  emit (s_indent + tl::sprintf ("%d", int (n)));
}

void
GDS2Dumper::timestamp (const RecordDefinition *record_def, uint16_t len)
{
  if (len != 24) {
    error (tl::sprintf (tl::translate ("There must be two timestamps for %s records"), record_def->record_name));
  }

  for (int i = 0; i < 2; ++i) {
    int year = get_uint16 ();
    int month = get_uint16 ();
    int day = get_uint16 ();
    int hour = get_uint16 ();
    int min = get_uint16 ();
    int sec = get_uint16 ();
    emit (s_indent + tl::sprintf ("%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec));
  }
}

void
GDS2Dumper::xy (const RecordDefinition *record_def, uint16_t len)
{
  while (len > 0) {
    int32_t x = get_int32 ();
    int32_t y = get_int32 ();
    emit (s_indent + tl::sprintf ("%d,%d", x, y));
    len -= 8;
  }
}

void
GDS2Dumper::generic (const RecordDefinition *record_def, uint16_t len)
{
  if (record_def->datatype == 0x00) {
    
    //  no data
  
  } else if (record_def->datatype == 0x01) {

    //  bitmap
    while (len > 0) {

      uint16_t bm = get_uint16 ();

      std::string s = s_indent;
      uint16_t m = bm;
      for (int i = 0; i < 16; ++i) {
        s += ((m & 0x8000) != 0 ? "1" : "0");
        m <<= 1;
      }
      s += tl::sprintf (" (0x%04x)", int (bm));

      emit (s);

      len -= 2;

    }
   
  } else if (record_def->datatype == 0x02) {

    //  int16
    while (len > 0) {
      emit (s_indent + tl::sprintf ("%d", get_int16 ()));
      len -= 2;
    }
   
  } else if (record_def->datatype == 0x03) {

    //  int32
    while (len > 0) {
      emit (s_indent + tl::sprintf ("%d", get_int32 ()));
      len -= 4;
    }
   
  } else if (record_def->datatype == 0x05) {

    //  double
    while (len > 0) {
      emit (s_indent + tl::sprintf ("%.12g", get_double ()));
      len -= 8;
    }
    
  } else if (record_def->datatype == 0x06) {

    std::string s = get_str (len);

    //  substitute special chars and add quotes
    std::string srep = s_indent + "\"";
    for (const char *cp = s.c_str (); *cp; ++cp) {
      if (*cp >= ' ' && (unsigned char) *cp < 0x80 && *cp != '"') {
        srep += *cp;
      } else {
        srep += tl::sprintf ("\\x%02x", (unsigned char) *cp);
      }
    }
    srep += "\"";

    emit (srep);
   
  }
}

void 
GDS2Dumper::dump ()
{
  //  read next record
  while (m_stream.get (2)) {

    m_stream.unget (2);

    uint16_t len = get_uint16 ();
    if (len >= 0x8000) {
      warn (tl::translate ("Record length treated as unsigned int"));
    }
    if (len < 4) {
      error (tl::translate ("Invalid record length less than 4"));
    }
    if ((len % 2) == 1) {
      error (tl::translate ("Invalid odd record length"));
    }

    uint8_t type = get_uint8 ();
    uint8_t datatype = get_uint8 ();

    const RecordDefinition *record_def = 0;
    for (size_t i = 0; i < sizeof (s_record_defs) / sizeof (s_record_defs[0]) && !record_def; ++i) {
      if (s_record_defs[i].type == type) {
        record_def = s_record_defs + i;
      }
    }

    if (! record_def) {
      error (tl::sprintf (tl::translate ("Invalid record type 0x%02x"), type));
    }
    if (record_def->datatype != datatype) {
      error (tl::sprintf (tl::translate ("Invalid type code 0x%02x for record 0x%02x"), datatype, type));
    }

    emit (record_def->record_name);
    (this->*(record_def->dump)) (record_def, len - 4);

  }
}

}

