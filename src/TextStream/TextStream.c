#ifndef __TEXTUTILSTREAM_INCLUDED
#include "TextStream.h"
#endif
#define OSSTRLEN strlen
#define OSSTROUT snprintf
#endif
#include <stdarg.h>

 /**
  * @file textutilstream.c
  * @brief utility stream library
  * @author thepainters@gmail.com
  */

/**
 * @file textstream.c
 * @brief Output stream utilities for different output types
 * Supports the following output types:
 *   * XML
 *   * Tcl
 *   * JSON
 *   * Perl
 * ## Example
 * @code
    OutputType oType = XML;
    TextUtilStream *toplevel;
    TextUtilStream *outer;
    TextUtilStream *inner1, *inner2, *users, *user;
       toplevel = newTextUtilStream(stdout, oType);
    outer = createList(toplevel, "");
    inner1 = createObject(outer, "");
    addString(inner1, "user", "benp");
    addString(inner1, "date", "2001.020");
    addNumber(inner1, "systems", 1);
    users = createList(inner1, "users");
    user = createObject(users, "user");
    addString(inner1, "date", "2001.020");
    addNumber(inner1, "systems", 1);
    destroy(user);
    addString(inner1, "onemore", NULL);
    destroy(users);
    destroy(inner1);
    inner1 = createObject(outer, "");
    addNumber(inner1, "available", 121);
    addNumber(inner1, "queued", 0);
    addString(inner1, "serial", "101034");
    addString(inner1, "issuer", "isaadmin");
    addNumber(inner1, "dup_group", 0);
    addString(inner1, "hostid", NULL);
    users = createList(inner1, "users");
    user = createObject(users, "user");
    destroy(user);
    destroy(users);
    destroy(inner1);

    destroy(outer);
    destroy(toplevel);
 * @endcode
 */


void loaddata(TextUtilStream *in, char *fmt, ...) {
  va_list remaining;
  va_start(remaining,fmt);
  if (in->buffered) {
      char buffer[4096];
      memset(&buffer, 0, 4096);
      vsprintf(buffer, fmt, remaining);
      str_append(in->stream, buffer);
  } else {
      vfprintf(in->output, fmt, remaining);
  }
  va_end(remaining);
}

void loadsmalldata(TextUtilStream *in, char *rest) {
    loaddata(in, "%s", rest);
}
/*
 * this routine prints the indentation
 */
void printSpaces(TextUtilStream *that, int howmany) {
    int i = 0;
    if (that == NULL) return;
    if (that->otype == TCL) return;
    if (that->otype == CSV) return;
    for (i =0; i <=howmany; i++) loadsmalldata(that, " ");
}
/*
 * that routine completes the prior line, either adding
 * a newline or a comma with a newline.
 */
void finishPriorLine(TextUtilStream *that, int newobject) {
    int needscomma = 0;

    if (that == NULL) {
        return;
    }
    if (that->level == 0) return;
    if (that->count > 0) needscomma = 1;
    else needscomma = 0;
    if (that->parent != NULL) {
        if (that->parent->type == HASH) {
            switch (that->otype) {
                case TCL:
                   if (that->count >0) loadsmalldata(that, " "); /*removing this line will break the tests, but this is unnecessary. */
                   break;
                case CSV:
                   break;
                case JSON:
                case PERL:
                   if (needscomma) loadsmalldata(that, ",\n");
                   else  loadsmalldata(that, "\n");
                   break;
                default:
                   loadsmalldata(that, "\n");
                   break;
            }
        }
        else if (that->parent->type == ARRAY) {
            switch (that->otype) {
                case CSV:
                   break;
                case TCL:
                   if (that->count > 0) loadsmalldata(that, " ");
                   break;
                case JSON:
                case PERL:
                   if (needscomma) loadsmalldata(that, ",\n");
                   else  loadsmalldata(that, "\n");
                   break;
                default:
                   loadsmalldata(that, "\n");
                   break;
            }
        }
        else {
            switch (that->otype) {
                case CSV:
                   if (that->count > 0) loadsmalldata(that, " ");
                   break;
                case TCL:
                   if (that->count > 0) loadsmalldata(that, " ");
                   break;
                case JSON:
                case PERL:
                   if (needscomma) loadsmalldata(that, ",\n");
                   else  loadsmalldata(that, "\n");
                   break;
                default:
                   loadsmalldata(that, "\n");
                   break;
            }
        }
    } 
}

/**
 * creates a new generic TextUtilStream
 */
