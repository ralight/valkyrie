/****************************************************************************
** Various utility functions
** --------------------------------------------------------------------------
**
** Copyright (C) 2000-2010, OpenWorks LLP. All rights reserved.
** <info@open-works.co.uk>
**
** This file is part of Valkyrie, a front-end for Valgrind.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "vk_utils.h"
#include "options/vk_option.h"
#include "utils/vk_config.h"        // vkname()

#include <cstdlib>                  // exit, mkstemp, free/malloc, etc

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QString>

/*
#include "config.h"                 // PACKAGE_BUGREPORT

#include <stdlib.h>                 // mkstemp()
#include <stdarg.h>                 // va_start, va_end
#include <sys/types.h>              // getpid
#include <unistd.h>                 // getpid
#include <pwd.h>                    // getpwuid

#include <qapplication.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qregexp.h>
*/


/* prints various info msgs to stdout --------------------------------- */
void vkPrint( const char* msg, ... )
{
   QByteArray ba = vkConfig->vkName.toLatin1();
   const char* vkname = vkConfig ? ba.constData() : "";
   va_list ap;
   va_start( ap, msg );
   va_end( ap );
   fprintf( stdout, "===%s:%d=== ", vkname, ( int )getpid() );
   vfprintf( stdout, msg, ap );
   va_end( ap );
   fprintf( stdout, "\n" );
   fflush( stdout );
}


/* prints error msg -------------------------------------------------- */
void vkPrintErr( const char* msg, ... )
{
   QByteArray ba = vkConfig->vkName.toLatin1();
   const char* vkname = vkConfig ? ba.constData() : "";
   va_list ap;
   va_start( ap, msg );
   va_end( ap );
   fprintf( stderr, "===%s:%d=== ", vkname, ( int )getpid() );
   vfprintf( stderr, msg, ap );
   va_end( ap );
   fprintf( stderr, "\n" );
   fflush( stderr );
}


/* prints debug msg -------------------------------------------------- */
void vkDebug( const char* msg, ... )
{
#ifdef DEBUG_ON
   QByteArray ba = vkConfig->vkName.toLatin1();
   const char* vkname = vkConfig ? ba.constData() : "";
   va_list ap;
   va_start( ap, msg );
   va_end( ap );
   fprintf( stderr, "===%s:%d=== ", vkname, ( int )getpid() );
   vfprintf( stderr, msg, ap );
   va_end( ap );
   fprintf( stderr, "\n" );
#endif
}

/* prints an "Assertion failed" message and exits ---------------------- */
__attribute__(( noreturn ) )
void vk_assert_fail( const char* expr, const char* file,
                     unsigned int line, const char* fn )
{
   vkPrintErr( "Assertion failed '%s':", expr );
   vkPrintErr( "   at %s#%u:%s\n", file, line, fn );
   exit( 1 );
}

/* prints a message asking user to email a bug report,
 * and then exits. ----------------------------------------------------- */
__attribute__(( noreturn ) )
void vk_assert_never_reached_fail( const char* file,
                                   unsigned int line,
                                   const char* fn )
{
   QByteArray ba = vkConfig->vkName.toLatin1();
   const char* vkname = vkConfig ? ba.constData() : "";
   
   vkPrintErr( "Assertion 'never reached' failed," );
   vkPrintErr( "   at %s#%u:%s", file, line, fn );
   vkPrintErr( "%s version: %s", vkname, PACKAGE_VERSION );
   vkPrintErr( "Built with QT version:   %s", QT_VERSION_STR );
   vkPrintErr( "Running with QT version: %s", qVersion() );
   vkPrintErr( "Hopefully, you should never see this message." );
   vkPrintErr( "If you are, then Something Really Bad just happened." );
   vkPrintErr( "Please report this bug to: %s", PACKAGE_BUGREPORT );
   vkPrintErr( "In the bug report, please send the the above text," );
   vkPrintErr( "along with the output of `uname -a`." );
   vkPrintErr( "Thanks.\n" );
   exit( 1 );
}


