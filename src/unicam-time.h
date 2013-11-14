#ifndef UNICAM_TIME_H__
#define UNICAM_TIME_H__

#ifdef NON_PEBBLE
#include <time.h>
#else
#include <pebble.h>
#endif

enum uni_term_name
{
    TERM_NAME_MICHAELMAS,
    TERM_NAME_CHRISTMAS_VACATION,
    TERM_NAME_LENT,
    TERM_NAME_EASTER_VACATION,
    TERM_NAME_EASTER,
    TERM_NAME_LONG_VACATION,
};

struct uni_term_date
{
    int                 academic_year;  /* N.B. year which Michaelmas falls in */
    enum uni_term_name  name;
    int                 week;   /* [-1, 8] weeks from start of week 1 (-1 and 8 are rare) */
    int                 day;    /* [0, 7] days from start of week */
};

struct uni_term_date* uni_term_make(const struct tm* timeval, struct uni_term_date* output);
const char* uni_term_name_to_string(enum uni_term_name name);

#endif /* UNICAM_TIME_H__ */
