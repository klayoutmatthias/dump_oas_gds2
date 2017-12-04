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


#ifndef _HDR_tlVariant
#define _HDR_tlVariant

#include "config.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "tlInternational.h"
#include "tlAssert.h"

#include <QtCore/QString>

namespace tl
{

class Extractor;
template <class T> KLAYOUT_DLL void extractor_impl (tl::Extractor &, T &);

template<class T> std::string to_string (const T &o);

/**
 *  @brief A base class which describes a class, i.e. an object capable of converting and handling void *
 *
 *  There must be one particular class object per class. In particular the equality of the 
 *  class object pointer's must indicate that two object's share the same class.
 */
class KLAYOUT_DLL VariantUserClassBase
{
public:
  VariantUserClassBase () { }
  virtual ~VariantUserClassBase () { }

  virtual void *create () const = 0;
  virtual void destroy (void *) const = 0;
  virtual bool equal (void *, void *) const = 0;
  virtual bool less (void *, void *) const = 0;
  virtual void *clone (void *) const = 0;
  virtual std::string to_string (void *) const = 0;
  virtual void read (void *, tl::Extractor &ex) const = 0;
  virtual const char *name () const = 0;
  virtual unsigned int type_code () const = 0;
};

/**
 *  @brief A derived class encapsulating a certain user type without the capability to compare
 */
template <class T>
class VariantUserClass
  : public VariantUserClassBase
{
public:
  VariantUserClass () { }

  virtual void *create () const 
  {
    return new T ();
  }

  virtual void destroy (void *t) const 
  {
    delete (T *)t;
  }

  //  TODO: not implemented to be more generic
  virtual bool equal (void *, void *) const { tl_assert (false); return false; }
  virtual bool less (void *, void *) const { tl_assert (false); return false; }

  virtual void *clone (void *t) const 
  { 
    return new T(*(T *)t);
  }

  std::string to_string (void *t) const { return tl::to_string (*(T *)t); }
  void read (void *, tl::Extractor &ex) const { }
  const char *name () const { return ""; }
  unsigned int type_code () const { return 0; }

  T &get (void *t) const { return *(T *)t; }
  const T &get (const void *t) const { return *(const T *)t; }

  static KLAYOUT_DLL VariantUserClass<T> &instance ()
  {
    return ms_instance;
  }

  /**
   *  @brief DLL local instance
   *  Use this version instead of instance() in DLL code if T is defined inside the DLL
   */
  static VariantUserClass<T> &dll_local_instance ()
  {
    return ms_instance;
  }

private:
  static VariantUserClass<T> ms_instance;
};

template <class T> VariantUserClass<T> VariantUserClass<T>::ms_instance;

/**
 *  @brief A class providing an equality operator
 */
template <class T>
class VariantUserClassWithEqual
  : public VariantUserClass<T>
{
public:
  VariantUserClassWithEqual () : VariantUserClass<T> () { }

  virtual bool equal (void *a, void *b) const 
  { 
    return *(const T *)a == *(const T *)b;
  }

  static KLAYOUT_DLL VariantUserClassWithEqual<T> &instance ()
  {
    return ms_instance;
  }

  /**
   *  @brief DLL local instance
   *  Use this version instead of instance() in DLL code if T is defined inside the DLL
   */
  static VariantUserClassWithEqual<T> &dll_local_instance ()
  {
    return ms_instance;
  }

private:
  static VariantUserClassWithEqual<T> ms_instance;
};

template <class T> VariantUserClassWithEqual<T> VariantUserClassWithEqual<T>::ms_instance;

/**
 *  @brief A class providing an equality operator
 */
template <class T>
class VariantUserClassWithEqualAndLess
  : public VariantUserClassWithEqual<T>
{
public:
  VariantUserClassWithEqualAndLess () : VariantUserClassWithEqual<T> () { }

  virtual bool less (void *a, void *b) const 
  { 
    return *(const T *)a < *(const T *)b;
  }

  static KLAYOUT_DLL VariantUserClassWithEqualAndLess<T> &instance ()
  {
     return ms_instance;
  }

  /**
   *  @brief DLL local instance
   *  Use this version instead of instance() in DLL code if T is defined inside the DLL
   */
  static VariantUserClassWithEqualAndLess<T> &dll_local_instance ()
  {
    return ms_instance;
  }

private:
  static VariantUserClassWithEqualAndLess<T> ms_instance;
};

template <class T> VariantUserClassWithEqualAndLess<T> VariantUserClassWithEqualAndLess<T>::ms_instance;

void register_variant_complex_type (const VariantUserClassBase *cls, unsigned int type, const char *name);

/**
 *  @brief A class providing everything required for the complex types
 */
template <class T>
class VariantComplexTypeClass
  : public VariantUserClassWithEqualAndLess<T>
{
public:
  VariantComplexTypeClass (unsigned int t, const char *name) 
    : VariantUserClassWithEqualAndLess<T> (), m_type_code (t), mp_name (name)
  { 
    register_variant_complex_type (this, t, name);
  }

  void read (void *t, tl::Extractor &ex) const
  {
    extractor_impl<T> (ex, *(T* )t);
  }

  unsigned int type_code () const
  {
    return m_type_code;
  }

  const char *name () const
  {
    return mp_name;
  }

  static KLAYOUT_DLL VariantComplexTypeClass<T> &instance ()
  {
    return ms_instance;
  }

  /**
   *  @brief DLL local instance
   *  Use this version instead of instance() in DLL code if T is defined inside the DLL
   */
  static VariantComplexTypeClass<T> &dll_local_instance ()
  {
    return ms_instance;
  }

private:
  static VariantComplexTypeClass<T> ms_instance;
  unsigned int m_type_code;
  const char *mp_name;
};

/**
 *   @brief A basic variant type
 *
 *   This variant is capable of storing long, double, std::string, void (nil) and lists
 *   of other variants.
 */
class KLAYOUT_DLL Variant
{
public:
  enum type { 
    t_nil, 
    t_bool, 
    t_char, 
    t_schar, 
    t_uchar, 
    t_short, 
    t_ushort, 
    t_int, 
    t_uint, 
    t_long, 
    t_ulong, 
    t_longlong, 
    t_ulonglong, 
    t_id, 
    t_float, 
    t_double, 
    t_string, 
    t_stdstring, 
    t_qstring, 
    t_list, 
    t_array, 
    //  "complex types": these are types which are basically predefined user types.
    //  Access to such values is only provided through the to_user<T> method.
    t_complex_first,
    t_point,    //  -> db::Point
    t_dpoint,   //  -> db::DPoint
    t_box,      //  -> db::Box
    t_dbox,     //  -> db::DBox
    t_edge,     //  -> db::Edge
    t_dedge,    //  -> db::DEdge
    t_vector,   //  -> db::Vector
    t_dvector,  //  -> db::DVector
    t_polygon,  //  -> db::Polygon
    t_dpolygon, //  -> db::DPolygon
    t_path,     //  -> db::Path
    t_dpath,    //  -> db::DPath
    t_layer,    //  -> db::LayerProperties
    //  "real" user types
    t_user 
  };