TextUtilStream *_createTextUtilStream(FILE *output, OutputType otype, int buffered) {
    TextUtilStream *that = NULL;
    that = (TextUtilStream *) mobjalloc(sizeof(TextUtilStream));
    that->parent = (TextUtilStream *) NULL;
    that->output = output;
    that->otype = otype;
    that->type = 0;
    that->level = 0;
    that->count = 0;
    that->include_these = NULL;
    that->exclude_these = NULL;
    that->buffered = buffered;
    that->stream = (char *) NULL;
    return that;
}
TextUtilStream *newBufferedTextUtilStream(FILE *output, OutputType otype) {
    return _createTextUtilStream(output, otype, 1);
}
/**
 * creates a new generic TextUtilStream
 */
TextUtilStream *newTextUtilStream(FILE *output, OutputType otype) {
    return _createTextUtilStream(output, otype, 0);
}

int findNumberOfParents(TextUtilStream *what) {
    int count = 0;
    TextUtilStream *tmp = what;
    while ((tmp = tmp->parent) != (TextUtilStream *) NULL) {
        count++;
    }
    return count;
}
void initList( TextUtilStream *list, char *name) {
    switch (list->otype) {
        case TCL:
            if (strlen(name) > 0) { loaddata(list,"%s %c", NSTR(name), 123);}
            else  { loaddata(list,"%c", 123);}
            break;
        case CSV:
            loadsmalldata(list,"\n");
            break;
        case JSON:
            if (strlen(name) > 0) { loaddata(list,"\"%s\": [", NSTR(name));}
            else {loadsmalldata(list,"[");}
            break;
        case XML:
            if (strlen(name) > 0) { loaddata(list,"<list name=\"%s\">", NSTR(name));}
            else {loadsmalldata(list,"<list>");}
            break;
        case PERL:
            if (strlen(name) > 0) { loaddata(list,"'%s' => [", NSTR(name));}
            else {loadsmalldata(list,"[");}
            break;
        default:
            break;
    }
}


void initObject( TextUtilStream *obj, char *name) {
    switch (obj->otype) {
        case TCL:
            loaddata(obj, "%c", 123);
            break;
            /*
        case JSON:
            if (strlen(name) > 0) loaddata(obj,"\"%s\": %c", NSTR(name), 123);
            else loaddata(obj,"%c", 123);
            break;
            */
        case XML:
            loaddata(obj,"<object name=\"%s\">", NSTR(name));
            break;
        case JSON:
        case PERL:
            loaddata(obj, "%c", 123);
            break;
        default:
            break;
    }
}
TextUtilStream *createExpandedType(TextUtilStream *what, char *name, StructType type) {
    TextUtilStream *expandedtype = (TextUtilStream*) NULL;
    char tmpname[1024];
    if (what == NULL) return expandedtype;
    expandedtype = (TextUtilStream *) mobjalloc(sizeof(TextUtilStream));
    expandedtype->parent = what;
    expandedtype->buffered = what->buffered;
    expandedtype->stream = NULL;
    expandedtype->otype = what->otype;
    expandedtype->output = what->output;
    expandedtype->level = (expandedtype->parent->level + 1);
    expandedtype->count = 0;
    expandedtype->type = type;
    expandedtype->include_these = NULL;
    expandedtype->exclude_these = NULL;
    if (expandedtype->parent->include_these != NULL) {
        mstrdup((expandedtype->include_these),(expandedtype->parent->include_these));
    } else {
        expandedtype->include_these = NULL;
    }
    if (expandedtype->parent->exclude_these != NULL) {
        mstrdup((expandedtype->exclude_these),(expandedtype->parent->exclude_these));
    } else {
        expandedtype->exclude_these = NULL;
    }



    if (expandedtype->type == XML) {
        if (expandedtype->parent->type == HASH) {
            if (strlen(name) <= 0) {
                sprintf(tmpname, "hash%d", expandedtype->parent->count);
                name = tmpname;
            }
        }
        if (expandedtype->parent->type == ARRAY) {
            if (strlen(name) <= 0) {
                sprintf(tmpname, "array%d", expandedtype->parent->count);
                name = tmpname;
            }
        }
    }
    switch (type) {
        case HASH: 
          initObject(expandedtype, name);
          break;
        case ARRAY: {
          initList(expandedtype, name);
          break;
        default: break;
        }
    }
    what->count++;
    return expandedtype;
}
/**
 * creates a new object TextUtilStream
 */
TextUtilStream *createObject(TextUtilStream *obj, char *name) {
  if (obj == NULL) return NULL;
  finishPriorLine(obj,1);
  printSpaces(obj, obj->level+1);
  return (TextUtilStream*) createExpandedType(obj, name, HASH);
}
/**
 * creates a new list TextUtilStream
 */
