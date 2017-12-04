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


#ifndef HDR_tlStream
#define HDR_tlStream

#include "config.h"

#include "tlException.h"
#include "tlString.h"

#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <zlib.h>


namespace tl
{

class InflateFilter;
class DeflateFilter;

/**
 *  @brief The input stream delegate base class
 *
 *  This class provides the basic input stream functionality.
 *  The actual implementation is provided through InputFile, InputPipe and InputZLibFile.
 */

class KLAYOUT_DLL InputStreamBase
{
public:
  InputStreamBase () { }
  virtual ~InputStreamBase () { }

  /** 
   *  @brief Read a block of n bytes
   *
   *  Read the requested number of bytes or less.
   *  May throw an exception if a read error occures.
   *  
   *  @param b The buffer where to write to
   *  @param n The number of bytes to read (or less)
   *
   *  @return The number of bytes read. Should report 0 on EOF
   */
  virtual size_t read (char *b, size_t n) = 0;

  /**
   *  @brief Seek to the beginning
   */
  virtual void reset () = 0;

  /**
   *  @brief Get the source specification (the file name)
   *
   *  Returns an empty string if no file name is available.
   */
  virtual std::string source () = 0;
};

/**
 *  @brief The output stream delegate base class
 *
 *  This class provides the basic output stream functionality.
 *  The actual implementation is provided through OutputFile, OutputPipe and OutputZLibFile.
 */

class KLAYOUT_DLL OutputStreamBase
{
public:
  OutputStreamBase () { }
  virtual ~OutputStreamBase () { }

  /**
   *  @brief Write a block a n bytes
   *
   *  May throw an exception if a write error occures.
   *
   *  @param b What to write
   *  @param n The number of bytes to write 
   */
  virtual void write (const char *b, size_t n) = 0;

  /**
   *  @brief Seek to the specified position 
   *
   *  Writing continues at that position after a seek.
   */
  virtual void seek (size_t s) 
  {
    //  .. the default implementation does nothing ..
  }

  /**
   *  @brief Returns a value indicating whether that stream supports seek
   */
  virtual bool supports_seek () 
  {
    return false;
  }
};

/**
 *  @brief An input stream abstraction class
 *
 *  The basic objective of this interface is to provide
 *  the capability to read a block of n bytes into a buffer.
 *  This object provides unget capabilities and buffering.
 *  The actual stream access is delegated to another object.
 */

class KLAYOUT_DLL InputStream
{
public:
  /**
   *  @brief Default constructor
   *
   *  This constructor takes a delegate object. 
   */
  InputStream (InputStreamBase &delegate);

  /**
   *  @brief Destructor
   */
  virtual ~InputStream ();

  /** 
   *  @brief This is the outer write method to call
   *  
   *  This implementation writes data through the 
   *  protected write call.
   */
  void put (const char *b, size_t n);

  /** 
   *  @brief This is the outer read method to call
   *  
   *  This implementation obtains data through the 
   *  protected read call and buffers the data accordingly so
   *  a contigous memory block can be returned.
   *  If inline deflating is enabled, the method will return
   *  inflate data unless "bypass_inflate" is set to true.
   *
   *  @return 0 if not enough data can be obtained
   */
  const char *get (size_t n, bool bypass_inflate = false);

  /** 
   *  @brief Undo a previous get call
   *  
   *  This call puts back the bytes read by a previous get call.
   *  Only one call can be made undone.
   */
  void unget (size_t n);

  /**
   *  @brief Enable uncompression of the following DEFLATE-compressed block
   *
   *  This call will enable uncompression of the next block
   *  of DEFLATE (RFC1951) compressed data. Subsequence get() calls will deliver
   *  the uncompressed data rather than the raw data, until the
   *  compressed block is finished.
   *  The stream must not be in inflate state yet.
   */
  void inflate ();

  /**
   *  @brief Obtain the current file position
   */
  size_t pos () const 
  {
    return m_pos;
  }

  /**
   *  @brief Obtain the available number of bytes
   *
   *  This is the number of bytes that can be delivered on the next get, not
   *  the total file size.
   */
  size_t blen () const
  {
    return m_blen;
  }

  /**
   *  @brief Get the source specification (the file name)
   *
   *  Returns an empty string if no file name is available.
   */
  std::string source () 
  {
    return mp_delegate->source ();
  }

