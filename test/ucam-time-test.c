/* Test program for ucam time */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../src/unicam-time.h"

int main(int argc, char** argv)
{
    time_t now_t = time(NULL);
    struct tm now = *localtime(&now_t);
    struct uni_term_date date;
    if(NULL == uni_term_make(&now, &date)) {
        return EXIT_FAILURE;
    }

    printf("yr %i, day %i, week %i, %s\n",
            date.academic_year, date.day+1, date.week,
            uni_term_name_to_string(date.name));

    return EXIT_SUCCESS;
}
