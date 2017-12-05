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


#include "tlVariant.h"
#include "tlString.h"

#include <string.h>
#include <limits>

namespace tl
{

// --------------------------------------------------------------------
//  Complex type registration

static std::map <std::string, const VariantUserClassBase *> *s_complex_type_by_name = 0;

void register_variant_complex_type (const VariantUserClassBase *cls, unsigned int type, const char *name)
{
  if (! s_complex_type_by_name) {
    s_complex_type_by_name = new std::map <std::string, const VariantUserClassBase *> ();
  }

  s_complex_type_by_name->insert (std::make_pair (std::string (name), cls));
}

const VariantUserClassBase *find_cls_by_name (const char *name) 
{
  if (! s_complex_type_by_name) {
    return 0;
  }

  std::map <std::string, const VariantUserClassBase *>::const_iterator s = s_complex_type_by_name->find (name);
  if (s == s_complex_type_by_name->end ()) {
    return 0;
  }

  return s->second;
}

// --------------------------------------------------------------------
//  Implementation of tl::Variant

Variant::Variant () 
  : m_type (t_nil), m_string (0)
{
  // .. nothing yet ..
}

Variant::Variant (const std::string &s) 
  : m_type (t_stdstring), m_string (0)
{
  m_var.m_stdstring = new std::string (s);
}

Variant::Variant (const char *s) 
  : m_type (t_string)
{
  m_string = new char [strlen (s) + 1];
  strcpy (m_string, s);
}

Variant::Variant (double d)
  : m_type (t_double), m_string (0)
{
  m_var.m_double = d;
}

Variant::Variant (float d)
  : m_type (t_float), m_string (0)
{
  m_var.m_float = d;
}

Variant::Variant (bool b)
  : m_type (t_bool), m_string (0)
{
  m_var.m_bool = b;
}

Variant::Variant (char c)
  : m_type (t_char), m_string (0)
{
  m_var.m_char = c;
}

Variant::Variant (signed char c)
  : m_type (t_schar), m_string (0)
{
  m_var.m_schar = c;
}

Variant::Variant (unsigned char c)
  : m_type (t_uchar), m_string (0)
{
  m_var.m_uchar = c;
}

Variant::Variant (short s)
  : m_type (t_short), m_string (0)
{
  m_var.m_short = s;
}

Variant::Variant (unsigned short s)
  : m_type (t_ushort), m_string (0)
{
  m_var.m_ushort = s;
}

Variant::Variant (int l)
  : m_type (t_int), m_string (0)
{
  m_var.m_int = l;
}

Variant::Variant (unsigned int l)
  : m_type (t_uint), m_string (0)
{
  m_var.m_uint = l;
}

Variant::Variant (long long l)
  : m_type (t_longlong), m_string (0)
{
  m_var.m_longlong = l;
}

Variant::Variant (unsigned long long l)
  : m_type (t_ulonglong), m_string (0)
{
  m_var.m_ulonglong = l;
}

Variant::Variant (long l)
  : m_type (t_long), m_string (0)
{
  m_var.m_long = l;
}

Variant::Variant (unsigned long l)
  : m_type (t_ulong), m_string (0)
{
  m_var.m_ulong = l;
}

Variant::Variant (size_t l, bool /*dummy*/)
  : m_type (t_id), m_string (0)
{
  m_var.m_id = l;
}

Variant::Variant (const Variant &v)
  : m_type (t_nil), m_string (0)
{
  operator= (v);
}

Variant::~Variant ()
{
  reset ();
}

void
Variant::reset ()
{
  if (m_string) {
    delete [] m_string;
  }
  m_string = 0;
  if (m_type == t_list) {
    delete m_var.m_list;
  } else if (m_type == t_array) {
    delete m_var.m_array;
  } else if (m_type == t_stdstring) {
    delete m_var.m_stdstring;
  } else if (is_user () || is_complex ()) {
    if (m_var.mp_user.object) {
      m_var.mp_user.cls->destroy (m_var.mp_user.object);
    }
  }
  m_type = t_nil;
}

Variant &
Variant::operator= (const char *s)
{
  if (m_type == t_string && s == m_string) {
    //  we are assigning to ourselves
  } else {
    char *snew = new char [strlen (s) + 1];
    strcpy (snew, s);
    reset ();
    m_type = t_string;
    m_string = snew;
  }
  return *this;
}

Variant &
Variant::operator= (const std::string &s)
{
  if (m_type == t_stdstring && &s == m_var.m_stdstring) {
    //  we are assigning to ourselves
  } else {
    std::string *snew = new std::string (s);
    reset ();
    m_type = t_stdstring;
    m_var.m_stdstring = snew;
  }
  return *this;
}

Variant &
Variant::operator= (double d)
{
  reset ();
  m_type = t_double;
  m_var.m_double = d;
  return *this;
}

Variant &
Variant::operator= (float d)
{
  reset ();
  m_type = t_float;
  m_var.m_float = d;
  return *this;
}

Variant &
Variant::operator= (bool b)
{
  reset ();
  m_type = t_bool;
  m_var.m_bool = b;
  return *this;
}

Variant &
Variant::operator= (signed char c)
{
  reset ();
  m_type = t_schar;
  m_var.m_schar = c;
  return *this;
}

Variant &
Variant::operator= (unsigned char c)
{
  reset ();
  m_type = t_uchar;
  m_var.m_uchar = c;
  return *this;
}

Variant &
Variant::operator= (char c)
{
  reset ();
  m_type = t_char;
  m_var.m_char = c;
  return *this;
}

Variant &
Variant::operator= (unsigned short s)
{
  reset ();
  m_type = t_ushort;
  m_var.m_ushort = s;
  return *this;
}

Variant &
Variant::operator= (short s)
{
  reset ();
  m_type = t_short;
  m_var.m_short = s;
  return *this;
}

Variant &
Variant::operator= (unsigned int l)
{
  reset ();
  m_type = t_uint;
  m_var.m_uint = l;
  return *this;
}

Variant &
Variant::operator= (int l)
{
  reset ();
  m_type = t_int;
  m_var.m_int = l;
  return *this;
}

Variant &
Variant::operator= (unsigned long l)
{
  reset ();
  m_type = t_ulong;
  m_var.m_ulong = l;
  return *this;
}

Variant &
Variant::operator= (long l)
{
  reset ();
  m_type = t_long;
  m_var.m_long = l;
  return *this;
}

Variant &
Variant::operator= (unsigned long long l)
{
  reset ();
  m_type = t_ulonglong;
  m_var.m_ulonglong = l;
  return *this;
}

Variant &
Variant::operator= (long long l)
{
  reset ();
  m_type = t_longlong;
  m_var.m_longlong = l;
  return *this;
}

Variant &
Variant::operator= (const Variant &v)
{
  if (this != &v) {

    reset ();

    m_type = v.m_type;
    if (m_type == t_double) {
      m_var.m_double = v.m_var.m_double;
    } else if (m_type == t_float) {
      m_var.m_float = v.m_var.m_float;
    } else if (m_type == t_bool) {
      m_var.m_bool = v.m_var.m_bool;
    } else if (m_type == t_uchar) {
      m_var.m_uchar = v.m_var.m_uchar;
    } else if (m_type == t_schar) {
      m_var.m_schar = v.m_var.m_schar;
    } else if (m_type == t_char) {
      m_var.m_char = v.m_var.m_char;
    } else if (m_type == t_ushort) {
      m_var.m_ushort = v.m_var.m_ushort;
    } else if (m_type == t_short) {
      m_var.m_short = v.m_var.m_short;
    } else if (m_type == t_uint) {
      m_var.m_uint = v.m_var.m_uint;
    } else if (m_type == t_int) {
      m_var.m_int = v.m_var.m_int;
    } else if (m_type == t_ulong) {
      m_var.m_ulong = v.m_var.m_ulong;
    } else if (m_type == t_long) {
      m_var.m_long = v.m_var.m_long;
    } else if (m_type == t_longlong) {
      m_var.m_longlong = v.m_var.m_longlong;
    } else if (m_type == t_ulonglong) {
      m_var.m_ulonglong = v.m_var.m_ulonglong;
    } else if (m_type == t_id) {
      m_var.m_id = v.m_var.m_id;
    } else if (m_type == t_stdstring) {
      m_var.m_stdstring = new std::string (*v.m_var.m_stdstring);
    } else if (m_type == t_string) {
      m_string = new char [strlen (v.m_string) + 1];
      strcpy (m_string, v.m_string);
    } else if (m_type == t_list) {
      m_var.m_list = new std::vector<tl::Variant> (*v.m_var.m_list);
    } else if (m_type == t_array) {
      m_var.m_array = new std::map<tl::Variant, tl::Variant> (*v.m_var.m_array);
    } else if (is_user () || is_complex ()) {
      m_var.mp_user.cls = v.m_var.mp_user.cls;
      if (v.m_var.mp_user.object) {
        m_var.mp_user.object = v.m_var.mp_user.cls->clone (v.m_var.mp_user.object);
      } else {
        m_var.mp_user.object = 0;
      }
    }

  } 
  return *this;
}

inline Variant::type 
normalized_type (Variant::type type)
{
  switch (type) {
  case Variant::t_float:
  case Variant::t_double:
    return Variant::t_double;
  case Variant::t_char:
  case Variant::t_schar:
  case Variant::t_short:
  case Variant::t_int:
  case Variant::t_long:
    return Variant::t_long;
  case Variant::t_uchar:
  case Variant::t_ushort:
  case Variant::t_uint:
  case Variant::t_ulong:
    return Variant::t_ulong;
  case Variant::t_stdstring:
  case Variant::t_string:
    return Variant::t_string;
  default:
  case Variant::t_longlong:
  case Variant::t_ulonglong:
  case Variant::t_bool:
  case Variant::t_nil:
    return type;
  }
}

inline std::pair<bool, Variant::type> 
normalized_type (Variant::type type1, Variant::type type2)
{
  type1 = normalized_type (type1);
  type2 = normalized_type (type2);

  if (type1 == Variant::t_nil || type2 == Variant::t_nil) {
    return std::make_pair (type1 == type2, Variant::t_nil);
  } else if (type1 == Variant::t_string || type2 == Variant::t_string) {
    return std::make_pair (true, Variant::t_string);
  } else if (type1 == Variant::t_double || type2 == Variant::t_double) {
    return std::make_pair (true, Variant::t_double);
  } else if (type1 == Variant::t_longlong || type2 == Variant::t_longlong) {
    return std::make_pair (true, Variant::t_longlong);
  } else if (type1 == Variant::t_ulonglong || type2 == Variant::t_ulonglong) {
    return std::make_pair (true, Variant::t_ulonglong);
  } else if (type1 == Variant::t_long || type2 == Variant::t_long) {
    return std::make_pair (true, Variant::t_long);
  } else if (type1 == Variant::t_ulong || type2 == Variant::t_ulong) {
    return std::make_pair (true, Variant::t_ulong);
  } else {
    return std::make_pair (type1 == type2, type1);
  }
}


bool 
Variant::operator== (const tl::Variant &d) const
{
  std::pair<bool, type> tt = normalized_type (m_type, d.m_type);
  if (! tt.first) {
    return false;
  }
  type t = tt.second;

  if (t == t_nil) {
    return true;
  } else if (t == t_bool) {
    return m_var.m_bool == d.m_var.m_bool;
  } else if (t == t_ulong) {
    return to_ulong () == d.to_ulong ();
  } else if (t == t_long) {
    return to_long () == d.to_long ();
  } else if (t == t_ulonglong) {
    return to_ulonglong () == d.to_ulonglong ();
  } else if (t == t_longlong) {
    return to_longlong () == d.to_longlong ();
  } else if (t == t_id) {
    return m_var.m_id == d.m_var.m_id;
  } else if (t == t_double) {
    return to_double () == d.to_double ();
  } else if (t == t_string) {
    return strcmp (to_string (), d.to_string ()) == 0;
  } else if (t == t_list) {
    return *m_var.m_list == *d.m_var.m_list;
  } else if (t == t_array) {
    return *m_var.m_array == *d.m_var.m_array;
  } else if (is_user () || is_complex ()) {
    return m_var.mp_user.cls == d.m_var.mp_user.cls && m_var.mp_user.cls->equal (m_var.mp_user.object, d.m_var.mp_user.object);
  } else {
    return false;
  }
}

bool 
Variant::operator< (const tl::Variant &d) const
{
  std::pair<bool, type> tt = normalized_type (m_type, d.m_type);
  if (! tt.first) {
    return normalized_type (m_type) < normalized_type (d.m_type);
  }

  type t = tt.second;

  if (t == t_nil) {
    return false;
  } else if (t == t_bool) {
    return m_var.m_bool < d.m_var.m_bool;
  } else if (t == t_ulong) {
    return to_ulong () < d.to_ulong ();
  } else if (t == t_long) {
    return to_long () < d.to_long ();
  } else if (t == t_ulonglong) {
    return to_ulonglong () < d.to_ulonglong ();
  } else if (t == t_longlong) {
    return to_longlong () < d.to_longlong ();
  } else if (t == t_id) {
    return m_var.m_id < d.m_var.m_id;
  } else if (t == t_double) {
    return to_double () < d.to_double ();
  } else if (t == t_string) {
    return strcmp (to_string (), d.to_string ()) < 0;
  } else if (t == t_list) {
    return *m_var.m_list < *d.m_var.m_list;
  } else if (t == t_array) {
    return *m_var.m_array < *d.m_var.m_array;
  } else if (is_user () || is_complex ()) {
    if (m_var.mp_user.cls != d.m_var.mp_user.cls) {
      //  TODO: there should be some class Id that can be used for comparison (that is more predictable)
      return m_var.mp_user.cls < d.m_var.mp_user.cls;
    }
    return m_var.mp_user.cls->less (m_var.mp_user.object, d.m_var.mp_user.object);
  } else {
    return false;
  }
}

bool 
Variant::can_convert_to_float () const
{
  switch (m_type) {
  case t_float:
  case t_char:
  case t_uchar:
  case t_schar:
  case t_short:
  case t_ushort:
  case t_int:
  case t_uint:
  case t_long:
  case t_ulong:
  case t_longlong:
  case t_ulonglong:
  case t_bool:
  case t_nil:
    return true;
  case t_double:
    return m_var.m_double < std::numeric_limits<float>::max () && m_var.m_double > std::numeric_limits<float>::min ();
  case t_stdstring:
  case t_string:
    {
      tl::Extractor ex (to_string ());
      double d;
      return ex.try_read (d) && ex.at_end ();
    }
  default:
    return false;
  }
}

bool 
Variant::can_convert_to_double () const
{
  switch (m_type) {
  case t_double:
  case t_float:
  case t_char:
  case t_uchar:
  case t_schar:
  case t_short:
  case t_ushort:
  case t_int:
  case t_uint:
  case t_long:
  case t_ulong:
  case t_longlong:
  case t_ulonglong:
  case t_bool:
  case t_nil:
    return true;
  case t_stdstring:
  case t_string:
    {
      tl::Extractor ex (to_string ());
      double d;
      return ex.try_read (d) && ex.at_end ();
    }
  default:
    return false;
  }
}

bool 
Variant::can_convert_to_ulonglong () const
{
  switch (m_type) {
  case t_double:
    return m_var.m_double <= std::numeric_limits<unsigned long long>::max () && m_var.m_double >= std::numeric_limits<unsigned long long>::min ();
  case t_float:
    return m_var.m_float <= float (std::numeric_limits<unsigned long long>::max ()) && m_var.m_float >= float (std::numeric_limits<unsigned long long>::min ());
  case t_longlong:
    return m_var.m_longlong >= 0;
  case t_ulonglong:
  case t_ulong:
  case t_bool:
  case t_uchar:
  case t_ushort:
  case t_uint:
  case t_nil:
    return true;
  case t_long:
    return m_var.m_long >= 0;
  case t_char:
    return m_var.m_char >= 0;
  case t_schar:
    return m_var.m_schar >= 0;
  case t_short:
    return m_var.m_short >= 0;
  case t_int:
    return m_var.m_int >= 0;
  case t_string:
  case t_stdstring:
    {
      tl::Extractor ex (to_string ());
      unsigned long long ll;
      return ex.try_read (ll) && ex.at_end ();
    }
  default:
    return false;
  }
}

bool 
Variant::can_convert_to_longlong () const
{
  switch (m_type) {
  case t_double:
    return m_var.m_double <= std::numeric_limits<long long>::max () && m_var.m_double >= std::numeric_limits<long long>::min ();
  case t_float:
    return m_var.m_float <= float (std::numeric_limits<long long>::max ()) && m_var.m_float >= float (std::numeric_limits<long long>::min ());
  case t_ulonglong:
    return m_var.m_ulonglong <= (unsigned long long) std::numeric_limits<long long>::max ();
  case t_longlong:
  case t_ulong:
  case t_long:
  case t_bool:
  case t_char:
  case t_uchar:
  case t_schar:
  case t_short:
  case t_ushort:
  case t_int:
  case t_uint:
  case t_nil:
    return true;
  case t_string:
  case t_stdstring:
    {
      tl::Extractor ex (to_string ());
      long long ll;
      return ex.try_read (ll) && ex.at_end ();
    }
  default:
    return false;
  }
}

bool 
Variant::can_convert_to_ulong () const
{
  switch (m_type) {
  case t_double:
    return m_var.m_double <= std::numeric_limits<unsigned long>::max () && m_var.m_double >= std::numeric_limits<unsigned long>::min ();
  case t_float:
    return m_var.m_float <= std::numeric_limits<unsigned long>::max () && m_var.m_float >= std::numeric_limits<unsigned long>::min ();
  case t_longlong:
    return m_var.m_longlong >= 0 && m_var.m_longlong < (long long) std::numeric_limits<unsigned long>::max ();
  case t_ulonglong:
    return m_var.m_ulonglong < (unsigned long long) std::numeric_limits<unsigned long>::max ();
  case t_ulong:
  case t_bool:
  case t_uchar:
  case t_ushort:
  case t_uint:
  case t_nil:
    return true;
  case t_long:
    return m_var.m_long >= 0;
  case t_char:
    return m_var.m_char >= 0;
  case t_schar:
    return m_var.m_schar >= 0;
  case t_short:
    return m_var.m_short >= 0;
  case t_int:
    return m_var.m_int >= 0;
  case t_string:
  case t_stdstring:
    {
      tl::Extractor ex (to_string ());
      unsigned long l;
      return ex.try_read (l) && ex.at_end ();
    }
  default:
    return false;
  }
}

bool 
Variant::can_convert_to_long () const
{
  switch (m_type) {
  case t_double:
    return m_var.m_double <= std::numeric_limits<long>::max () && m_var.m_double >= std::numeric_limits<long>::min ();
  case t_float:
    return m_var.m_float <= std::numeric_limits<long>::max () && m_var.m_float >= std::numeric_limits<long>::min ();
  case t_ulonglong:
    return m_var.m_ulonglong <= (unsigned long long) std::numeric_limits<long>::max ();
  case t_longlong:
    return m_var.m_longlong >= (long long) std::numeric_limits<long>::min () && m_var.m_longlong <= (long long) std::numeric_limits<long>::max ();
  case t_ulong:
    return m_var.m_ulong <= (unsigned long) std::numeric_limits<long>::max ();
  case t_long:
  case t_bool:
  case t_char:
  case t_uchar:
  case t_schar:
  case t_short:
  case t_ushort:
  case t_int:
  case t_uint:
  case t_nil:
    return true;
  case t_string:
  case t_stdstring:
    {
      tl::Extractor ex (to_string ());
      long l;
      return ex.try_read (l) && ex.at_end ();
    }
  default:
    return false;
  }
}

bool 
Variant::can_convert_to_int () const
{
  return can_convert_to_long () && (to_long () <= (long) std::numeric_limits<int>::max () && to_long () >= (long) std::numeric_limits<int>::min ());
}

bool 
Variant::can_convert_to_uint () const
{
  return can_convert_to_long () && (to_long () <= (long) std::numeric_limits<unsigned int>::max () && to_long () >= (long) std::numeric_limits<unsigned int>::min ());
}

bool 
Variant::can_convert_to_short () const
{
  return can_convert_to_long () && (to_long () <= (long) std::numeric_limits<short>::max () && to_long () >= (long) std::numeric_limits<short>::min ());
}

bool 
Variant::can_convert_to_ushort () const
{
  return can_convert_to_long () && (to_long () <= (long) std::numeric_limits<unsigned short>::max () && to_long () >= (long) std::numeric_limits<unsigned short>::min ());
}

bool 
Variant::can_convert_to_char () const
{
  return can_convert_to_long () && (to_long () <= (long) std::numeric_limits<char>::max () && to_long () >= (long) std::numeric_limits<char>::min ());
}

bool 
Variant::can_convert_to_schar () const
{
  return can_convert_to_long () && (to_short () <= (long) std::numeric_limits<signed char>::max () && to_short () >= (long) std::numeric_limits<signed char>::min ());
}

bool 
Variant::can_convert_to_uchar () const
{
  return can_convert_to_long () && (to_short () <= (long) std::numeric_limits<unsigned char>::max () && to_short () >= (long) std::numeric_limits<unsigned char>::min ());
}

std::string
Variant::to_stdstring () const
{
  if (m_type == t_stdstring) {
    return *m_var.m_stdstring;
  } else {
    return std::string (to_string ());
  }
}

const char *
Variant::to_string () const
{
  if (m_type == t_stdstring) {

    return m_var.m_stdstring->c_str ();

  // conversion needed
  } else if (! m_string) {

    std::string r;

    if (m_type == t_nil) {
      r = "nil";
    } else if (m_type == t_double) {
      r = tl::to_string (m_var.m_double);
    } else if (m_type == t_float) {
      r = tl::to_string (m_var.m_float);
    } else if (m_type == t_char) {
      r = tl::to_string ((int) m_var.m_char);
    } else if (m_type == t_schar) {
      r = tl::to_string ((int) m_var.m_schar);
    } else if (m_type == t_uchar) {
      r = tl::to_string ((int) m_var.m_uchar);
    } else if (m_type == t_short) {
      r = tl::to_string ((int) m_var.m_short);
    } else if (m_type == t_ushort) {
      r = tl::to_string ((int) m_var.m_ushort);
    } else if (m_type == t_int) {
      r = tl::to_string (m_var.m_int);
    } else if (m_type == t_uint) {
      r = tl::to_string (m_var.m_uint);
    } else if (m_type == t_long) {
      r = tl::to_string (m_var.m_long);
    } else if (m_type == t_ulong) {
      r = tl::to_string (m_var.m_ulong);
    } else if (m_type == t_longlong) {
      r = tl::to_string (m_var.m_longlong);
    } else if (m_type == t_ulonglong) {
      r = tl::to_string (m_var.m_ulonglong);
    } else if (m_type == t_bool) {
      r = tl::to_string (m_var.m_bool);
    } else if (m_type == t_list) {
      for (std::vector<tl::Variant>::const_iterator v = m_var.m_list->begin (); v != m_var.m_list->end (); ++v) {
        if (v != m_var.m_list->begin ()) {
          r += ",";
        }
        r += v->to_string ();
      }
    } else if (m_type == t_array) {
      for (const_array_iterator v = m_var.m_array->begin (); v != m_var.m_array->end (); ++v) {
        if (v != m_var.m_array->begin ()) {
          r += ",";
        }
        r += v->first.to_string ();
        r += "=>";
        r += v->second.to_string ();
      }
    } else if (m_type == t_id)  {
      r = "[id" + tl::to_string (m_var.m_id) + "]";
    } else if (is_user () || is_complex ()) {
      r = m_var.mp_user.cls->to_string (m_var.mp_user.object);
    } else {
      r = "[unknown]";
    }

    m_string = new char [r.size () + 1];
    strcpy (m_string, r.c_str ());

  }

  return m_string;
}

bool
Variant::to_bool () const
{
  if (m_type == t_nil) {
    return false;
  } else if (m_type == t_bool) {
    return m_var.m_bool;
  } else {
    return true;
  }
}

unsigned long long
Variant::to_ulonglong () const
{
  if (m_type == t_nil) {
    return 0;
  } else if (m_type == t_double) {
    return (unsigned long long) (m_var.m_double);
  } else if (m_type == t_float) {
    return (unsigned long long) (m_var.m_float);
  } else if (m_type == t_uchar) {
    return m_var.m_uchar;
  } else if (m_type == t_schar) {
    return m_var.m_schar;
  } else if (m_type == t_char) {
    return m_var.m_char;
  } else if (m_type == t_ushort) {
    return m_var.m_ushort;
  } else if (m_type == t_short) {
    return m_var.m_short;
  } else if (m_type == t_uint) {
    return m_var.m_uint;
  } else if (m_type == t_int) {
    return m_var.m_int;
  } else if (m_type == t_ulong) {
    return m_var.m_ulong;
  } else if (m_type == t_long) {
    return m_var.m_long;
  } else if (m_type == t_ulonglong) {
    return m_var.m_ulonglong;
  } else if (m_type == t_longlong) {
    return m_var.m_longlong;
  } else if (m_type == t_bool) {
    return m_var.m_bool;
  } else if (m_type == t_stdstring) {
    unsigned long long l = 0;
    tl::from_string (*m_var.m_stdstring, l);
    return l;
  } else if (m_type == t_string) {
    unsigned long long l = 0;
    tl::from_string (to_string (), l);
    return l;
  } else {
    return 0;
  }
}

long long
Variant::to_longlong () const
{
  if (m_type == t_nil) {
    return 0;
  } else if (m_type == t_double) {
    return (long long) (m_var.m_double);
  } else if (m_type == t_float) {
    return (long long) (m_var.m_float);
  } else if (m_type == t_uchar) {
    return m_var.m_uchar;
  } else if (m_type == t_schar) {
    return m_var.m_schar;
  } else if (m_type == t_char) {
    return m_var.m_char;
  } else if (m_type == t_ushort) {
    return m_var.m_ushort;
  } else if (m_type == t_short) {
    return m_var.m_short;
  } else if (m_type == t_uint) {
    return m_var.m_uint;
  } else if (m_type == t_int) {
    return m_var.m_int;
  } else if (m_type == t_ulong) {
    return m_var.m_ulong;
  } else if (m_type == t_long) {
    return m_var.m_long;
  } else if (m_type == t_ulonglong) {
    return m_var.m_ulonglong;
  } else if (m_type == t_longlong) {
    return m_var.m_longlong;
  } else if (m_type == t_bool) {
    return m_var.m_bool;
  } else if (m_type == t_stdstring) {
    long long l = 0;
    tl::from_string (*m_var.m_stdstring, l);
    return l;
  } else if (m_type == t_string) {
    long long l = 0;
    tl::from_string (to_string (), l);
    return l;
  } else {
    return 0;
  }
}

unsigned long
Variant::to_ulong () const
{
  if (m_type == t_nil) {
    return 0;
  } else if (m_type == t_double) {
    return (unsigned long) (m_var.m_double);
  } else if (m_type == t_float) {
    return (unsigned long) (m_var.m_float);
  } else if (m_type == t_uchar) {
    return m_var.m_uchar;
  } else if (m_type == t_schar) {
    return m_var.m_schar;
  } else if (m_type == t_char) {
    return m_var.m_char;
  } else if (m_type == t_ushort) {
    return m_var.m_ushort;
  } else if (m_type == t_short) {
    return m_var.m_short;
  } else if (m_type == t_uint) {
    return m_var.m_uint;
  } else if (m_type == t_int) {
    return m_var.m_int;
  } else if (m_type == t_ulong) {
    return m_var.m_ulong;
  } else if (m_type == t_long) {
    return m_var.m_long;
  } else if (m_type == t_ulonglong) {
    return m_var.m_ulonglong;
  } else if (m_type == t_longlong) {
    return m_var.m_longlong;
  } else if (m_type == t_bool) {
    return m_var.m_bool;
  } else if (m_type == t_stdstring) {
    unsigned long l = 0;
    tl::from_string (*m_var.m_stdstring, l);
    return l;
  } else if (m_type == t_string) {
    unsigned long l = 0;
    tl::from_string (to_string (), l);
    return l;
  } else {
    return 0;
  }
}

long 
Variant::to_long () const
{
  if (m_type == t_nil) {
    return 0;
  } else if (m_type == t_double) {
    return (long) (m_var.m_double);
  } else if (m_type == t_float) {
    return (long) (m_var.m_float);
  } else if (m_type == t_uchar) {
    return m_var.m_uchar;
  } else if (m_type == t_schar) {
    return m_var.m_schar;
  } else if (m_type == t_char) {
    return m_var.m_char;
  } else if (m_type == t_ushort) {
    return m_var.m_ushort;
  } else if (m_type == t_short) {
    return m_var.m_short;
  } else if (m_type == t_uint) {
    return m_var.m_uint;
  } else if (m_type == t_int) {
    return m_var.m_int;
  } else if (m_type == t_ulong) {
    return m_var.m_ulong;
  } else if (m_type == t_long) {
    return m_var.m_long;
  } else if (m_type == t_ulonglong) {
    return m_var.m_ulonglong;
  } else if (m_type == t_longlong) {
    return m_var.m_longlong;
  } else if (m_type == t_bool) {
    return m_var.m_bool;
  } else if (m_type == t_stdstring) {
    long l = 0;
    tl::from_string (*m_var.m_stdstring, l);
    return l;
  } else if (m_type == t_string) {
    long l = 0;
    tl::from_string (to_string (), l);
    return l;
  } else {
    return 0;
  }
}

int 
Variant::to_int () const
{
  return (int) to_long ();
}

unsigned int 
Variant::to_uint () const
{
  return (unsigned int) to_ulong ();
}

short 
Variant::to_short () const
{
  return (short) to_long ();
}

unsigned short 
Variant::to_ushort () const
{
  return (unsigned short) to_ulong ();
}

char 
Variant::to_char () const
{
  return (char) to_long ();
}

signed char 
Variant::to_schar () const
{
  return (signed char) to_long ();
}

unsigned char 
Variant::to_uchar () const
{
  return (unsigned char) to_ulong ();
}

size_t 
Variant::to_id () const
{
  if (m_type == t_id) {
    return m_var.m_id;
  } else {
    return 0;
  }
}

double 
Variant::to_double () const
{
  if (m_type == t_nil) {
    return 0;
  } else if (m_type == t_double) {
    return m_var.m_double;
  } else if (m_type == t_float) {
    return m_var.m_float;
  } else if (m_type == t_uchar) {
    return m_var.m_uchar;
  } else if (m_type == t_schar) {
    return m_var.m_schar;
  } else if (m_type == t_char) {
    return m_var.m_char;
  } else if (m_type == t_ushort) {
    return m_var.m_ushort;
  } else if (m_type == t_short) {
    return m_var.m_short;
  } else if (m_type == t_uint) {
    return m_var.m_uint;
  } else if (m_type == t_int) {
    return m_var.m_int;
  } else if (m_type == t_ulong) {
    return m_var.m_ulong;
  } else if (m_type == t_long) {
    return m_var.m_long;
  } else if (m_type == t_ulonglong) {
    return m_var.m_ulonglong;
  } else if (m_type == t_longlong) {
    return m_var.m_longlong;
  } else if (m_type == t_bool) {
    return m_var.m_bool;
  } else if (m_type == t_stdstring) {
    double d = 0;
    tl::from_string (*m_var.m_stdstring, d);
    return d;
  } else if (m_type == t_string) {
    double d = 0;
    tl::from_string (to_string (), d);
    return d;
  } else {
    return 0;
  }
}

float 
Variant::to_float () const
{
  return to_double ();
}

const void *
Variant::native_ptr () const
{
  if (is_user () || is_complex ()) {
    return m_var.mp_user.object;
  }

  switch (m_type) {
  case t_double:
    return &m_var.m_double;
  case t_float:
    return &m_var.m_float;
  case t_ulonglong:
    return &m_var.m_ulonglong;
  case t_longlong:
    return &m_var.m_longlong;
  case t_ulong:
    return &m_var.m_ulong;
  case t_long:
    return &m_var.m_long;
  case t_bool:
    return &m_var.m_bool;
  case t_char:
    return &m_var.m_char;
  case t_uchar:
    return &m_var.m_uchar;
  case t_schar:
    return &m_var.m_schar;
  case t_short:
    return &m_var.m_short;
  case t_ushort:
    return &m_var.m_ushort;
  case t_int:
    return &m_var.m_int;
  case t_uint:
    return &m_var.m_uint;
  case t_string:
    return m_string;
  case t_stdstring:
    return m_var.m_stdstring;
  case t_array:
    return m_var.m_array;
  case t_list:
    return m_var.m_list;
  case t_nil:
  default:
    return 0;
  }
}

tl::Variant
Variant::empty_list ()
{
  static std::vector<tl::Variant> empty_list;
  return tl::Variant (empty_list.begin (), empty_list.end ());
}

tl::Variant
Variant::empty_array ()
{
  tl::Variant e;
  e.set_array ();
  return e;
}

tl::Variant *
tl::Variant::find (const tl::Variant &k)
{
  if (m_type != t_array) {
    return 0;
  } else {
    array_iterator a = m_var.m_array->find (k);
    if (a != m_var.m_array->end ()) {
      return &a->second;
    } else {
      return 0;
    }
  }
}

/**
 *  @brief Returns the value for the given key or 0 if the variant is not an array or does not contain the key
 */
const tl::Variant *
tl::Variant::find (const tl::Variant &k) const
{
  if (m_type != t_array) {
    return 0;
  } else {
    const_array_iterator a = m_var.m_array->find (k);
    if (a != m_var.m_array->end ()) {
      return &a->second;
    } else {
      return 0;
    }
  }
}

std::string 
Variant::to_parsable_string () const
{
  if (is_long () || is_int () || is_short () || is_char () || is_schar ()) {
    return "#" + tl::to_string (to_long ());
  } else if (is_longlong ()) {
    return "#l" + tl::to_string (to_longlong ());
  } else if (is_ulong () || is_uint () || is_ushort () || is_uchar ()) {
    return "#u" + tl::to_string (to_ulong ());
  } else if (is_ulonglong ()) {
    return "#lu" + tl::to_string (to_ulonglong ());
  } else if (is_double () || is_float ()) {
    return "##" + tl::to_string (to_double ());
  } else if (is_bool ()) {
    return m_var.m_bool ? "true" : "false";
  } else if (is_nil ()) {
    return "nil";
  } else if (is_stdstring ()) {
    return tl::to_quoted_string (*m_var.m_stdstring);
  } else if (is_cstring ()) {
    return tl::to_quoted_string (to_string ());
  } else if (is_list ()) {
    std::string r = "(";
    for (tl::Variant::const_iterator l = begin (); l != end (); ++l) {
      if (l != begin ()) {
        r += ",";
      }
      r += l->to_parsable_string ();
    }
    r += ")";
    return r;
  } else if (is_array ()) {
    std::string r = "{";
    for (tl::Variant::const_array_iterator l = begin_array (); l != end_array (); ++l) {
      if (l != begin_array ()) {
        r += ",";
      }
      r += l->first.to_parsable_string ();
      r += "=>";
      r += l->second.to_parsable_string ();
    }
    r += "}";
    return r;
  } else if (is_id ()) {
    return "[id" + tl::to_string (m_var.m_id) + "]";
  } else if (is_user ()) {
    return "[user_type]";
  } else if (is_complex ()) {
    if (m_var.mp_user.cls) {
      return "[" + std::string (m_var.mp_user.cls->name ()) + ":" + m_var.mp_user.cls->to_string (m_var.mp_user.object) + "]";
    } else {
      return "[complex_type]";
    }
  } else {
    return "";
  }
}

void 
tl::Variant::swap (tl::Variant &other)
{
  std::swap (m_type, other.m_type);
  std::swap (m_string, other.m_string);

  ValueHolder a = m_var;
  m_var = other.m_var;
  other.m_var = a;
}

// ----------------------------------------------------------------------------------
//  Extractor implementation

template <>
KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, tl::Variant &v)
{
  if (! test_extractor_impl (ex, v)) {
    ex.error (tl::translate ("Expected a value specification"));
  }
}