  typedef std::vector<tl::Variant>::const_iterator const_iterator;
  typedef std::vector<tl::Variant>::iterator iterator;
  typedef std::map<tl::Variant, tl::Variant> array_type;
  typedef array_type::const_iterator const_array_iterator;
  typedef array_type::iterator array_iterator;

  /**
   *  @brief Initialize the Variant with "nil"
   */
  Variant ();

  /**
   *  @brief Copy ctor
   */
  Variant (const tl::Variant &d);

  /**
   *  @brief Initialize the Variant with a QString
   */
  Variant (const QString &s);

  /**
   *  @brief Initialize the Variant with "string"
   */
  Variant (const std::string &s);

  /**
   *  @brief Initialize the Variant with "string"
   */
  Variant (const char *s);

  /**
   *  @brief Initialize the Variant with "double"
   */
  Variant (double d);

  /**
   *  @brief Initialize the Variant with "float"
   */
  Variant (float d);

  /**
   *  @brief Initialize the Variant with "char"
   */
  Variant (char c);

  /**
   *  @brief Initialize the Variant with "signed char"
   */
  Variant (signed char c);

  /**
   *  @brief Initialize the Variant with "unsigned char"
   */
  Variant (unsigned char c);

  /**
   *  @brief Initialize the Variant with "short"
   */
  Variant (short s);

  /**
   *  @brief Initialize the Variant with "unsigned short"
   */
  Variant (unsigned short s);

  /**
   *  @brief Initialize the Variant with "bool"
   */
  Variant (bool l);

  /**
   *  @brief Initialize the Variant with "int" (actually "long")
   */
  Variant (int l);

  /**
   *  @brief Initialize the Variant with "unsigned int" (actually "unsigned long")
   */
  Variant (unsigned int l);

  /**
   *  @brief Initialize the Variant with "long"
   */
  Variant (long l);

  /**
   *  @brief Initialize the Variant with "unsigned long"
   */
  Variant (unsigned long l);
 
  /**
   *  @brief Initialize the Variant with "long long"
   */
  Variant (long long l);

  /**
   *  @brief Initialize the Variant with "unsigned long long"
   */
  Variant (unsigned long long l);
 
  /**
   *  @brief Initialize the Variant with an "id" 
   *
   *  The "id" type is basically a size_t, but is supposed to be used as a representative for another value.
   *  One application for that type is a placeholder for an OASIS name until it is associated with a real value.
   */
  Variant (size_t l, bool /*dummy*/);