TextUtilStream *createList(TextUtilStream *obj, char *name) {
  if (obj == NULL) return NULL;
  finishPriorLine(obj,1);
  printSpaces(obj, obj->level+1);
  return (TextUtilStream*) createExpandedType(obj, name, ARRAY);
}
int filteredOut(TextUtilStream *obj, char *name) {
    int keepgoing = 1;
    int stopgoing = 0;
    int currentchoice = keepgoing;
    int included = 0;
    int excluded = 0;
    if (name == NULL) return 1;
    if (strlen(name) == 0) return 1;
    if (obj->include_these == NULL) included = 1;
    else {
       if (strstr(obj->include_these, "all") != NULL) included=1;
       if (strstr(obj->include_these, name) != NULL) included=1;
       /*
        if (included) {
            printf("\nincluding %s\n", name);
        } else {
            printf("\nnot including %s (%s)\n", name, obj->include_these);
        }
        */
    }
    if (obj->exclude_these == NULL) excluded = 0;
    else {
       char *tmp_name = NULL;
       str_append(tmp_name, ":");
       str_append(tmp_name, name);
       str_append(tmp_name, ":");
       if (strstr(obj->exclude_these, tmp_name) != NULL) included=0;
       /*
        if (excluded) {
            printf("\nexcluding %s\n", tmp_name);
        } else {
            printf("\nnot excluding %s (%s)\n", tmp_name, obj->exclude_these);
        }
        */
        mfree(tmp_name);
    }
    return (included);
}

void addToList(TextUtilStream *list, char *name, char *value) {
    if (list == NULL) return;
    if (list->parent == NULL) return;
    if (! filteredOut(list, name)) return;
    finishPriorLine(list,0);
    printSpaces(list, list->level+1);
    switch (list->otype) {
        case TCL:
            loaddata(list,"%c%s%c ", 123, NSTR(value), 125);
            break;
        case JSON:
            loaddata(list,"\"%s\"", NSTR(value));
            break;
        case XML:
            loaddata(list,"<item value=\"%s\"/>", NSTR(value));
            break;
        case CSV:
            loaddata(list,"%s,", NSTR(value));
            break;
        case PERL:
            loaddata(list,"'%s'", NSTR(value));
            break;
        default:
            loaddata(list,"%s", NSTR(value));
            break;
    }
    list->count++;
}
void addToObject(TextUtilStream *obj, char *name, char *value) {
    if (obj == NULL) {
        fprintf(stderr, "no this for object!\n");
        return;
    }
    if (obj->parent == NULL) {
        loadsmalldata(obj, "orphaned object element\n");
        return;
    }
    if (! filteredOut(obj, name)) {
        return;
    }
    finishPriorLine(obj,0);
    printSpaces(obj, obj->level+1);
    switch (obj->otype) {
        case TCL:
            loaddata(obj,"%s %c%s%c ", NSTR(name), 123, NSTR(value), 125);
            break;
        case JSON:
            loaddata(obj,"\"%s\": \"%s\"", NSTR(name), NSTR(value));
            break;
        case XML:
            loaddata(obj,"<item name=\"%s\" value=\"%s\"/>", NSTR(name), NSTR(value));
            break;
        case CSV:
            loaddata(obj,"%s,", NSTR(value));
            break;
        case PERL:
            loaddata(obj,"'%s' => '%s'", NSTR(name), NSTR(value));
            break;
        default:
            loaddata(obj,"%s = %s", NSTR(name), NSTR(value));
            break;
    }
    obj->count++;
}
/**
 * adds a name/value pair with a hexadecimal value to a parent TextUtilStream.
 */
void addHexString(TextUtilStream *what, char *name, unsigned char *value) {
    char *newvalue = NULL;
    char *ptr = NULL;
    unsigned int len = OSSTRLEN(value);
    unsigned int i = 0;
    mstralloc(newvalue, ((2*len) + 1));
    ptr = newvalue;
    for (i=0;i<len;i++) {
        OSSTROUT(ptr, 2*sizeof(char),  "%02X", value[i]);
        ptr+=2;
    } 
    newvalue[(2*len)+1] = '\0';

    if (what->parent == NULL) {
        loadsmalldata(what, "orphan");
        return;
    }
    if (what->type == ARRAY) addToList(what, name, newvalue);
    else if (what->type == HASH) addToObject(what, name, newvalue);
    else {fprintf(stderr, "ERROR, unknown parent type\n");}
    mfree(newvalue);
}
/**
 * adds a name/value pair with a string value to a parent TextUtilStream.
 */