template <>
KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, tl::Variant &v)
{
  std::string s;

  if (ex.test ("##")) {

    double x = 0;
    ex.read (x);
    v = x;
    return true;

  } else if (ex.test ("#lu")) {

    unsigned long long x = 0;
    ex.read (x);
    v = x;
    return true;

  } else if (ex.test ("#l")) {

    long long x = 0;
    ex.read (x);
    v = x;
    return true;

  } else if (ex.test ("#u")) {

    unsigned long x = 0;
    ex.read (x);
    v = x;
    return true;

  } else if (ex.test ("#")) {

    long x = 0;
    ex.read (x);
    v = x;
    return true;

  } else if (ex.test ("nil")) {

    v = tl::Variant ();
    return true;

  } else if (ex.test ("false")) {

    v = false;
    return true;

  } else if (ex.test ("true")) {

    v = true;
    return true;

  } else if (ex.test ("[")) {

    std::string cls;
    ex.read_word_or_quoted (cls);

    const VariantUserClassBase *ccls = find_cls_by_name (cls.c_str ());
    if (ccls) {
      void *obj = ccls->create ();
      v.set_complex ((tl::Variant::type) ccls->type_code (), obj, ccls);
      ex.test (":");
      ccls->read (obj, ex);
    }

    ex.test ("]");
    return true;

  } else if (ex.test ("(")) {

    std::vector<tl::Variant> values;
    if (! ex.test (")")) {
      while (true) {
        values.push_back (tl::Variant ());
        ex.read (values.back ());
        if (ex.test (",")) {
          //  .. continue
        } else {
          ex.expect (")");
          break;
        }
      }
    }
    v = tl::Variant (values.begin (), values.end ());
    return true;

  } else if (ex.test ("{")) {

    v = tl::Variant::empty_array ();
    if (! ex.test ("}")) {
      while (true) {
        tl::Variant k, x;
        ex.read (k);
        if (ex.test ("=>")) {
          ex.read (x);
        }
        v.insert (k, x);
        if (ex.test (",")) {
          //  .. continue
        } else {
          ex.expect ("}");
          break;
        }
      }
    }
    return true;

  } else if (ex.try_read_word_or_quoted (s)) {

    v = tl::Variant (s);
    return true;

  } else {

    return false;

  }
}

} // namespace tl