  /**
   *  @brief Initialize with a user type based on VariantUserBase
   *
   *  The Variant will take over the ownership over the VariantUserBase object.
   */
  Variant (unsigned int utype, void *object, const VariantUserClassBase *cls)
    : m_type (type (t_user + utype)), m_string (0)
  {
    m_var.mp_user.object = object;
    m_var.mp_user.cls = cls;
  }

  /**
   *  @brief Initialize with a complex type 
   */
  template <class T>
  Variant (const T &obj)
    : m_string (0)
  {
    m_type = (tl::Variant::type) VariantComplexTypeClass<T>::instance ().type_code ();
    m_var.mp_user.object = new T (obj);
    m_var.mp_user.cls = &VariantComplexTypeClass<T>::instance ();
  }

  /**
   *  @brief Initialize the Variant with a list
   */
  template <class Iter>
  Variant (Iter from, Iter to)
    : m_type (t_list), m_string (0)
  {
    m_var.m_list = new std::vector<tl::Variant> (from, to);
  }

  /**
   *  @brief Destructor
   */
  ~Variant ();

  /**
   *  @brief Assignment
   */
  Variant &operator= (const Variant &v);

  /**
   *  @brief Assignment of a string
   */
  Variant &operator= (const char *v);

  /**
   *  @brief Assignment of a string
   */
  Variant &operator= (const QString &v);

  /**
   *  @brief Assignment of a string
   */
  Variant &operator= (const std::string &v);

  /**
   *  @brief Assignment of a double
   */
  Variant &operator= (double d);

  /**
   *  @brief Assignment of a float
   */
  Variant &operator= (float d);

  /**
   *  @brief Assignment of a bool
   */
  Variant &operator= (bool l);

  /**
   *  @brief Assignment of a char
   */
  Variant &operator= (char l);

  /**
   *  @brief Assignment of an unsigned char
   */
  Variant &operator= (unsigned char c);

  /**
   *  @brief Assignment of a signed char
   */
  Variant &operator= (signed char c);

  /**
   *  @brief Assignment of a short
   */
  Variant &operator= (short s);

  /**
   *  @brief Assignment of an unsigned char
   */
  Variant &operator= (unsigned short s);

  /**
   *  @brief Assignment of a int
   */
  Variant &operator= (int l);

  /**
   *  @brief Assignment of a unsigned int
   */
  Variant &operator= (unsigned int l);

  /**
   *  @brief Assignment of a long
   */
  Variant &operator= (long l);

  /**
   *  @brief Assignment of a unsigned long
   */
  Variant &operator= (unsigned long l);

  /**
   *  @brief Assignment of a long long
   */
  Variant &operator= (long long l);

  /**
   *  @brief Assignment of a unsigned long long
   */
  Variant &operator= (unsigned long long l);

  /**
   *  @brief Reset to nil
   */
  void reset ();

  /**
   *  @brief Initialize with a user type
   */
  void set_user (unsigned int utype, void *object, const VariantUserClassBase *cls)
  {
    reset ();
    m_type = type (t_user + utype);
    m_var.mp_user.object = object;
    m_var.mp_user.cls = cls;
  }

  /**
   *  @brief Initialize with a complex type
   */
  void set_complex (type t, void *object, const VariantUserClassBase *cls)
  {
    reset ();
    m_type = t;
    m_var.mp_user.object = object;
    m_var.mp_user.cls = cls;
  }

  /**
   *  @brief Initialize with a complex type
   */
  template <class T> 
  void set_complex (const T &obj)
  {
    reset ();
    m_type = (tl::Variant::type) VariantComplexTypeClass<T>::instance ().type_code ();
    m_var.mp_user.object = new T (obj);
    m_var.mp_user.cls = &VariantComplexTypeClass<T>::instance ();
  }

  /**
   *  @brief Initialize with an empty list with the given reserve
   */
  void set_list (size_t reserve = 0)
  {
    reset ();
    m_type = t_list;
    m_var.m_list = new std::vector<tl::Variant> ();
    if (reserve > 0) {
      m_var.m_list->reserve (reserve);
    }
  }

  /**
   *  @brief Initialize with an empty array
   */
  void set_array ()
  {
    reset ();
    m_type = t_array;
    m_var.m_array = new std::map<tl::Variant, tl::Variant> ();
  }

  /**  
   *  @brief Equality
   *
   *  For user types, this is not implemented yet.
   */
  bool operator== (const Variant &d) const;

  /**  
   *  @brief Inequality
   *
   *  For user types, this is not implemented yet.
   */
  bool operator!= (const Variant &d) const
  {
    return !operator== (d);
  }

