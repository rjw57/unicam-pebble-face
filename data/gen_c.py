#!/usr/bin/env python
# vim: set fileencoding=utf-8

import datetime
import re
import sys

from bs4 import BeautifulSoup
from pytz import utc

def to_posix_time(t):
    delta = t.astimezone(utc) - datetime.datetime(1970, 1, 1, tzinfo=utc)
    return delta.days*3600*24 + delta.seconds

def main():
    soup = BeautifulSoup(sys.stdin)

    year_range_re = re.compile(u'([0-9]+)[-–]([0-9]+)')

    print('''/* This file is auto-generated by gen_c.py. DO NOT EDIT. */

#include <time.h>

struct uni_year
{
    int     start_year;

    /* End times are not inclusive. */
    time_t  mich_start, mich_end;
    time_t  lent_start, lent_end;
    time_t  easter_start, easter_end;
};
''')
    print('struct uni_year year_records[] = {')

    tz = utc
    
    n_years = 0
    for row in soup('tr'):
        cells = row('td')

        # Must have at least the cells we're expecting
        if len(cells) < 9:
            continue

        # Get the concatenated stripped strings for each cell
        cell_strings = list(''.join(x.stripped_strings) for x in cells)

        # A data row starts with a year range
        years = year_range_re.match(cell_strings[0])
        if years is None:
            continue

        start_year = int(years.group(1))

        # Note that ends are not inclusive

        mich_start = tz.localize(datetime.datetime(start_year, 10, int(cell_strings[1]))).astimezone(utc)
        mich_end = mich_start + datetime.timedelta(weeks=8, days=4)

        lent_start = tz.localize(datetime.datetime(start_year+1, 1, int(cell_strings[3]))).astimezone(utc)
        lent_end = lent_start + datetime.timedelta(weeks=8, days=4)

        easter_start = tz.localize(datetime.datetime(start_year+1, 4, int(cell_strings[6]))).astimezone(utc)
        easter_end = easter_start + datetime.timedelta(weeks=7, days=4)

        n_years += 1
        row = ', '.join(str(x) for x in (
            start_year,
            to_posix_time(mich_start), to_posix_time(mich_end),
            to_posix_time(lent_start), to_posix_time(lent_end),
            to_posix_time(easter_start), to_posix_time(easter_end),
        ))
        print('    { ' + row + ' },')

    print('''};
''')
    print('#define N_YEAR_RECORDS {0}'.format(n_years))

if __name__ == '__main__':
    main()