void addString(TextUtilStream *what, char *name, char *value) {
    if (what->parent == NULL) {
        loadsmalldata(what, "orphan");
        return;
    }
    if (what->type == ARRAY) addToList(what, name, value);
    else if (what->type == HASH) addToObject(what, name, value);
    else {fprintf(stderr, "ERROR, unknown parent type\n");}
}
/**
 * adds a name/value pair with a time value to a parent TextUtilStream.
 */
void addTimestamp(TextUtilStream *what, char *name, time_t when) {
    char *value="";
    struct tm *timeptr = gmtime(&when);
    if (what->parent == NULL) {
        loadsmalldata(what, "orphan");
        return;
    }
    value = asctime(gmtime(&when));
    OSSTROUT(value, 65, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ", 
        1900+timeptr->tm_year, timeptr->tm_mon, timeptr->tm_mday,
        timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);
    if (what->type == ARRAY) addToList(what, name, value);
    else if (what->type == HASH) addToObject(what, name, value);
    else {fprintf(stderr, "ERROR, unknown parent type\n");}
}
/**
 * adds a name/value pair with a int value to a parent TextUtilStream.
 */
void addNumber(TextUtilStream *what, char *name, int number) {
    char value[66];
    memset(value, 0, sizeof(value));
    OSSTROUT(value, 65, "%d", number);
    if (what->parent == NULL) return;
    if (what->type == ARRAY) addToList(what, name, value);
    if (what->type == HASH) addToObject(what, name, value);
}

/**
 * adds a name/value pair with a int value to a parent TextUtilStream.
 */
void addLong(TextUtilStream *what, char *name, long number) {
    char value[66];
    memset(value, 0, sizeof(value));
    OSSTROUT(value, 65, "%ld", number);
    if (what->parent == NULL) return;
    if (what->type == ARRAY) addToList(what, name, value);
    if (what->type == HASH) addToObject(what, name, value);
}


/**
 * adds a name/value pair with a string value to a parent TextUtilStream.
 */
void hideString(TextUtilStream *what, char *name, char *value) {
    if (what->parent == NULL) {
        loadsmalldata(what, "orphan");
        return;
    }
    if (what->type == ARRAY) addToList(what, name, value);
    else if (what->type == HASH) addToObject(what, name, value);
    else {fprintf(stderr, "ERROR, unknown parent type\n");}
}
/**
 * adds a name/value pair with a int value to a parent TextUtilStream.
 */
void hideNumber(TextUtilStream *what, char *name, int number) {
    char value[66];
    memset(value, 0, sizeof(value));
    OSSTROUT(value, 65, "%d", number);
    if (what->parent == NULL) return;
    if (what->type == ARRAY) addToList(what, name, value);
    if (what->type == HASH) addToObject(what, name, value);
}



void destroyList(TextUtilStream *list) {
    switch (list->otype) {
        case TCL:
            loaddata(list,"%c", 125);
            break;
        case JSON:
    loadsmalldata(list,"\n");
    printSpaces(list, list->level);
            loadsmalldata(list,"]");
            break;
        case XML:
    loadsmalldata(list,"\n");
    printSpaces(list, list->level);
            loadsmalldata(list,"</list>");
            break;
        case CSV:
    printSpaces(list, list->level);
            break;
        case PERL:
    loadsmalldata(list,"\n");
    printSpaces(list, list->level);
            loadsmalldata(list,"]");
            break;
        default:
            break;
    }
}
void destroyObject(TextUtilStream *obj) {
    switch (obj->otype) {
        case TCL:
            loaddata(obj,"%c ", 125);
            break;
        case JSON:
            loadsmalldata(obj,"\n");
    printSpaces(obj, obj->level);
            loaddata(obj,"%c", 125);
            break;
        case XML:
            loadsmalldata(obj,"\n");
    printSpaces(obj, obj->level);
            loadsmalldata(obj,"</object>");
            break;
        case CSV:
            loadsmalldata(obj,"\n");
    printSpaces(obj, obj->level);
            break;
        case PERL:
            loadsmalldata(obj,"\n");
    printSpaces(obj, obj->level);
            loaddata(obj,"%c", 125);
            break;
        default:
            break;
    }

}
void destroy(TextUtilStream *obj) {
    TextUtilStream *parent = NULL;
    if (obj == NULL) return;
    if (obj->parent == NULL) {
        loadsmalldata(obj,"\n");
    }
    switch (obj->type) {
        case 0: break;
        case 1: {destroyObject(obj);break;}
        case 2: {destroyList(obj);break;}
    }
    if ((obj->buffered ==1) && (obj->stream !=NULL)) {
    char *ptr = obj->stream;
    fprintf(obj->output, "%s", ptr);
    mfree(obj->stream);
    obj->stream = (char *) NULL;
    }
    if (obj != NULL) {
      if ((obj->include_these) != NULL) {
        mfree((obj->include_these));
      }
      if ((obj->exclude_these) != NULL) {
        mfree((obj->exclude_these));
      }
    }
    mfree(obj);
}