  /**  
   *  @brief Comparison
   *
   *  For user types, this is not implemented yet.
   */
  bool operator< (const Variant &d) const;

  /**
   *  @brief Conversion to a string
   *
   *  This performs the conversion to a string as far as possible.
   *  No conversion is provided to user types currently.
   */
  const char *to_string () const;

  /**
   *  @brief Conversion to a QString
   *
   *  This performs the conversion to a QString as far as possible.
   *  No conversion is provided to user types currently.
   */
  QString to_qstring () const;

  /**
   *  @brief Conversion to a std::string
   *
   *  This performs the conversion to a QString as far as possible.
   *  No conversion is provided to user types currently.
   */
  std::string to_stdstring () const;

  /**
   *  @brief Conversion to a unsigned long long
   *
   *  This performs the conversion to a unsigned long long as far as possible.
   *  No conversion is provided to user types currently.
   */
  unsigned long long to_ulonglong () const;

  /**
   *  @brief Conversion to a long long
   *
   *  This performs the conversion to a long long as far as possible.
   *  No conversion is provided to user types currently.
   */
  long long to_longlong () const;

  /**
   *  @brief Conversion to a unsigned int
   *
   *  This performs the conversion to a unsigned int as far as possible.
   *  No conversion is provided to user types currently.
   */
  unsigned int to_uint () const;

  /**
   *  @brief Conversion to a int
   *
   *  This performs the conversion to a int as far as possible.
   *  No conversion is provided to user types currently.
   */
  int to_int () const;

  /**
   *  @brief Conversion to a unsigned long
   *
   *  This performs the conversion to a unsigned long as far as possible.
   *  No conversion is provided to user types currently.
   */
  unsigned long to_ulong () const;

  /**
   *  @brief Conversion to a long
   *
   *  This performs the conversion to a long as far as possible.
   *  No conversion is provided to user types currently.
   */
  long to_long () const;

  /**
   *  @brief Conversion to a unsigned long
   *
   *  This performs the conversion to a unsigned short as far as possible.
   *  No conversion is provided to user types currently.
   */
  unsigned short to_ushort () const;

  /**
   *  @brief Conversion to a short
   *
   *  This performs the conversion to a short as far as possible.
   *  No conversion is provided to user types currently.
   */
  short to_short () const;

  /**
   *  @brief Conversion to a signed char
   *
   *  This performs the conversion to a signed char as far as possible.
   *  No conversion is provided to user types currently.
   */
  signed char to_schar () const;

  /**
   *  @brief Conversion to a unsigned char
   *
   *  This performs the conversion to an unsigned char as far as possible.
   *  No conversion is provided to user types currently.
   */
  unsigned char to_uchar () const;

  /**
   *  @brief Conversion to a char
   *
   *  This performs the conversion to a char as far as possible.
   *  No conversion is provided to user types currently.
   */
  char to_char () const;

  /**
   *  @brief Conversion to an id
   *
   *  This gets the id value if the variant is an id.
   *  No conversion is provided to user types currently.
   */
  size_t to_id () const;

  /**
   *  @brief Conversion to a bool
   *
   *  This performs the conversion to a bool as far as possible.
   *  No conversion is provided to user types currently.
   */
  bool to_bool () const;

  /**
   *  @brief Conversion to a double
   *
   *  This performs the conversion to a double value as far as possible.
   *  No conversion is provided to user types currently.
   */
  double to_double () const;

  /**
   *  @brief Conversion to a float
   *
   *  This performs the conversion to a float value as far as possible.
   *  No conversion is provided to user types currently.
   */
  float to_float () const;

  /**
   *  @brief conversion to a standard type 
   *
   *  This is a templatized version of the various to_... methods. This
   *  does also not include conversion to a user type.
   *  This is the generic version. Specializations follow.
   */
  template <class T>
  T to () const
  {
    tl_assert (false);
  }

  /** 
   *  @brief Convert to the user object
   */
  void *to_user_object ()
  {
    return m_var.mp_user.object;
  }

  /** 
   *  @brief Convert to the user object (const)
   */
  const void *to_user_object () const
  {
    return m_var.mp_user.object;
  }

  /** 
   *  @brief Get the user object's class
   */
  const VariantUserClassBase *user_cls () const
  {
    return m_var.mp_user.cls;
  }

  /** 
   *  @brief Convert to the given complex type
   */
  template <class T> 
  T &to_complex ()
  {
    tl_assert (is_complex ());
    const VariantUserClass<T> *tcls = dynamic_cast<const VariantUserClass<T> *> (m_var.mp_user.cls);
    tl_assert (tcls != 0);
    return tcls->get (m_var.mp_user.object);
  }