/* Create a unique filename, with an optional extension ---------------- */
QString vk_mkstemp( QString filepath, QString ext/*=QString::null*/ )
{
   // create tempfiles with datetime, so can sort easily if they stay around
   
   QString datetime = QDateTime::currentDateTime().toString( "_yyyy.MM.dd_hh:mm:ss" );
   QString unique = filepath + datetime;
   
   if ( !ext.isNull() ) {
      unique +=  "." + ext;
   }
   
   if ( QFile::exists( unique ) ) {
      /* fall back on mkstemp */
      char* tmpname = vk_str_malloc( unique.length() + 10 );
      sprintf( tmpname, "%s.XXXXXX", qPrintable( unique ) );
      int fd = mkstemp( tmpname );
      
      if ( fd == -1 ) {
         /* something went wrong */
         VK_DEBUG( "failed to create unique filename from '%s'.",
                   qPrintable( filepath ) );
         return QString::null;
      }
      
      unique = QString( tmpname );
      tmpname = vk_str_free( tmpname );
   }
   
   return unique;
}


/* Version check -------------------------------------------------------
   Given version string of "major.minor.patch" (e.g. 3.3.0),
   hex version = (major << 16) + (minor << 8) + patch
*/
int strVersion2hex( QString ver_str )
{
   QRegExp rxver( ".*(\\d{1,2})\\.(\\d{1,2})\\.(\\d{1,2}).*" );
   
   if ( rxver.indexIn( ver_str ) == -1 ) {
      return -1;
   }
   
   int major = rxver.cap( 1 ).toInt();
   int minor = rxver.cap( 2 ).toInt();
   int patch = rxver.cap( 3 ).toInt();
   return ( major << 16 ) + ( minor << 8 ) + patch;
}



/* escape html entities
   current list: '<', '>', '&' ----------------------------------------- */
QString escapeEntities( const QString& content )
{
   QString ret_str = "";
   
   for ( int i = 0; i < content.length(); i++ ) {
      switch ( content[i].toLatin1() ) {
      case '<':
         ret_str += "&lt;";
         break;
      case '>':
         ret_str += "&gt;";
         break;
      case '&': {
         /* already escaped? */
         if (( content.mid( i + 1, 4 ) == "amp;" ) ||
             ( content.mid( i + 1, 3 ) == "lt;" ) ||
             ( content.mid( i + 1, 3 ) == "gt;" ) ) {
            ret_str += content[i];
         }
         else {
            ret_str += "&amp;";
         }
      }
      break;
      default:
         ret_str += content[i];
         break;
      }
   }
   
   return ret_str;
}


/* swap '\n' for <br> */
QString str2html( QString str )
{
   str.replace( '\n', "<br>" );
   return str;
}


/* wrappers for various fns -------------------------------------------- */

/* wrappers to free(3)
   hides const compilation noise, permit NULL, return NULL always. */
void* vk_free( const void* ptr )
{
   if ( ptr != NULL ) {
      free(( void* )ptr );
      ptr = NULL;
   }
   
   return NULL;
}

char* vk_str_free( const char* ptr )
{
   if ( ptr != NULL ) {
      free(( char* )ptr );
   }
   
   return NULL;
}

void* vk_malloc( unsigned long n_bytes )
{
   void* mem;
   mem = malloc( n_bytes );
   
   if ( !mem ) {
      VK_DEBUG( "failed to allocate %lu bytes", n_bytes );
   }
   
   return mem;
}


char* vk_str_malloc( int sz )
{
   char* arr;
   arr = ( char* ) malloc(( size_t )(( sz + 2 ) * sizeof( char ) ) );
   
   if ( !arr ) {
      VK_DEBUG( "malloc failure: virtual memory exhausted" );
      vk_assert_never_reached();
   }
   
   return arr;
}


/* wrapper to strcmp(): returns true || false */
bool vk_strcmp( const char* str1, const char* str2 )
{
   if ( !str1 || !str2 ) {
      VK_DEBUG( "can't call strcmp on null strings:\n"
                "str1 == %s, str2 == %s\n", str1, str2 );
      return false;
   }
   
   if (( strlen( str1 ) == 0 ) || ( strlen( str2 ) == 0 ) ) {
      VK_DEBUG( "one of these two strings is empty:\n"
                "\tstr1: -->%s<--, str2: -->%s<--\n", str1, str2 );
      return false;
   }
   
   return ( strcmp( str1, str2 ) == 0 ) ? true : false;
}


