

 /**
  * @file apitrace.h
  * @brief C-preprocessor macros for API tracing
  * @author thepainters@gmail.com
  */

#ifndef __API_TRACING_INCLUDED__
#define __API_TRACING_INCLUDED__
#ifdef PC
#define __func__ __FUNCTION__
#endif
#include <stdio.h>
#define API_TRACING_INIT(API) \
     short API ## _TRACING = 0; \
     short API ## _TRACING_SAVE = 0; \
     FILE * API ## _TRACING_STREAM = (FILE *) NULL; \
     int API ## _check_for_tracing(); \
     void API ## _trace_close(); \
     void API ## _trace_file(char *); \
     void API ## _trace_set(int); \
     FILE * API ## _trace_stream(); \
void API ## _trace_file(char *file) { \
    FILE *tmp = NULL; \
    tmp = fopen(file, "w+"); \
    if ((FILE*) tmp != (FILE*) NULL) { \
      API ## _TRACING_STREAM = tmp; \
    } else { \
      API ## _TRACING_STREAM = stderr; \
    } \
} \
int API ## _check_for_tracing() { \
    if ((void*)getenv(#API "_TRACING_FILE") != (void*) NULL) { \
        if (API ##_TRACING_STREAM == (FILE *) NULL) { API ## _trace_file( (char *) getenv (#API "_TRACING_FILE")); } \
        return 1; \
    };\
    if (API ## _TRACING_STREAM == NULL) API ## _TRACING_STREAM = stderr; \
    if ((void*)getenv(#API "_TRACING") != (void*) NULL) return 1; \
    if ((void*)getenv(#API "_TRACE") != (void*) NULL) return 1; \
    return API ## _TRACING; \
} \
void API ## _trace_close(void) { \
    if ((void*) API ## _TRACING_STREAM != (FILE*) NULL) fclose(API ## _TRACING_STREAM); \
    API ## _TRACING_STREAM = NULL; \
} \
void API ## _trace_set(int val) { \
    API ## _TRACING = val; \
    if (API ## _TRACING_STREAM == NULL) API ## _TRACING_STREAM = stderr; \
} \
 \
FILE* API ## _trace_stream() { \
  return API ## _TRACING_STREAM; \
}

#define API_TRACING_STREAM(API) ((API ##_trace_stream() == NULL) ? stdout : API ##_trace_stream())
#define API_TRACE_FROM_FILE(API,fmt, ...) \
        do { if (API ##_check_for_tracing()) fprintf(API ##_TRACING_STREAM, "\n/* from %s:%d:%s()*/\n\t" fmt "\n",  __FILE__, \
                                __LINE__, __func__ , __VA_ARGS__); fflush ((FILE *) API ##_TRACING_STREAM);} while (0)
#define API_TRACE(API,fmt, ...) \
        do { if (API ##_check_for_tracing()) { fprintf(API ##_TRACING_STREAM, "\n\t" fmt "\n", __VA_ARGS__); fflush((FILE *) API ##_TRACING_STREAM);} } while (0)
#define API_TRACE_BLURB(API,fmt, ...) \
        do { if (API ##_check_for_tracing()) {fprintf(API ##_TRACING_STREAM, fmt,  __VA_ARGS__);} } while (0)

#define API_TRACE_HIDE(API) \
        do { API ##_TRACING_SAVE= API ##_TRACING; if ( API ## _check_for_tracing()) fprintf(API ## _TRACING_STREAM, "\n/*\n"); API ## _TRACING=0;} while (0)

#define API_TRACE_SHOW(API) \
        do { API ## _TRACING=API ## _TRACING_SAVE; if (API ## _check_for_tracing()) fprintf(API ## _TRACING_STREAM, "\n*/\n");fflush ((FILE *) API ##_TRACING_STREAM); } while (0)

#define API_TRACE_PRINT(API,fmt, ...) \
        do { if (API ## _check_for_tracing()) {fprintf(API ## _TRACING_STREAM, "\n/*\n" fmt "\n*/\n", __VA_ARGS__); fflush ((FILE *) API ##_TRACING_STREAM);} \
        else { fprintf(API ## _TRACING_STREAM, fmt, __VA_ARGS__); fflush (API ##_TRACING_STREAM);} } while (0)

#define API_TRACING_STOP(API) \
        do { API ## _trace_close(); } while(0)



/*
 * @code
 * // use environment variable: test_TRACE, test_TRACING, or test_TRACING_FILE
 * // If test_TRACING_FILE is set, then the output will be redirected to the file.
API_TRACING_INIT(test)

int main(int argc, char *argv[]) {
  API_TRACE(test, "%s", "Hello World\n");
  test_TRACING = 2;
  API_TRACING_STOP(test);
}
  * @endcode
*/
#endif
