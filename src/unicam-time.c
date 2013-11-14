#include "unicam-time.h"

#include <stdlib.h>
#ifdef NON_PEBBLE
#include <time.h>
#else
#include <pebble.h>
#include "mktime.h"
#endif

#include "term-dates.h"

/* seconds in a day */
#define DAY (24*60*60)

struct uni_term_date* uni_term_make(const struct tm* timeval, struct uni_term_date* output)
{
    /* Convert time to POSIX timestamp. */
    struct tm tmp_tm = *timeval;
    time_t timestamp = mktime(&tmp_tm);

    /* What are the earliest and latest times we know about? */
    time_t ken_start = year_records[0].mich_start, ken_end = year_records[N_YEAR_RECORDS-1].easter_end;

    /* Don't return anything if the timestamp is beyond our ken. */
    if((timestamp < ken_start) || (timestamp > ken_end)) {
        return NULL;
    }

    /* Find which year we're within. */
    const struct uni_year* year_p = NULL;
    if(timestamp >= year_records[N_YEAR_RECORDS-1].mich_start) {
        year_p = &year_records[N_YEAR_RECORDS-1];
    } else {
        for(int record_idx=0; (year_p == NULL) && (record_idx<N_YEAR_RECORDS-1); record_idx++)
        {
            const struct uni_year* this_year = &year_records[record_idx];
            const struct uni_year* next_year = &year_records[record_idx+1];

            if((timestamp >= this_year->mich_start) && (timestamp < next_year->mich_start)) {
                year_p = this_year;
            }
        }
    }

    /* If, for some reason, we failed to find a year, return failure. */
    if(NULL == year_p) {
        return NULL;
    }

    /* Record the academic year, hour, minute and second */
    output->academic_year = year_p->start_year;

    /* The start of 'week 0' */
    time_t start;

    /* Now find which part of the year we're in. */
    if(timestamp < year_p->mich_end) {
        output->name = TERM_NAME_MICHAELMAS;
        start = year_p->mich_start;
    } else if(timestamp < year_p->lent_start) {
        output->name = TERM_NAME_CHRISTMAS_VACATION;
        start = year_p->mich_end;
    } else if(timestamp < year_p->lent_end) {
        output->name = TERM_NAME_LENT;
        start = year_p->lent_start;
    } else if(timestamp < year_p->easter_start) {
        output->name = TERM_NAME_EASTER_VACATION;
        start = year_p->lent_end;
    } else if(timestamp < year_p->easter_end) {
        output->name = TERM_NAME_EASTER;
        start = year_p->easter_start;
    } else {
        output->name = TERM_NAME_LONG_VACATION;
        start = year_p->easter_end;
    }

    /* Week "1" starts on the first Thursday. */
    struct tm wk1_start;
    wk1_start = *localtime(&start);
    while(wk1_start.tm_wday != 4) {
        wk1_start.tm_mday++;
        wk1_start.tm_isdst = -1;
        mktime(&wk1_start); // mktime normalises the tm structure
    }

    /* We can't just use difftime because of daylight savings. Grr. This is
     * inefficient but correct. It will iterate at most 60 times. */
    tmp_tm = wk1_start;
    int days_into_term = 0;
    while((tmp_tm.tm_year != timeval->tm_year) || (tmp_tm.tm_yday != timeval->tm_yday)) {
        tmp_tm.tm_mday++;
        tmp_tm.tm_isdst = -1;
        days_into_term++;
        mktime(&tmp_tm);
    }

    output->week = days_into_term / 7;
    output->day = days_into_term % 7;

    return output;
}

const char* uni_term_name_to_string(enum uni_term_name name)
{
    switch(name) {
        case TERM_NAME_MICHAELMAS:
            return "Michaelmas";
        case TERM_NAME_CHRISTMAS_VACATION:
            return "Christmas vac.";
        case TERM_NAME_LENT:
            return "Lent";
        case TERM_NAME_EASTER_VACATION:
            return "Easter vac.";
        case TERM_NAME_EASTER:
            return "Easter";
        case TERM_NAME_LONG_VACATION:
            return "Long vac.";
    };

    return "???";
}