  /** 
   *  @brief Convert to the given complex type (const version)
   */
  template <class T> 
  const T &to_complex () const
  {
    tl_assert (is_complex ());
    const VariantUserClass<T> *tcls = dynamic_cast<const VariantUserClass<T> *> (m_var.mp_user.cls);
    tl_assert (tcls != 0);
    return tcls->get (m_var.mp_user.object);
  }

  /** 
   *  @brief Convert to the given user type
   */
  template <class T> 
  T &to_user ()
  {
    tl_assert (is_user ());
    const VariantUserClass<T> *tcls = dynamic_cast<const VariantUserClass<T> *> (m_var.mp_user.cls);
    tl_assert (tcls != 0);
    return tcls->get (m_var.mp_user.object);
  }

  /** 
   *  @brief Convert to the given user type (const version)
   */
  template <class T> 
  const T &to_user () const
  {
    tl_assert (is_user ());
    const VariantUserClass<T> *tcls = dynamic_cast<const VariantUserClass<T> *> (m_var.mp_user.cls);
    tl_assert (tcls != 0);
    return tcls->get (m_var.mp_user.object);
  }

  /**
   *  @brief Morph to the given type
   *
   *  After morphing the variant, the variant will use the given type internally.
   *  The native pointer can be used to access the value then.
   *  A nil value is not morphed and remains nil. In that case, the native pointer will be 0.
   */
  template<class T>
  tl::Variant &morph ()
  {
    if (! is_nil ()) {
      *this = to<T> ();
    }
    return *this;
  }

  /**
   *  @brief Cast to the given type
   *
   *  This creates a new variant which uses the given type internally
   */
  template <class T>
  Variant cast () const
  {
    return Variant (to<T> ());
  }

  /**
   *  @brief Access the native (internal) object 
   *
   *  For nil, 0 is returned.
   */
  void *native_ptr ()
  {
    //  saves one implementation ...
    return const_cast<void *> (((const tl::Variant *) this)->native_ptr ());
  }

  /**
   *  @brief Access the native (internal) object 
   *
   *  For nil, 0 is returned.
   */
  const void *native_ptr () const;

  /**
   *  @brief Get the list iterators, if it is one
   */
  const_iterator begin () const
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->begin ();
  }