  /**
   *  @brief Reset to the initial position
   */
  virtual void reset ();

  /**
   *  @brief Start recording 
   */
  void start_recording ()
  {
    m_recorded.clear ();
    m_recording = true;
  }

  /**
   *  @brief Stop recording
   */
  void stop_recording ()
  {
    m_recorded.clear ();
    m_recording = false;
  }

  /**
   *  @brief Reset recording 
   */
  void reset_recording ()
  {
    m_recorded.clear ();
  }

  /**
   *  @brief Get the recorded bytes 
   */
  size_t n_recorded () const
  {
    return m_recorded.size ();
  }
    
  /**
   *  @brief Get the recorded byte array 
   */
  const char *recorded () const
  {
    return &m_recorded.front ();
  }
    
protected:
  void reset_pos ()
  {
    m_pos = 0;
  }

private:
  std::vector <char> m_recorded;
  bool m_recording;
  size_t m_pos;
  char *mp_buffer;
  size_t m_bcap;
  size_t m_blen;
  char *mp_bptr;
  InputStreamBase *mp_delegate;

  //  inflate support 
  InflateFilter *mp_inflate;

  //  No copying currently
  InputStream (const InputStream &);
  InputStream &operator= (const InputStream &);
};

/**
 *  @brief An ASCII input stream
 */
class KLAYOUT_DLL ASCIIInputStream 
  : public InputStream
{
public:
  /**
   *  @brief Default constructor
   *
   *  This constructor takes a delegate object. 
   */
  ASCIIInputStream (InputStreamBase &delegate);

  /**
   *  @brief Get a single line
   */
  const std::string &get_line ();

  /**
   *  @brief Get a single character
   */
  char get_char ();

  /**
   *  @brief Peek a single character
   */
  char peek_char ();

  /**
   *  @brief Skip blanks, newlines etc.
   *
   *  Returns the following character without getting it.
   */
  char skip ();

  /**
   *  @brief Get the current line number
   */
  size_t line_number ()
  {
    return m_line;
  }

  /**
   *  @brief Return false, if no more characters can be obtained
   */
  bool at_end () const 
  {
    return m_at_end;
  }

  /**
   *  @brief Reset to the initial position
   */
  virtual void reset ();

private:
  size_t m_line, m_next_line;
  bool m_at_end;
  std::string m_line_buffer;
};

/**
 *  @brief An output stream abstraction class
 *
 *  The basic objective of this interface is to provide
 *  the capability to write a block of n bytes into a buffer.
 *  The actual stream access is delegated to another object.
 */

class KLAYOUT_DLL OutputStream
{
public:
  /**
   *  @brief Default constructor
   *
   *  This constructor takes a delegate object. 
   */
  OutputStream (OutputStreamBase &delegate);

  /**
   *  @brief Destructor
   *
   *  This will delete the delegate object passed in the constructor.
   */
  virtual ~OutputStream ();

  /**
   *  @brief Enable inline-DEFLATE (RFC1951) compression of the following bytes
   *
   *  This call will enable inline compressed of the following data
   *  until end_inflate is called.
   *  The stream must not be in deflating state yet.
   */
  void begin_deflate ();

  /**
   *  @brief Disable inline compression (see \begin_deflate)
   *
   *  Warning: if end_deflate is not called on the stream before it is closed,
   *  data may be missing at the end of the stream.
   *
   *  @return A pair of the uncompressed and compressed byte count collected since begin_deflate
   */
  std::pair<size_t, size_t> end_deflate ();

  /** 
   *  @brief This is the outer write method to call
   *  
   *  This implementation writes data through the 
   *  protected write call.
   */
  void put (const char *b, size_t n);

  /**
   *  @brief Puts a C string (UTF-8) to the output
   */
  void put (const char *s)
  {
    put (s, strlen (s));
  }

  /**
   *  @brief Puts a STL string (UTF-8) to the output
   */
  void put (const std::string &s)
  {
    put (s.c_str (), s.size ());
  }

  /**
   *  @brief << operator
   */
  OutputStream &operator<< (char s)
  {
    put (&s, 1);
    return *this;
  }

