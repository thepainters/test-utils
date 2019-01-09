
 /**
  * @file textutilstream.h
  * @brief utility stream functions
  * @author thepainters@gmail.com
  */

#ifndef __TEXTUTILSTREAM_INCLUDED
#define __TEXTUTILSTREAM_INCLUDED
#include <stdio.h>
#include <time.h>
/**
 * Used by Textutils to format output data
 */
typedef enum {
  /** Base string `name = value` */
    STRING,
  /** TCL data `name value` */
    TCL,
  /** Bourne Shell data `name=value` */
    SH,
  /** Powershell data `$name=value` */
    PS,
  /** BAT `set name=value` */
    BAT,
  /** Perl Data `$name=value;` */
    PERL,
  /** Java script object notation `$name=value;` */
    JSON,
  /** XML data: `<item name="value"/>` */
    XML,
  /** CSV */
    CSV
} OutputType;

typedef enum {
    UNSET = 0,
    HASH = 1,
    ARRAY = 2,
} StructType;


typedef struct structuredOutputStream {
  FILE *output;
  char *include_these;
  char *exclude_these;
  OutputType otype;
  struct structuredOutputStream *parent;
  int count;
  int level;
  int type;
  int buffered;
  char *stream;
} TextUtilStream;

TextUtilStream *newTextUtilStream(FILE *, OutputType );
void includeThis(TextUtilStream *, char *);
void excludeThis(TextUtilStream *, char *);
TextUtilStream* createList(TextUtilStream *, char *);
TextUtilStream* createObject(TextUtilStream *, char *);
void addNumber(TextUtilStream *, char *, int );
void addLong(TextUtilStream *, char *, long );
void addString(TextUtilStream *, char *, char *);
void addHexString(TextUtilStream *, char *, unsigned char *);
void addTimestamp(TextUtilStream *, char *, time_t);
void destroy(TextUtilStream *);
void hideNumber(TextUtilStream *, char *, int );
void hideString(TextUtilStream *, char *, char * );
int filteredOut(TextUtilStream *, char *);
#endif