char* vk_strdup( const char* str )
{
   char* new_str;
   unsigned int length;
   
   if ( str ) {
      length = strlen( str ) + 1;
      new_str = vk_str_malloc( length );
      strcpy( new_str, str );
   }
   else {
      new_str = NULL;
   }
   
   return new_str;
}






//***************************************************************************
// helper functions
//***************************************************************************

/*!
  Local helper function to find file_name: either directly or via $PATH
  Returns absolute path to file_name.
*/
static QString getFileAbsPath( const QString file_name )
{
   QString absPath = QString::null;

   if ( QFile::exists( file_name ) ) {
      // file_name exists: get its absolute path.
      absPath = QFileInfo( file_name ).absoluteFilePath();
   }
   else if ( !file_name.contains('/') ) {
      // no '/' in file_name -> try $PATH env variable
      QString env = getenv( "PATH" );
      QStringList paths = env.split( ':' );

      foreach ( QString candidate, paths ) {
         candidate += "/" + file_name;
         if ( QFile::exists( candidate ) ) {
            // found it!  set its absolute path.
            absPath = QFileInfo( candidate ).absoluteFilePath();
            break;
         }
      }
   }

   return absPath;
}


/*!
  Checks file exists and has correct permissions.
  Returns absolute path to file, if it exists: else QString::null
*/
QString fileCheck( int* err_val, const QString fpath,
                   bool check_read/*=false*/,
                   bool check_write/*=false*/,
                   bool check_exe/*=false*/ )
{
   *err_val = PARSED_OK;
   QString absPath = QString::null;
   QFileInfo fi;

   // check exists: if so, return absolute path
   if ( fpath.isEmpty() ) {
      *err_val = PERROR_BADFILE;
      goto bye;
   }

   // try to find fpath: directly or via $PATH
   absPath = getFileAbsPath( fpath );
   if ( absPath.isNull() ) {
      // couldn't find fpath :-(
      *err_val = PERROR_BADFILE;
      goto bye;
   }

   fi.setFile( absPath );

   // check this is really a file
   if ( !fi.isFile() ) {
      *err_val = PERROR_BADFILE;
      goto bye;
   }

   // check for read permissions
   if ( check_read && !fi.isReadable() ) {
      *err_val = PERROR_BADFILERD;
      goto bye;
   }

   // check for write permissions
   if ( check_write && !fi.isWritable() ) {
      *err_val = PERROR_BADFILEWR;
      goto bye;
   }

   // check for executable permissions
   if ( check_exe && !fi.isExecutable() ) {
      *err_val = PERROR_BADEXEC;
      goto bye;
   }

bye:
   return absPath;
}


/*!
  Checks dir exists and has correct permissions.
  Returns absolute path to dir, if it exists: else QString::null
*/
QString dirCheck( int* err_val, const QString dir,
                  bool check_read/*=false*/,
                  bool check_write/*=false*/,
                  bool check_exe/*=false*/ )
{
   *err_val = PARSED_OK;
   QString absPath = QString::null;
   QFileInfo fi( dir );

   // check exists: if so, return absolute path
   if ( dir.isEmpty() || !fi.exists() || !fi.isDir() ) {
      *err_val = PERROR_BADDIR;
      goto bye;
   }

   // dir exists: set its absolute path
   absPath = fi.absolutePath();

   // check for read permissions
   if ( check_read && !fi.isReadable() ) {
      *err_val = PERROR_BADFILERD;
      goto bye;
   }

   // check for write permissions
   if ( check_write && !fi.isWritable() ) {
      *err_val = PERROR_BADFILEWR;
      goto bye;
   }

   // check for executable permissions
   if ( check_exe && !fi.isExecutable() ) {
      *err_val = PERROR_BADEXEC;
      goto bye;
   }

bye:
   return absPath;
}