  /**
   *  @brief << operator
   */
  OutputStream &operator<< (unsigned char s)
  {
    put ((const char *) &s, 1);
    return *this;
  }

  /**
   *  @brief << operator
   */
  OutputStream &operator<< (const char *s)
  {
    put (s);
    return *this;
  }

  /**
   *  @brief << operator
   */
  OutputStream &operator<< (const std::string &s)
  {
    put (s);
    return *this;
  }

  /**
   *  @brief << operator
   */
  template <class T>
  OutputStream &operator<< (const T &t)
  {
    put (tl::to_string (t));
    return *this;
  }

  /**
   *  @brief Returns a value indicating whether that stream supports seek
   */
  bool supports_seek () const
  {
    return mp_delegate->supports_seek ();
  }

  /**
   *  @brief Seek to the specified position 
   *
   *  Writing continues at that position after a seek.
   *  Seek is not supported while in deflate mode.
   */
  void seek (size_t s);

  /**
   *  @brief Obtain the current file position
   */
  size_t pos () const 
  {
    return m_pos;
  }
    
protected:
  void reset_pos ()
  {
    m_pos = 0;
  }

private:
  size_t m_pos;
  OutputStreamBase *mp_delegate;

  //  deflate support 
  DeflateFilter *mp_deflate;

