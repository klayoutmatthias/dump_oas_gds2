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



#include <errno.h> 
#include <string.h> 
#include <ctype.h> 

#include "tlStream.h"
#include "tlDeflate.h"
#include "tlAssert.h"

#include "tlException.h"
#include "tlString.h"

namespace tl
{

// ---------------------------------------------------------------
//  InputStream implementation

InputStream::InputStream (InputStreamBase &delegate)
  : m_recording (false), m_pos (0), mp_bptr (0), mp_delegate (&delegate), mp_inflate (0)
{ 
  m_bcap = 4096; // initial buffer capacity
  m_blen = 0;
  mp_buffer = new char [m_bcap];
}

InputStream::~InputStream ()
{
  if (mp_inflate) {
    delete mp_inflate;
    mp_inflate = 0;
  } 
  if (mp_buffer) {
    delete[] mp_buffer;
    mp_buffer = 0;
  }
}

const char * 
InputStream::get (size_t n, bool bypass_deflate)
{
  //  if deflating, employ the deflate filter to get the data
  if (mp_inflate && ! bypass_deflate) {
    if (! mp_inflate->at_end ()) {

      const char *r = mp_inflate->get (n);
      tl_assert (r != 0);  //  since deflate did not report at_end()
      if (m_recording) {
        m_recorded.insert (m_recorded.end (), r, r + n);
      }
      return r;

    } else {
      delete mp_inflate;
      mp_inflate = 0;
    }
  } 

  if (m_blen < n) {

    //  to keep move activity low, allocate twice as much as required
    if (m_bcap < n * 2) {

      while (m_bcap < n) {
        m_bcap *= 2;
      }

      char *buffer = new char [m_bcap];
      memcpy (buffer, mp_bptr, m_blen);
      delete [] mp_buffer;
      mp_buffer = buffer;

    } else {
      memmove (mp_buffer, mp_bptr, m_blen);
    }

    m_blen += mp_delegate->read (mp_buffer + m_blen, m_bcap - m_blen); 
    mp_bptr = mp_buffer;

  }

  if (m_blen >= n) {
    const char *r = mp_bptr;
    mp_bptr += n;
    m_blen -= n;
    m_pos += n;
    if (m_recording) {
      m_recorded.insert (m_recorded.end (), r, r + n);
    }
    return r;
  } else {
    return 0;
  }

}

void
InputStream::unget (size_t n)
{
  if (m_recording && n >= m_recorded.size ()) {
    m_recorded.erase (m_recorded.end () - n, m_recorded.end ());
  }
  if (mp_inflate) {
    mp_inflate->unget (n);
  } else {
    mp_bptr -= n;
    m_blen += n;
    m_pos -= n;
  }
}

void
InputStream::inflate ()
{
  tl_assert (mp_inflate == 0);
  mp_inflate = new tl::InflateFilter (*this);
}

void 
InputStream::reset ()
{
  mp_delegate->reset ();
  m_pos = 0;

  if (mp_inflate) {
    delete mp_inflate;
    mp_inflate = 0;
  } 
  if (mp_buffer) {
    delete[] mp_buffer;
    mp_buffer = 0;
  }

  mp_bptr = 0;
  m_blen = 0;
  mp_buffer = new char [m_bcap];
}

// ---------------------------------------------------------------
//  ASCIIInputStream implementation

ASCIIInputStream::ASCIIInputStream (InputStreamBase &delegate)
  : InputStream (delegate), m_line (1), m_next_line (1), m_at_end (false)
{ 
  if (get (1) == 0) {
    m_at_end = true;
  } else {
    unget (1);
  }
}

const std::string &
ASCIIInputStream::get_line ()
{
  m_line = m_next_line;
  m_line_buffer.clear ();

  while (! at_end ()) {
    char c = get_char ();
    if (c == '\r') {
      //  simply skip CR 
    } else if (c == '\n' || c == 0) {
      break;
    } else {
      m_line_buffer += c;
    }
  }

  return m_line_buffer;
}

char 
ASCIIInputStream::get_char ()
{
  m_line = m_next_line;
  const char *c = get (1);
  if (c == 0) {
    m_at_end = true;
    return 0;
  } else {
    if (*c == '\n') {
      ++m_next_line;
    }
    return *c;
  }
}

char 
ASCIIInputStream::peek_char ()
{
  m_line = m_next_line;
  const char *c = get (1);
  if (c == 0) {
    m_at_end = true;
    return 0;
  } else {
    char cc = *c;
    unget (1);
    return cc;
  }
}

char 
ASCIIInputStream::skip ()
{
  char c = 0;
  while (! at_end () && isspace (c = peek_char ())) {
    get_char ();
  }
  return at_end () ? 0 : c;
}

void
ASCIIInputStream::reset ()
{
  InputStream::reset ();

  m_line = 1;
  m_next_line = 1;

  if (get (1) == 0) {
    m_at_end = true;
  } else {
    m_at_end = false;
    unget (1);
  }
}

// ---------------------------------------------------------------
//  OutputStream implementation

OutputStream::OutputStream (OutputStreamBase &delegate)
  : m_pos (0), mp_delegate (&delegate), mp_deflate (0)
{ 
  //  .. nothing yet ..
}

OutputStream::~OutputStream ()
{
  if (mp_deflate != 0) {
    delete mp_deflate;
    mp_deflate = 0;
  }
}

void 
OutputStream::begin_deflate ()
{
  tl_assert (mp_deflate == 0);
  mp_deflate = new tl::DeflateFilter (*this);
}

std::pair<size_t, size_t> 
OutputStream::end_deflate ()
{
  tl_assert (mp_deflate != 0);

  //  by detaching the deflate filter first, flush () does not call put () recursively
  tl::DeflateFilter *f = mp_deflate;
  mp_deflate = 0;

  f->flush ();
  std::pair<size_t, size_t> pc = std::make_pair (f->uncompressed (), f->compressed ());

  delete f;

  return pc;
}

void
OutputStream::put (const char *b, size_t n)
{
  if (mp_deflate) {

    //  by detaching the filter first, we don't call put recursively when the 
    //  filter emits the compressed data
    tl::DeflateFilter *f = mp_deflate;
    mp_deflate = 0;
    f->put (b, n);
    mp_deflate = f;

  } else {

    mp_delegate->write (b, n);
    m_pos += n;

  }
}

void
OutputStream::seek (size_t pos)
{
  tl_assert (mp_deflate == 0);
  mp_delegate->seek (pos);
  m_pos = pos;
}

// ---------------------------------------------------------------
//  Input file delegate implementation

InputFile::InputFile (const std::string &path)
  : m_file (NULL)
{
  m_source = path;
#if defined(_WIN32)
  m_file = _wfopen ((const wchar_t *) tl::to_qstring (path).constData (), L"rb");
#else
  m_file = fopen (tl::string_to_system (path).c_str (), "rb");
#endif
  if (m_file == NULL) {
    throw FileOpenErrorException (m_source, errno);
  }
}

InputFile::~InputFile ()
{
  if (m_file != NULL) {
    fclose (m_file);
    m_file = NULL;
  }  
}

size_t 
InputFile::read (char *b, size_t n)
{
  tl_assert (m_file != NULL);
  size_t ret = fread (b, 1, n, m_file);
  if (ret < n) {
    if (ferror (m_file)) {
      throw FileReadErrorException (m_source, ferror (m_file));
    }
  }

  return ret;
}

void 
InputFile::reset ()
{
  if (m_file != NULL) {
    fseek (m_file, 0l, SEEK_SET);
  }
}

// ---------------------------------------------------------------
//  Output file delegate implementation

OutputFile::OutputFile (const std::string &path)
  : m_file (NULL)
{
  m_source = path;
#if defined(_WIN32)
  m_file = _wfopen ((const wchar_t *) tl::to_qstring (path).constData (), L"wb");
#else
  m_file = fopen (tl::string_to_system (path).c_str (), "wb");
#endif
  if (m_file == NULL) {
    throw FileOpenErrorException (m_source, errno);
  }
}

OutputFile::~OutputFile ()
{
  if (m_file != NULL) {
    fclose (m_file);
    m_file = NULL;
  }  
}

void 
OutputFile::seek (size_t s)
{
  tl_assert (m_file != NULL);
  fseek (m_file, s, SEEK_SET);
}

void 
OutputFile::write (const char *b, size_t n)
{
  tl_assert (m_file != NULL);
  size_t ret = fwrite (b, 1, n, m_file);
  if (ret < n) {
    if (ferror (m_file)) {
      throw FileWriteErrorException (m_source, ferror (m_file));
    }
  }
}

// ---------------------------------------------------------------
//  InputZLibFile implementation

InputZLibFile::InputZLibFile (const std::string &path)
  : m_zs (NULL)
{
  m_source = path;
#if defined(_WIN32)
  FILE *file = _wfopen ((const wchar_t *) tl::to_qstring (path).constData (), L"rb");
  if (file == NULL) {
    throw FileOpenErrorException (m_source, errno);
  }
  m_zs = gzdopen (_fileno (file), "rb");
#else
  m_zs = gzopen (tl::string_to_system (path).c_str (), "rb");
#endif
  if (m_zs == NULL) {
    throw FileOpenErrorException (m_source, errno);
  }
}

InputZLibFile::~InputZLibFile ()
{
  if (m_zs != NULL) {
    gzclose (m_zs);
    m_zs = NULL;
  }  
}

size_t 
InputZLibFile::read (char *b, size_t n)
{
  tl_assert (m_zs != NULL);
  int ret = gzread (m_zs, b, n);
  if (ret < 0) {
    int gz_err = 0;
    const char *em = gzerror (m_zs, &gz_err);
    if (gz_err == Z_ERRNO) {
      throw FileReadErrorException (m_source, errno);
    } else {
      throw ZLibReadErrorException (m_source, em);
    }
  }

  return ret;
}

void 
InputZLibFile::reset ()
{
  if (m_zs != NULL) {
    gzrewind (m_zs);
  }
}

// ---------------------------------------------------------------
//  OutputZLibFile implementation

OutputZLibFile::OutputZLibFile (const std::string &path)
  : m_zs (NULL)
{
  m_source = path;
#if defined(_WIN32)
  FILE *file = _wfopen ((const wchar_t *) tl::to_qstring (path).constData (), L"wb");
  if (file == NULL) {
    throw FileOpenErrorException (m_source, errno);
  }
  m_zs = gzdopen (_fileno (file), "wb");
#else
  m_zs = gzopen (tl::string_to_system (path).c_str (), "wb");
#endif
  if (m_zs == NULL) {
    throw FileOpenErrorException (m_source, errno);
  }
}

OutputZLibFile::~OutputZLibFile ()
{
  if (m_zs != NULL) {
    tl_assert (m_zs != NULL);
    gzclose (m_zs);
    m_zs = NULL;
  }  
}

void 
OutputZLibFile::write (const char *b, size_t n)
{
  tl_assert (m_zs != NULL);
  int ret = gzwrite (m_zs, (char *) b, n);
  if (ret < 0) {
    int gz_err = 0;
    const char *em = gzerror (m_zs, &gz_err);
    if (gz_err == Z_ERRNO) {
      throw FileWriteErrorException (m_source, errno);
    } else {
      throw ZLibWriteErrorException (m_source, em);
    }
  }
}

#ifndef _WIN32 // not available on Windows

// ---------------------------------------------------------------
//  InputPipe delegate implementation

InputPipe::InputPipe (const std::string &path)
  : m_file (NULL)
{
  m_source = path;
  m_file = popen (tl::string_to_system (path).c_str (), "r");
  if (m_file == NULL) {
    throw FilePOpenErrorException (m_source, errno);
  }
}

InputPipe::~InputPipe ()
{
  if (m_file != NULL) {
    fclose (m_file);
    m_file = NULL;
  }  
}

size_t 
InputPipe::read (char *b, size_t n)
{
  tl_assert (m_file != NULL);
  size_t ret = fread (b, 1, n, m_file);
  if (ret < n) {
    if (ferror (m_file)) {
      throw FilePReadErrorException (m_source, ferror (m_file));
    }
  }

  return ret;
}

void 
InputPipe::reset ()
{
  throw tl::Exception (tl::translate ("'reset' is not supported on pipeline input files"));
}

// ---------------------------------------------------------------
//  OutputPipe delegate implementation

OutputPipe::OutputPipe (const std::string &path)
  : m_file (NULL)
{
  m_source = path;
  m_file = popen (tl::string_to_system (path).c_str (), "w");
  if (m_file == NULL) {
    throw FilePOpenErrorException (m_source, errno);
  }
}

OutputPipe::~OutputPipe ()
{
  if (m_file != NULL) {
    fclose (m_file);
    m_file = NULL;
  }  
}

void 
OutputPipe::write (const char *b, size_t n)
{
  tl_assert (m_file != NULL);
  size_t ret = fwrite (b, 1, n, m_file);
  if (ret < n) {
    if (ferror (m_file)) {
      throw FilePWriteErrorException (m_source, ferror (m_file));
    }
  }
}

#else

// ---------------------------------------------------------------
//  InputPipe delegate implementation

InputPipe::InputPipe (const std::string &path)
  : m_file (NULL)
{
  // TODO: emulate?
}

InputPipe::~InputPipe ()
{
}

size_t 
InputPipe::read (char *b, size_t n)
{
  throw tl::Exception (tl::translate ("pipeline input files not available on Windows"));
}

void 
InputPipe::reset ()
{
  throw tl::Exception (tl::translate ("pipeline input files not available on Windows"));
}

// ---------------------------------------------------------------
//  OutputPipe delegate implementation

OutputPipe::OutputPipe (const std::string &path)
  : m_file (NULL)
{
  // TODO: emulate?
}

OutputPipe::~OutputPipe ()
{
}

void 
OutputPipe::write (const char *b, size_t n)
{
  throw tl::Exception (tl::translate ("pipeline input files not available on Windows"));
}

#endif

// ---------------------------------------------------------------
//  read_file implementation

void 
read_file (tl::InputStreamBase &stream_base, std::string &into)
{
  tl::InputStream input (stream_base);

  size_t l = 0;
  for (const char *c = input.get (1); c; c = input.get (1), ++l) 
    ;

  into.clear ();
  into.reserve (l);

  input.reset ();

  for (const char *c = input.get (1); c; c = input.get (1)) {
    into += *c;
  }
}

}

