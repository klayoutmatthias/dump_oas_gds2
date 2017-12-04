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



#ifndef HDR_dbPoint
#define HDR_dbPoint

#include "config.h"

#include "dbTypes.h"
#include "tlString.h"

#include <string>

namespace db {

template <class C> class vector;

/**
 *  @brief A point class
 */

template <class C>
class point 
{
public:
  typedef C coord_type;
  typedef db::coord_traits<C> coord_traits;
  typedef db::vector<C> vector_type;
  typedef typename coord_traits::distance_type distance_type; 
  typedef typename coord_traits::area_type area_type; 

  /** 
   *  @brief Default constructor
   *
   *  Creates a point at 0,0
   */
  point () : m_x (0), m_y (0) { }

  /**
   *  @brief Standard constructor
   *
   *  @param x The x coordinate
   *  @param y The y coordinate
   */
  point (C x, C y) : m_x (x), m_y (y) { }

  /**
   *  @brief The copy constructor 
   *
   *  @param d The source from which to copy
   */
  point (const point<C> &d) : m_x (d.x ()), m_y (d.y ()) { }

  /**
   *  @brief The copy constructor that converts also
   *
   *  The copy constructor allows to convert between different
   *  coordinate types, if possible.
   *
   *  @param d The source from which to copy
   */
  template <class D>
  point (const point<D> &d) : m_x (coord_traits::rounded (d.x ())), m_y (coord_traits::rounded (d.y ())) { }

  /**
   *  @brief Standard conversion from DPoint 
   */
  static point<C> from_double (const point<double> &p);

  /**
   *  @brief Add to operation
   */
  point<C> &operator+= (const vector<C> &v);

  /**
   *  @brief Add to operation
   */
  point<C> &operator+= (const point<C> &p);

  /**
   *  @brief method version of operator+ (mainly for automation purposes)
   */
  point<C> add (const vector<C> &v) const;

  /**
   *  @brief method version of operator+ (mainly for automation purposes)
   */
  point<C> add (const point<C> &p) const;

  /**
   *  @brief Subtract from operation
   */
  point<C> &operator-= (const vector<C> &v);
  
  /**
   *  @brief Subtract from operation
   */
  point<C> &operator-= (const point<C> &p);
  
  /**
   *  @brief method version of operator- (mainly for automation purposes)
   */
  point<C> subtract (const vector<C> &v) const;

  /**
   *  @brief method version of operator- (mainly for automation purposes)
   */
  point<C> subtract (const point<C> &p) const;

  /**
   *  @brief "less" comparison operator
   *
   *  This operator is provided to establish a sorting
   *  order
   */
  bool operator< (const point<C> &p) const;

  /**
   *  @brief Equality test operator
   */
  bool operator== (const point<C> &p) const;

  /**
   *  @brief Inequality test operator
   */
  bool operator!= (const point<C> &p) const;

  /**
   *  @brief Const transform
   *
   *  Transforms the point with the given transformation
   *  without modifying the point.
   *
   *  @param t The transformation to apply
   *  @return The transformed point
   */
  template <class Tr>
  point<typename Tr::target_coord_type> transformed (const Tr &t) const;

  /**
   *  @brief In-place transformation
   *
   *  Transforms the point with the given transformation
   *  and writes the result back to the point.
   *
   *  @param t The transformation to apply
   *  @return The transformed point
   */
  template <class Tr>
  point &transform (const Tr &t);

  /**
   *  @brief Accessor to the x coordinate
   */
  C x () const;

  /**
   *  @brief Accessor to the y coordinate
   */
  C y () const;

  /**
   *  @brief Write accessor to the x coordinate
   */
  void set_x (C _x);

  /**
   *  @brief Write accessor to the y coordinate
   */
  void set_y (C _y);

  /**
   *  @brief Scaling self by some factor
   *
   *  Scaline involves rounding which in our case is simply handled
   *  with the coord_traits scheme.
   */
  point<C> &operator*= (double s);

  /**
   *  @brief Scaling self by some integer factor
   */
  point<C> &operator*= (long s);

  /**
   *  @brief The euclidian distance to another point
   *
   *  @param d The other to compute the distance to.
   */
  distance_type distance (const point<C> &p) const;

  /**
   *  @brief The euclidian distance of the point to (0,0)
   */
  distance_type distance () const;