  //  No copying currently
  OutputStream (const OutputStream &);
  OutputStream &operator= (const OutputStream &);
};


class KLAYOUT_DLL FileOpenErrorException
  : public tl::Exception
{
public:
  FileOpenErrorException (const std::string &f, int en)
    : tl::Exception (tl::translate ("Unable to open file: %s (errno=%d)"), f, en)
  { }
};

class KLAYOUT_DLL FileWriteErrorException
  : public tl::Exception
{
public:
  FileWriteErrorException (const std::string &f, int en)
    : tl::Exception (tl::translate ("Write error on file: %s (errno=%d)"), f, en)
  { }
};

class KLAYOUT_DLL FileReadErrorException
  : public tl::Exception
{
public:
  FileReadErrorException (const std::string &f, int en)
    : tl::Exception (tl::translate ("Read error on file: %s (errno=%d)"), f, en)
  { }
};

class KLAYOUT_DLL ZLibWriteErrorException
  : public tl::Exception
{
public:
  ZLibWriteErrorException (const std::string &f, const char *em)
    : tl::Exception (tl::translate ("Write error on file in decompression library: %s (message=%s)"), f, em)
  { }
};

class KLAYOUT_DLL ZLibReadErrorException
  : public tl::Exception
{
public:
  ZLibReadErrorException (const std::string &f, const char *em)
    : tl::Exception (tl::translate ("Read error on file in decompression library: %s (message=%s)"), f, em)
  { }
};

/**
 *  @brief An in-memory input file delegate
 */
class KLAYOUT_DLL InputMemoryStream
  : public InputStreamBase
{
public:
  /**
   *  @brief Create a stream reading from the given memory block
   *
   *  @param data The memory block where to read from
   *  @param length The length of the block
   */
  InputMemoryStream (const char *data, size_t length)
    : mp_data (data), m_length (length), m_pos (0)
  {
    //  .. nothing yet ..
  }

  /**
   *  @brief Read from the stream
   */
  virtual size_t read (char *b, size_t n)
  {
    if (m_pos + n > m_length) {
      n = m_length - m_pos;
    }
    memcpy (b, mp_data + m_pos, n);
    m_pos += n;
    return n;
  }

  /**
   *  @brief Reset to the beginning of the file
   */
  virtual void reset ()
  {
    m_pos = 0;
  }

  /**
   *  @brief Get the source specification (the file name)
   *
   *  Returns an empty string if no file name is available.
   */
  virtual std::string source () 
  {
    return "data";
  }

private:
  const char *mp_data;
  size_t m_length, m_pos;
};

/**
 *  @brief A string output delegate
 *
 *  Implements the writer to a string
 */
class KLAYOUT_DLL OutputStringStream
  : public OutputStreamBase
{
public:
  /**
   *  @brief Create a string writer
   */
  OutputStringStream ()
  {
    m_stream.imbue (std::locale ("C"));
  }

  /**
   *  @brief Write to a string 
   *
   *  Implements the basic write method. 
   */
  virtual void write (const char *b, size_t n)
  {
    m_stream.write (b, n);
  }

  /**
   *  @brief Seek to the specified position 
   *
   *  Writing continues at that position after a seek.
   */
  void seek (size_t s)
  {
    m_stream.seekp (s);
  }

  /**
   *  @brief Returns a value indicating whether that stream supports seek
   */
  bool supports_seek () 
  {
    return true;
  }

  /**
   *  @brief Get the content as a STL string
   *
   *  This method will return an char pointer containing the data written. 
   */
  std::string string () 
  {
    return m_stream.str ();
  }

private:
  std::ostringstream m_stream;
};

/**
 *  @brief A zlib input file delegate
 *
 *  Implements the reader for a zlib stream
 */
class KLAYOUT_DLL InputZLibFile
  : public InputStreamBase
{
public:
  /**
   *  @brief Open a file with the given path
   *
   *  Opening a file is a prerequisite for reading from the 
   *  object. open() will throw a FileOpenErrorException if 
   *  an error occurs.
   *
   *  @param path The (relative) path of the file to open
   */
  InputZLibFile (const std::string &path);

  /**
   *  @brief Close the file
   *
   *  The destructor will automatically close the file.
   */
  virtual ~InputZLibFile ();

  /**
   *  @brief Read from a file 
   *
   *  Implements the basic read method. 
   *  Will throw a ZLibReadErrorException if an error occurs.
   */
  virtual size_t read (char *b, size_t n);

  /**
   *  @brief Reset to the beginning of the file
   */
  virtual void reset ();

  /**
   *  @brief Get the source specification (the file name)
   *
   *  Returns an empty string if no file name is available.
   */
  virtual std::string source () 
  {
    return m_source;
  }

private:
  std::string m_source;
  gzFile m_zs;
};

/**
 *  @brief A zlib output file delegate
 *
 *  Implements the writer for a zlib stream
 */
class KLAYOUT_DLL OutputZLibFile
  : public OutputStreamBase
{
public:
  /**
   *  @brief Open a file with the given path
   *
   *  Opening a file is a prerequisite for reading from the 
   *  object. open() will throw a FileOpenErrorException if 
   *  an error occurs.
   *
   *  @param path The (relative) path of the file to open
   */
  OutputZLibFile (const std::string &path);

  /**
   *  @brief Close the file
   *
   *  The destructor will automatically close the file.
   */
  virtual ~OutputZLibFile ();

  /**
   *  @brief Write to a file 
   *
   *  Implements the basic write method. 
   *  Will throw a ZLibWriteErrorException if an error occurs.
   */
  virtual void write (const char *b, size_t n);

private:
  std::string m_source;
  gzFile m_zs;
};

/**
 *  @brief A simple input file delegate
 *
 *  Implements the reader for ordinary files.
 */
class KLAYOUT_DLL InputFile
  : public InputStreamBase
{
public:
  /**
   *  @brief Open a file with the given path
   *
   *  Opening a file is a prerequisite for reading from the 
   *  object. open() will throw a FileOpenErrorException if 
   *  an error occurs.
   *
   *  @param path The (relative) path of the file to open
   *  @param read True, if the file should be read, false on write.
   */
  InputFile (const std::string &path);

  /**
   *  @brief Close the file
   *
   *  The destructor will automatically close the file.
   */
  virtual ~InputFile ();

  /**
   *  @brief Read from a file 
   *
   *  Implements the basic read method. 
   *  Will throw a FileReadErrorException if an error occurs.
   */
  virtual size_t read (char *b, size_t n);

  /**
   *  @brief Reset to the beginning of the file
   */
  virtual void reset ();

  /**
   *  @brief Get the source specification (the file name)
   *
   *  Returns an empty string if no file name is available.
   */
  virtual std::string source () 
  {
    return m_source;
  }

private:
  std::string m_source;
  FILE *m_file;
};

/**
 *  @brief A simple output file delegate
 *
 *  Implements the writer for ordinary files.
 */
class KLAYOUT_DLL OutputFile
  : public OutputStreamBase
{
public:
  /**
   *  @brief Open a file with the given path
   *
   *  Opening a file is a prerequisite for reading from the 
   *  object. open() will throw a FileOpenErrorException if 
   *  an error occurs.
   *
   *  @param path The (relative) path of the file to open
   *  @param read True, if the file should be read, false on write.
   */
  OutputFile (const std::string &path);

  /**
   *  @brief Close the file
   *
   *  The destructor will automatically close the file.
   */
  virtual ~OutputFile ();

  /**
   *  @brief Seek to the specified position 
   *
   *  Writing continues at that position after a seek.
   */
  virtual void seek (size_t s);

  /**
   *  @brief Returns a value indicating whether that stream supports seek
   */
  bool supports_seek () 
  {
    return true;
  }

  /**
   *  @brief Write to a file 
   *
   *  Implements the basic write method. 
   *  Will throw a FileWriteErrorException if an error occurs.
   */
  virtual void write (const char *b, size_t n);

private:
  std::string m_source;
  FILE *m_file;
};


class KLAYOUT_DLL FilePOpenErrorException
  : public tl::Exception
{
public:
  FilePOpenErrorException (const std::string &f, int en)
    : tl::Exception (tl::translate ("Unable to get input from command through pipe: %s (errno=%d)"), f, en)
  { }
};

class KLAYOUT_DLL FilePReadErrorException
  : public tl::Exception
{
public:
  FilePReadErrorException (const std::string &f, int en)
    : tl::Exception (tl::translate ("Read error on pipe from command: %s (errno=%d)"), f, en)
  { }
};

class KLAYOUT_DLL FilePWriteErrorException
  : public tl::Exception
{
public:
  FilePWriteErrorException (const std::string &f, int en)
    : tl::Exception (tl::translate ("Write error on pipe from command: %s (errno=%d)"), f, en)
  { }
};

/**
 *  @brief A simple pipe input delegate
 *
 *  Implements the reader for pipe streams
 */
class KLAYOUT_DLL InputPipe
  : public InputStreamBase
{
public:
  /**
   *  @brief Open a stream by connecting with the stdout of a given command
   *
   *  Opening a pipe is a prerequisite for reading from the 
   *  object. open() will throw a FilePOpenErrorException if 
   *  an error occurs - commonly if the command cannot be executed.
   *  This implementation is based on popen ().
   *
   *  @param cmd The command to execute
   *  @param read True, if the file should be read, false on write.
   */
  InputPipe (const std::string &path);

  /**
   *  @brief Close the pipe
   *
   *  The destructor will automatically close the pipe.
   */
  virtual ~InputPipe ();

  /**
   *  @brief Read from the pipe 
   *
   *  Implements the basic read method. 
   *  Will throw a FilePReadErrorException if an error occurs.
   */
  virtual size_t read (char *b, size_t n);

  /**
   *  @brief Reset to the beginning of the file
   */
  virtual void reset ();

  /**
   *  @brief Get the source specification (the file name)
   *
   *  Returns an empty string if no file name is available.
   */
  virtual std::string source () 
  {
    //  No source (in the sense of a file name) is available ..
    return std::string ();
  }

private:
  FILE *m_file;
  std::string m_source;
};

/**
 *  @brief A simple pipe output delegate
 *
 *  Implements the writer for pipe streams
 */
class KLAYOUT_DLL OutputPipe
  : public OutputStreamBase
{
public:
  /**
   *  @brief Open a stream by connecting with the stdout of a given command
   *
   *  Opening a pipe is a prerequisite for reading from the 
   *  object. open() will throw a FilePOpenErrorException if 
   *  an error occurs - commonly if the command cannot be executed.
   *  This implementation is based on popen ().
   *
   *  @param cmd The command to execute
   *  @param read True, if the file should be read, false on write.
   */
  OutputPipe (const std::string &path);

  /**
   *  @brief Close the pipe
   *
   *  The destructor will automatically close the pipe.
   */
  virtual ~OutputPipe ();

  /**
   *  @brief Write to a file 
   *
   *  Implements the basic write method. 
   *  Will throw a FilePWriteErrorException if an error occurs.
   */
  virtual void write (const char *b, size_t n);

private:
  FILE *m_file;
  std::string m_source;
};

/**
 *  @brief Utility: read a whole file into a string
 */
void KLAYOUT_DLL read_file (tl::InputStreamBase &stream_base, std::string &into);

}

#endif