  /**
   *  @brief Get the list iterators, if it is one
   */
  const_iterator end () const
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->end ();
  }

  /**
   *  @brief Get the list iterators, if it is one
   */
  iterator begin () 
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->begin ();
  }

  /**
   *  @brief Get the list iterators, if it is one
   */
  iterator end () 
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->end ();
  }

  /**
   *  @brief Reserve some length for a list
   */ 
  void reserve (size_t n) 
  {
    tl_assert (m_type == t_list);
    m_var.m_list->reserve (n);
  }

  /**
   *  @brief Get the length of the list if there is one, otherwise 0
   */ 
  size_t size () const
  {
    return m_type == t_list ? m_var.m_list->size () : 0;
  }

  /**
   *  @brief Add a element to the list
   */
  void push (const tl::Variant &v)
  {
    tl_assert (m_type == t_list);
    m_var.m_list->push_back (v);
  }

  /**
   *  @brief Get the back element of the list
   */
  tl::Variant &back ()
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->back ();
  }

  /**
   *  @brief Get the back element of the list (const)
   */
  const tl::Variant &back () const
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->back ();
  }

  /**
   *  @brief Get the front element of the list
   */
  tl::Variant &front ()
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->front ();
  }

  /**
   *  @brief Get the front element of the list (const)
   */
  const tl::Variant &front () const
  {
    tl_assert (m_type == t_list);
    return m_var.m_list->front ();
  }

  /**
   *  @brief Get the list, if it is one
   */
  std::vector<tl::Variant> &get_list ()
  {
    tl_assert (m_type == t_list);
    return *m_var.m_list;
  }

  /**
   *  @brief Get the list, if it is one (const)
   */
  const std::vector<tl::Variant> &get_list () const
  {
    tl_assert (m_type == t_list);
    return *m_var.m_list;
  }

  /**
   *  @brief Get the array iterators, if it is one
   */
  const_array_iterator begin_array () const
  {
    tl_assert (m_type == t_array);
    return m_var.m_array->begin ();
  }

  /**
   *  @brief Get the array iterators, if it is one
   */
  const_array_iterator end_array () const
  {
    tl_assert (m_type == t_array);
    return m_var.m_array->end ();
  }

  /**
   *  @brief Get the array iterators, if it is one
   */
  array_iterator begin_array () 
  {
    tl_assert (m_type == t_array);
    return m_var.m_array->begin ();
  }

  /**
   *  @brief Get the array iterators, if it is one
   */
  array_iterator end_array () 
  {
    tl_assert (m_type == t_array);
    return m_var.m_array->end ();
  }

  /**
   *  @brief Get the length of the array if there is one, otherwise 0
   */ 
  size_t array_size () const
  {
    return m_type == t_array ? m_var.m_array->size () : 0;
  }

  /**
   *  @brief Insert an element into the array
   */
  void insert (const tl::Variant &k, const tl::Variant &v)
  {
    tl_assert (m_type == t_array);
    m_var.m_array->insert (std::make_pair (k, v));
  }

  /**
   *  @brief Returns the value for the given key or 0 if the variant is not an array or does not contain the key
   */
  tl::Variant *find (const tl::Variant &k);

  /**
   *  @brief Returns the value for the given key or 0 if the variant is not an array or does not contain the key
   */
  const tl::Variant *find (const tl::Variant &k) const;

  /**
   *  @brief Get the list, if it is one
   */
  array_type &get_array ()
  {
    tl_assert (m_type == t_array);
    return *m_var.m_array;
  }

  /**
   *  @brief Get the list, if it is one (const)
   */
  const array_type &get_array () const
  {
    tl_assert (m_type == t_array);
    return *m_var.m_array;
  }

  /**
   *  @brief Test, if it can convert to a double
   *
   *  All numeric types can convert to double. That is double and the integer types.
   */
  bool can_convert_to_double () const;

  /**
   *  @brief Test, if it can convert to a float
   *
   *  All numeric types can convert to float. That is double and the integer types unless the double value is outside the float range.
   */
  bool can_convert_to_float () const;

  /**
   *  @brief Test, if it can convert to a char
   *
   *  All numeric types can convert to char unless the value is outside the allowed range.
   */
  bool can_convert_to_char () const;

  /**
   *  @brief Test, if it can convert to an signed char
   *
   *  All numeric types can convert to signed char unless the value is outside the allowed range.
   */
  bool can_convert_to_schar () const;

  /**
   *  @brief Test, if it can convert to an unsigned char
   *
   *  All numeric types can convert to unsigned char unless the value is outside the allowed range.
   */
  bool can_convert_to_uchar () const;

  /**
   *  @brief Test, if it can convert to a short
   *
   *  All numeric types can convert to short unless the value is outside the allowed range.
   */
  bool can_convert_to_short () const;

  /**
   *  @brief Test, if it can convert to an unsigned short
   *
   *  All numeric types can convert to unsigned short unless the value is outside the allowed range.
   */
  bool can_convert_to_ushort () const;

  /**
   *  @brief Test, if it can convert to an int
   *
   *  All numeric types can convert to int unless the value is outside the allowed range.
   */
  bool can_convert_to_int () const;

  /**
   *  @brief Test, if it can convert to an unsigned int
   *
   *  All numeric types can convert to unsigned int unless the value is outside the allowed range.
   */
  bool can_convert_to_uint () const;

  /**
   *  @brief Test, if it can convert to a long
   *
   *  All numeric types can convert to long unless the value is outside the allowed range.
   */
  bool can_convert_to_long () const;

  /**
   *  @brief Test, if it can convert to an unsigned long
   *
   *  All numeric types can convert to unsigned long unless the value is outside the allowed range.
   */
  bool can_convert_to_ulong () const;

  /**
   *  @brief Test, if it can convert to a long long
   *
   *  All numeric types can convert to long unless the value is outside the allowed range.
   */
  bool can_convert_to_longlong () const;

  /**
   *  @brief Test, if it can convert to an unsigned long long
   *
   *  All numeric types can convert to unsigned long unless the value is outside the allowed range.
   */
  bool can_convert_to_ulonglong () const;

  /**
   *  @brief Returns true if the conversion to the given type is possible
   *
   *  This is a templatized version of the various can_convert_to_... methods. This
   *  does not include conversion to a user type, arrays or lists.
   *  This is the generic version. Specializations follow.
   */
  template <class T>
  bool can_convert_to () const
  {
    return false;
  }

  /**
   *  @brief Test, if it is a double
   */
  bool is_double () const
  {
    return m_type == t_double;
  }

  /**
   *  @brief Test, if it is a float
   */
  bool is_float () const
  {
    return m_type == t_float;
  }

  /**
   *  @brief Test, if it is a char 
   */
  bool is_char () const
  {
    return m_type == t_char;
  }

  /**
   *  @brief Test, if it is an signed char
   */
  bool is_schar () const
  {
    return m_type == t_schar;
  }

  /**
   *  @brief Test, if it is an unsigned char
   */
  bool is_uchar () const
  {
    return m_type == t_uchar;
  }

  /**
   *  @brief Test, if it is a short 
   */
  bool is_short () const
  {
    return m_type == t_short;
  }

  /**
   *  @brief Test, if it is an unsigned short
   */
  bool is_ushort () const
  {
    return m_type == t_ushort;
  }

  /**
   *  @brief Test, if it is an int 
   */
  bool is_int () const
  {
    return m_type == t_int;
  }

  /**
   *  @brief Test, if it is an unsigned int
   */
  bool is_uint () const
  {
    return m_type == t_uint;
  }

  /**
   *  @brief Test, if it is a long
   */
  bool is_long () const
  {
    return m_type == t_long;
  }

  /**
   *  @brief Test, if it is an unsigned long
   */
  bool is_ulong () const
  {
    return m_type == t_ulong;
  }

  /**
   *  @brief Test, if it is a long long
   */
  bool is_longlong () const
  {
    return m_type == t_longlong;
  }

  /**
   *  @brief Test, if it is a unsigned long long
   */
  bool is_ulonglong () const
  {
    return m_type == t_ulonglong;
  }

  /**
   *  @brief Test, if it is a bool
   */
  bool is_bool () const
  {
    return m_type == t_bool;
  }

  /**
   *  @brief Test, if it is a id
   */
  bool is_id () const
  {
    return m_type == t_id;
  }

  /**
   *  @brief Test, if it is a QString
   */
  bool is_qstring () const
  {
    return m_type == t_qstring;
  }

  /**
   *  @brief Test, if it is a std::string
   */
  bool is_stdstring () const
  {
    return m_type == t_stdstring;
  }

  /**
   *  @brief Test, if it is a "C" string
   */
  bool is_cstring () const
  {
    return m_type == t_string;
  }

  /**
   *  @brief Test, if it is any string
   */
  bool is_a_string () const
  {
    return m_type == t_qstring || m_type == t_string || m_type == t_stdstring;
  }

  /**
   *  @brief Returns true if the variant is of the given type internally
   *
   *  This is a templatized version of the various can_convert_to_... methods. This
   *  does not include conversion to a user type, arrays or lists.
   *  This is the generic version. Specializations follow.
   */
  template <class T>
  bool is () const
  {
    return false;
  }

  /**
   *  @brief Test, if it is nil
   */
  bool is_nil () const
  {
    return m_type == t_nil;
  }

  /**
   *  @brief Test, if it is an array
   */
  bool is_array () const
  {
    return m_type == t_array;
  }

  /**
   *  @brief Test, if it is a list
   */
  bool is_list () const
  {
    return m_type == t_list;
  }

  /**
   *  @brief Test, if this is a complex type
   */
  bool is_complex () const
  {
    return m_type >= t_complex_first;
  }

  /** 
   *  @brief Test, if this is a user type and can convert to the given type
   */
  template <class T> 
  bool is_complex () const
  {
    if (m_type >= t_complex_first) {
      const VariantUserClass<T> *tcls = dynamic_cast<const VariantUserClass<T> *> (m_var.mp_user.cls);
      return tcls != 0;
    } else {
      return false;
    }
  }

  /**
   *  @brief Get the type code
   */
  type type_code () const
  {
    return m_type;
  }

  /**
   *  @brief Test, if this is a user type
   */
  bool is_user () const
  {
    return m_type >= t_user;
  }

  /** 
   *  @brief Test, if this is a user type and can convert to the given type
   */
  template <class T> 
  bool is_user () const
  {
    if (m_type >= t_user) {
      const VariantUserClass<T> *tcls = dynamic_cast<const VariantUserClass<T> *> (m_var.mp_user.cls);
      return tcls != 0;
    } else {
      return false;
    }
  }

  /**
   *  @brief Get the user type if it is one
   */
  unsigned int user_type () const
  {
    tl_assert (m_type >= t_user);
    return (unsigned int) m_type - (unsigned int) t_user;
  }

  /**
   *  @brief Swap contents with another instance
   */
  void swap (tl::Variant &other);

  /**
   *  @brief A method to deliver an empty-list variant
   */
  static tl::Variant empty_list ();

  /**
   *  @brief A method to deliver an empty-array variant
   */
  static tl::Variant empty_array ();

  /**
   *  @brief Convert the Variant to a string that can be parsed with the Extractor
   *
   *  No conversion is provided for user types and "nil" currently.
   */
  std::string to_parsable_string () const;