  /**
   *  @brief The euclidian distance to another point as double value
   *
   *  @param d The other to compute the distance to.
   */
  double double_distance (const point<C> &p) const;

  /**
   *  @brief The euclidian distance of the point to (0,0) as double value
   */
  double double_distance () const;

  /**
   *  @brief The square euclidian distance to another point
   *
   *  @param d The other to compute the distance to.
   */
  area_type sq_distance (const point<C> &p) const;

  /**
   *  @brief The square euclidian distance to point (0,0)
   *
   *  @param d The other to compute the distance to.
   */
  area_type sq_distance () const;

  /**
   *  @brief The square of the euclidian distance to another point as double value
   *
   *  @param d The other to compute the distance to.
   */
  double sq_double_distance (const point<C> &p) const;

  /**
   *  @brief The square of the euclidian distance of the point to (0,0) as double value
   */
  double sq_double_distance () const;

  /**
   *  @brief Default conversion to string
   */
  std::string to_string () const
  {
    return to_string (0.0);
  }

  /**
   *  @brief String conversion
   *
   *  If dbu is set, it determines the factor by which the coordinates are multiplied to render
   *  micron units. In addition, a micron format is choosen for output of these coordinates.
   */
  std::string
  to_string (double dbu) const 
  {
    if (dbu == 1.0) {
      return tl::db_to_string (m_x) + "," + tl::db_to_string (m_y);
    } else if (dbu > 0.0) {
      return tl::micron_to_string (dbu * m_x) + "," + tl::micron_to_string (dbu * m_y);
    } else {
      return tl::to_string (m_x) + "," + tl::to_string (m_y);
    }
  }

  /**
   *  @brief Fuzzy comparison of points
   */
  bool equal (const point<C> &p) const;