void includeThis(TextUtilStream *stream, char *optarg) {
    char *addons = NULL;
    if (stream->include_these != NULL) {mstrdup(addons, (stream->include_these));}
    addons = str_add(NULL, ":");
    addons = str_add(&addons, optarg);
    addons = str_add(&addons, ":");
    mfree(stream->include_these);
    mstrdup((stream->include_these), addons);
    mfree(addons);
}
void excludeThis(TextUtilStream *stream, char *optarg) {
    char *addons = NULL;
    if (stream->exclude_these != NULL) {mstrdup(addons, (stream->exclude_these));}
    addons = str_add(&addons, ":");
    addons = str_add(&addons, optarg);
    addons = str_add(&addons, ":");
    mfree(stream->exclude_these);
    mstrdup((stream->exclude_these), addons);
    mfree(addons);
}

int testTextUtilStream(int argc, char *argv[]) {
    int i = 0;
    int j = 0;
    OutputType oType = XML;
    TextUtilStream *toplevel;
    TextUtilStream *outer;
    TextUtilStream *inner1, *inner2, *users, *user;


    for (i = 0; i <= 5; i++) {

        if (i == 0) oType = TCL;
        if (i == 1) oType = JSON;
        if (i == 2) oType = XML;
        if (i == 3) oType = PERL;
        if (i == 4) oType = STRING;
        if (i == 5) oType = CSV;


       toplevel = newTextUtilStream(stdout, oType);
        for (j = 1; j < argc; j++) {
            excludeThis(toplevel, argv[j]);
        }
printf ("\n____________________________________\n");

    outer = createList(toplevel, "");
    inner1 = createObject(outer, "");
    addString(inner1, "feature", "cvekernel");
    addString(inner1, "version", "2001.020");
    addNumber(inner1, "inuse", 10);
    addString(inner1, "expire", "01-jun-2029");
    addString(inner1, "daemon", "mgcld");
    addString(inner1, "vendor_def", "65f5f048");
    addNumber(inner1, "available", 121);
    addNumber(inner1, "queued", 0);
    addString(inner1, "serial", "101034");
    addString(inner1, "start", NULL);
    addString(inner1, "issuer", "isaadmin");
    addNumber(inner1, "dup_group", 0);
    addString(inner1, "hostid", NULL);
    users = createList(inner1, "users");
    user = createObject(users, "user");
    addString(user, "user", "Ben Painter");
    addNumber(user, "linger", 0);
    addString(user, "node", "orw-benp-m7");
    addNumber(user, "number", 10);
    addNumber(user, "uqueued", 1);
    addString(user, "eadate", "2011.020");
    addString(user, "seconds_in_use", "21.2");
    destroy(user);
    addString(inner1, "onemore", NULL);
    destroy(users);
    destroy(inner1);
    inner1 = createObject(outer, "");
    addString(inner1, "feature", "cvekernel");
    addString(inner1, "version", "2001.020");
    addNumber(inner1, "inuse", 10);
    addString(inner1, "expire", "01-jun-2029");
    addString(inner1, "daemon", "mgcld");
    addString(inner1, "vendor_def", "65f5f048");
    addNumber(inner1, "available", 121);
    addNumber(inner1, "queued", 0);
    addString(inner1, "serial", "101034");
    addString(inner1, "start", NULL);
    addString(inner1, "issuer", "isaadmin");
    addNumber(inner1, "dup_group", 0);
    addString(inner1, "hostid", NULL);
    users = createList(inner1, "users");
    user = createObject(users, "includetest-onlyexpireandinuse");
    includeThis(user, "expire:inuse");
    addString(user, "feature", "cvekernel");
    addString(user, "version", "2001.020");
    addNumber(user, "inuse", 10);
    addString(user, "expire", "01-jun-2029");
    destroy(user);
    destroy(users);
    user = createObject(users, "excludetest-onlyversion");
    includeThis(user, "expire:inuse:feature");
    addString(user, "feature", "cvekernel");
    addString(user, "version", "2001.020");
    addNumber(user, "inuse", 10);
    addString(user, "expire", "01-jun-2029");
    destroy(user);
    destroy(users);
    destroy(inner1);

    destroy(outer);
    destroy(toplevel);
    }
    return 0;
}