private:
  type m_type;

  union ValueHolder {
    std::vector<tl::Variant> *m_list;
    std::map<tl::Variant, tl::Variant> *m_array;
    double m_double;
    float m_float;
    char m_char;
    unsigned char m_uchar;
    signed char m_schar;
    short m_short;
    unsigned short m_ushort;
    int m_int;
    unsigned int m_uint;
    long m_long;
    unsigned long m_ulong;
    long long m_longlong;
    unsigned long long m_ulonglong;
    bool m_bool;
    size_t m_id;
    struct {
      void *object;
      const VariantUserClassBase *cls;
    } mp_user;
    QString *m_qstring;
    std::string *m_stdstring;
  } m_var;

  //  this will hold the string if it is valid
  mutable char *m_string;
};

//  specializations of the to ... methods
template<> inline bool Variant::to<bool> () const                                 { return to_bool (); }
template<> inline char Variant::to<char> () const                                 { return to_char (); }
template<> inline unsigned char Variant::to<unsigned char> () const               { return to_uchar (); }
template<> inline signed char Variant::to<signed char> () const                   { return to_schar (); }
template<> inline short Variant::to<short> () const                               { return to_short (); }
template<> inline unsigned short Variant::to<unsigned short> () const             { return to_ushort (); }
template<> inline int Variant::to<int> () const                                   { return to_int (); }
template<> inline unsigned int Variant::to<unsigned int> () const                 { return to_uint (); }
template<> inline long Variant::to<long> () const                                 { return to_long (); }
template<> inline unsigned long Variant::to<unsigned long> () const               { return to_ulong (); }
template<> inline long long Variant::to<long long> () const                       { return to_longlong (); }
template<> inline unsigned long long Variant::to<unsigned long long> () const     { return to_ulonglong (); }
template<> inline double Variant::to<double> () const                             { return to_double (); }
template<> inline float Variant::to<float> () const                               { return to_float (); }
template<> inline std::string Variant::to<std::string> () const                   { return to_stdstring (); }
template<> inline QString Variant::to<QString> () const                           { return to_qstring (); }
template<> inline const char *Variant::to<const char *> () const                  { return to_string (); }