  /**
   *  @brief Fuzzy "less" comparison of points
   */
  bool less (const point<C> &p) const;

private:
  C m_x, m_y;
};

template <class C>
inline point<C> 
point<C>::from_double (const point<double> &p)
{
  return point<C> (coord_traits::rounded (p.x ()), coord_traits::rounded (p.y ()));
}

template <class C>
inline point<C> &
point<C>::operator+= (const vector<C> &v)
{
  m_x += v.x ();
  m_y += v.y ();
  return *this;
}

template <class C>
inline point<C> &
point<C>::operator+= (const point<C> &p)
{
  m_x += p.x ();
  m_y += p.y ();
  return *this;
}

template <class C>
inline point<C> 
point<C>::add (const vector<C> &v) const
{
  point<C> r (*this);
  r += v;
  return r;
}

template <class C>
inline point<C> 
point<C>::add (const point<C> &p) const
{
  point<C> r (*this);
  r += p;
  return r;
}

template <class C>
inline point<C> &
point<C>::operator-= (const vector<C> &v)
{
  m_x -= v.x ();
  m_y -= v.y ();
  return *this;
}

template <class C>
inline point<C> &
point<C>::operator-= (const point<C> &p)
{
  m_x -= p.x ();
  m_y -= p.y ();
  return *this;
}

template <class C>
inline point<C> 
point<C>::subtract (const vector<C> &v) const
{
  point<C> r (*this);
  r -= v;
  return r;
}

template <class C>
inline point<C> 
point<C>::subtract (const point<C> &p) const
{
  point<C> r (*this);
  r -= p;
  return r;
}

template <class C>
inline bool 
point<C>::operator< (const point<C> &p) const
{
  return m_y < p.m_y || (m_y == p.m_y && m_x < p.m_x);
}

template <class C>
inline bool 
point<C>::less (const point<C> &p) const
{
  if (! coord_traits::equal (y (), p.y ())) {
    return y () < p.y ();
  }
  if (! coord_traits::equal (x (), p.x ())) {
    return x () < p.x ();
  }
  return false;
}

template <class C>
inline bool 
point<C>::operator== (const point<C> &p) const
{
  return m_x == p.m_x && m_y == p.m_y;
}

template <class C>
inline bool 
point<C>::equal (const point<C> &p) const
{
  return coord_traits::equal (x (), p.x ()) && coord_traits::equal (y (), p.y ());
}

template <class C>
inline bool 
point<C>::operator!= (const point<C> &p) const
{
  return !operator== (p);
}

template <class C> template <class Tr>
inline point<typename Tr::target_coord_type> 
point<C>::transformed (const Tr &t) const
{
  return t (*this);
}

template <class C> template <class Tr>
inline point<C> &
point<C>::transform (const Tr &t)
{
  *this = t (*this);
  return *this;
}

template <class C>
inline C 
point<C>::x () const
{
  return m_x;
}

template <class C>
inline C 
point<C>::y () const
{
  return m_y;
}

template <class C>
inline void 
point<C>::set_x (C _x) 
{
  m_x = _x;
}

template <class C>
inline void 
point<C>::set_y (C _y) 
{
  m_y = _y;
}

template <class C>
inline point<double> 
operator* (const db::point<C> &p, double s) 
{
  return point<double> (p.x () * s, p.y () * s);
}

template <class C>
inline point<C> 
operator* (const db::point<C> &p, long s) 
{
  return point<C> (p.x () * s, p.y () * s);
}

template <class C>
inline point<C> 
operator* (const db::point<C> &p, unsigned long s) 
{
  return point<C> (p.x () * s, p.y () * s);
}

template <class C>
inline point<C> 
operator* (const db::point<C> &p, int s) 
{
  return point<C> (p.x () * s, p.y () * s);
}

template <class C>
inline point<C> 
operator* (const db::point<C> &p, unsigned int s) 
{
  return point<C> (p.x () * s, p.y () * s);
}

template <class C>
inline point<C> &
point<C>::operator*= (double s) 
{
  m_x = coord_traits::rounded (m_x * s);
  m_y = coord_traits::rounded (m_y * s);
  return *this;
}

template <class C>
inline point<C> &
point<C>::operator*= (long s) 
{
  m_x = coord_traits::rounded (m_x * s);
  m_y = coord_traits::rounded (m_y * s);
  return *this;
}

template <class C>
inline typename point<C>::distance_type 
point<C>::distance (const point<C> &p) const
{
  double ddx (p.x ());
  double ddy (p.y ());
  ddx -= double (x ());
  ddy -= double (y ());
  return coord_traits::rounded_distance (sqrt (ddx * ddx + ddy * ddy));
}

template <class C>
inline typename point<C>::distance_type 
point<C>::distance () const
{
  double ddx (x ());
  double ddy (y ());
  return coord_traits::rounded_distance (sqrt (ddx * ddx + ddy * ddy));
}

template <class C>
inline double 
point<C>::double_distance (const point<C> &p) const
{
  double ddx (p.x ());
  double ddy (p.y ());
  ddx -= double (x ());
  ddy -= double (y ());
  return sqrt (ddx * ddx + ddy * ddy);
}

template <class C>
inline double 
point<C>::double_distance () const
{
  double ddx (x ());
  double ddy (y ());
  return sqrt (ddx * ddx + ddy * ddy);
}

template <class C>
inline typename point<C>::area_type 
point<C>::sq_distance (const point<C> &p) const
{
  return coord_traits::sq_length (p.x (), p.y (), x (), y ());
}

template <class C>
inline typename point<C>::area_type 
point<C>::sq_distance () const
{
  return coord_traits::sq_length (0, 0, x (), y ());
}

template <class C>
inline double 
point<C>::sq_double_distance (const point<C> &p) const
{
  double ddx (p.x ());
  double ddy (p.y ());
  ddx -= double (x ());
  ddy -= double (y ());
  return ddx * ddx + ddy * ddy;
}

template <class C>
inline double 
point <C>::sq_double_distance () const
{
  double ddx (x ());
  double ddy (y ());
  return ddx * ddx + ddy * ddy;
}

/**
 *  @brief The binary + operator (addition point and vector)
 *
 *  @param p The first point
 *  @param v The second point
 *  @return p + v
 */
template <class C>
inline point<C>
operator+ (point<C> p, const vector<C> &v)
{
  p += v;
  return p;
}

/**
 *  @brief The binary + operator (addition of points)
 *
 *  @param p1 The first point
 *  @param p2 The second point
 *  @return p1 + p2
 */
template <class C>
inline point<C>
operator+ (const point<C> &p1, const point<C> &p2)
{
  point<C> p (p1);
  p += p2;
  return p;
}

/**
 *  @brief The binary - operator (addition of points)
 *
 *  @param p1 The first point
 *  @param p2 The second point
 *  @return p1 - p2
 */
template <class C>
inline point<C>
operator- (point<C> p, const vector<C> &v)
{
  p -= v;
  return p;
}

/**
 *  @brief The binary - operator (addition of points)
 *
 *  @param p1 The first point
 *  @param p2 The second point
 *  @return p1 - p2
 */
template <class C>
inline point<C>
operator- (const point<C> &p1, const point<C> &p2)
{
  point<C> p (p1);
  p -= p2;
  return p;
}

/**
 *  @brief The unary - operator 
 *
 *  @param p The point 
 *  @return -p = (-p.x, -p.y)
 */
template <class C>
inline point<C> 
operator- (const point<C> &p)
{
  return point<C> (-p.x (), -p.y ());
}

/**
 *  @brief The stream insertion operator
 */
template <class C>
inline std::ostream &
operator<< (std::ostream &os, const point<C> &p)
{
  return (os << p.to_string ());
}

/**
 *  @brief The short integer point
 */
typedef point <short> ShortPoint;

/**
 *  @brief The standard point
 */
typedef point <db::Coord> Point;

/**
 *  @brief The standard double coordinate point
 */
typedef point <db::DCoord> DPoint;

/**
 *  @brief Convenience wrappers for coord_traits functions: vector product: 0->p x 0->q
 */
template <class C>
typename db::coord_traits<C>::area_type vprod (const db::point<C> &p, const db::point<C> &q)
{
  return db::coord_traits<C>::vprod (p.x (), p.y (), q.x (), q.y (), 0, 0);
}

/**
 *  @brief Convenience wrappers for coord_traits functions: vector product sign: sign(0->p x 0->q)
 */
template <class C>
int vprod_sign (const db::point<C> &p, const db::point<C> &q)
{
  return db::coord_traits<C>::vprod_sign (p.x (), p.y (), q.x (), q.y (), 0, 0);
}

/**
 *  @brief Convenience wrappers for coord_traits functions: scalar product: 0->p x 0->q
 */
template <class C>
typename db::coord_traits<C>::area_type sprod (const db::point<C> &p, const db::point<C> &q)
{
  return db::coord_traits<C>::sprod (p.x (), p.y (), q.x (), q.y (), 0, 0);
}

/**
 *  @brief Convenience wrappers for coord_traits functions: scalar product sign: sign(0->p x 0->q)
 */
template <class C>
int sprod_sign (const db::point<C> &p, const db::point<C> &q)
{
  return db::coord_traits<C>::sprod_sign (p.x (), p.y (), q.x (), q.y (), 0, 0);
}

/**
 *  @brief Convenience wrappers for coord_traits functions: vector product: o->p x o->q
 */
template <class C>
typename db::coord_traits<C>::area_type vprod (const db::point<C> &p, const db::point<C> &q, const db::point<C> &o)
{
  return db::coord_traits<C>::vprod (p.x (), p.y (), q.x (), q.y (), o.x (), o.y ());
}

/**
 *  @brief Convenience wrappers for coord_traits functions: vector product sign: sign(o->p x o->q)
 */
template <class C>
int vprod_sign (const db::point<C> &p, const db::point<C> &q, const db::point<C> &o)
{
  return db::coord_traits<C>::vprod_sign (p.x (), p.y (), q.x (), q.y (), o.x (), o.y ());
}

/**
 *  @brief Convenience wrappers for coord_traits functions: scalar product: o->p x o->q
 */
template <class C>
typename db::coord_traits<C>::area_type sprod (const db::point<C> &p, const db::point<C> &q, const db::point<C> &o)
{
  return db::coord_traits<C>::sprod (p.x (), p.y (), q.x (), q.y (), o.x (), o.y ());
}

/**
 *  @brief Convenience wrappers for coord_traits functions: scalar product sign: sign(o->p x o->q)
 */
template <class C>
int sprod_sign (const db::point<C> &p, const db::point<C> &q, const db::point<C> &o)
{
  return db::coord_traits<C>::sprod_sign (p.x (), p.y (), q.x (), q.y (), o.x (), o.y ());
}

/**
 *  @brief A generic conversion operator from double point to any type
 */
template <class C>
struct from_double_point 
{
  db::point<C> operator() (const DPoint &dp) const
  {
    return db::point<C>::from_double (dp);
  }
};

}

/**
 *  @brief Special extractors for the points
 */

namespace tl 
{

  template <> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::Point &p);
  template <> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::DPoint &p);

  template <> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::Point &p);
  template <> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::DPoint &p);

} // namespace tl

#endif

