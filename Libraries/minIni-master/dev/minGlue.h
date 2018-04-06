/*  Glue functions for the minIni library, based on the C/C++ stdio library
 *
 *  Or better said: this file contains macros that maps the function interface
 *  used by minIni to the standard C/C++ file I/O functions.
 *
 *  By CompuPhase, 2008-2014
 *  This "glue file" is in the public domain. It is distributed without
 *  warranties or conditions of any kind, either express or implied.
 */

/* map required file I/O types and functions to the standard C library */
#include <stdio.h>
#include "SD.h"

#define INI_BUFFERSIZE  128       /* maximum line length, maximum path length */

//#define strnicmp  strncasecmp
#define PORTABLE_STRNICMP

#define INI_FILETYPE                    File
#define ini_openread(filename,file)     ((*(file) = SD.open(filename, FILE_READ))	)
#define ini_openwrite(filename,file)    ((*(file) = SD.open(filename, FILE_WRITE))	)
#define ini_openrewrite(filename,file)  ((*(file) = SD.open(filename, FILE_WRITE))	)
#define ini_close(file)                 ((*(file)).close())
//#define ini_read(buffer,size,file)      (((*(file)).read(buffer, size)) > 0)
#define ini_read(buffer,size,file)      (((*(file)).readBytesUntil('\n',buffer, size)) > 0)

//#define ini_write(buffer,file)          (((*(file)).write(buffer, strlen(buffer))) >= 0)
#define ini_write(buffer,file)          (((*(file)).write(buffer, strlen(buffer))) >= 0)
#define ini_rename(source,dest)         (0)
#define ini_remove(filename)            (SD.remove(filename))

#define INI_FILEPOS                     unsigned long
#define ini_tell(file,pos)              (*pos = (*(file)).position())
#define ini_seek(file,pos)              ((*(file)).seek(*pos) == 1)

/* for floating-point support, define additional types and functions */
#define INI_REAL                        float
#define ini_ftoa(string,value)          sprintf((string),"%f",(value))
#define ini_atof(string)                (INI_REAL)strtod((string),NULL)