//  specializations if the is.. methods
template<> inline bool Variant::is<bool> () const                 { return is_bool (); }
template<> inline bool Variant::is<char> () const                 { return is_char (); }
template<> inline bool Variant::is<unsigned char> () const        { return is_uchar (); }
template<> inline bool Variant::is<signed char> () const          { return is_schar (); }
template<> inline bool Variant::is<short> () const                { return is_short (); }
template<> inline bool Variant::is<unsigned short> () const       { return is_ushort (); }
template<> inline bool Variant::is<int> () const                  { return is_int (); }
template<> inline bool Variant::is<unsigned int> () const         { return is_uint (); }
template<> inline bool Variant::is<long> () const                 { return is_long (); }
template<> inline bool Variant::is<unsigned long> () const        { return is_ulong (); }
template<> inline bool Variant::is<long long> () const            { return is_longlong (); }
template<> inline bool Variant::is<unsigned long long> () const   { return is_ulonglong (); }
template<> inline bool Variant::is<double> () const               { return is_double (); }
template<> inline bool Variant::is<float> () const                { return is_float (); }
template<> inline bool Variant::is<std::string> () const          { return is_stdstring (); }
template<> inline bool Variant::is<QString> () const              { return is_qstring (); }
template<> inline bool Variant::is<const char *> () const         { return is_cstring (); }

//  specializations of the can_convert.. methods
template<> inline bool Variant::can_convert_to<bool> () const                 { return true; }
template<> inline bool Variant::can_convert_to<char> () const                 { return can_convert_to_char (); }
template<> inline bool Variant::can_convert_to<unsigned char> () const        { return can_convert_to_uchar (); }
template<> inline bool Variant::can_convert_to<signed char> () const          { return can_convert_to_schar (); }
template<> inline bool Variant::can_convert_to<short> () const                { return can_convert_to_short (); }
template<> inline bool Variant::can_convert_to<unsigned short> () const       { return can_convert_to_ushort (); }
template<> inline bool Variant::can_convert_to<int> () const                  { return can_convert_to_int (); }
template<> inline bool Variant::can_convert_to<unsigned int> () const         { return can_convert_to_uint (); }
template<> inline bool Variant::can_convert_to<long> () const                 { return can_convert_to_long (); }
template<> inline bool Variant::can_convert_to<unsigned long> () const        { return can_convert_to_ulong (); }
template<> inline bool Variant::can_convert_to<long long> () const            { return can_convert_to_longlong (); }
template<> inline bool Variant::can_convert_to<unsigned long long> () const   { return can_convert_to_ulonglong (); }
template<> inline bool Variant::can_convert_to<double> () const               { return can_convert_to_double (); }
template<> inline bool Variant::can_convert_to<float> () const                { return can_convert_to_float (); }
template<> inline bool Variant::can_convert_to<std::string> () const          { return true; }
template<> inline bool Variant::can_convert_to<QString> () const              { return true; }
template<> inline bool Variant::can_convert_to<const char *> () const         { return true; }

/**
 *  @brief Special extractors for the variant
 */

template <class T> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, T &);
template <class T> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, T &);

template <> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, tl::Variant &p);
template <> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, tl::Variant &p);

} // namespace tl

#endif

