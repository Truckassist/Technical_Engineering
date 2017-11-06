#ifndef SYSTEMTIME_H
#define SYSTEMTIME_H

struct _SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
};
//  SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME
#endif // SYSTEMTIME_H
