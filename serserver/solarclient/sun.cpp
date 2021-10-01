// C++ program calculating the sunrise and sunset for
// the current date and a fixed location(latitude,longitude)
// Jarmo Lammi 1999 - 2000
// Last update January 6th, 2000

// Implemented as C++ class by LeonH Sat Jul  7 13:19:46 CEST 2012

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <time.h>

#include "sun.h"

using namespace std;

double tpi = 2 * M_PI;
double degs = 180.0/M_PI;
double rads = M_PI/180.0;

double L,g,daylen;
double SunDia = 0.53;     // Sunradius degrees

double AirRefr = 34.0/60.0; // athmospheric refraction degrees //

//   Get the days to J2000
//   h is UT in decimal hours
//   FNday only works between 1901 to 2099 - see Meeus chapter 7
double FNday (int y, int m, int d, float h)
{
    int luku = - 7 * (y + (m + 9)/12)/4 + 275*m/9 + d;
    // type casting necessary on PC DOS and TClite to avoid overflow
    luku+= (long int)y*367;
    return (double)luku - 730531.5 + h/24.0;
}

//   the function below returns an angle in the range
//   0 to 2*M_PI
double FNrange (double x) {
    double b = x / tpi;
    double a = tpi * (b - (long)(b));
    if (a < 0) a = tpi + a;
    return a;
}

// Calculating the hourangle
//
double f0(double lat, double declin) {
    double fo,dfo;
    // Correction: different sign at S HS
    dfo = rads*(0.5*SunDia + AirRefr); if (lat < 0.0) dfo = -dfo;
    fo = tan(declin + dfo) * tan(lat*rads);
    if (fo>0.99999) fo=1.0; // to avoid overflow //
    if (fo<-0.99999) fo=-1.0; // to avoid overflow //
    fo = asin(fo) + M_PI/2.0;
    return fo;
}

// Calculating the hourangle for twilight times
//
double f1(double lat, double declin) {
    double fi,df1;
    // Correction: different sign at S HS
    df1 = rads * 6.0; if (lat < 0.0) df1 = -df1;
    fi = tan(declin + df1) * tan(lat*rads);
    if (fi>0.99999) fi=1.0; // to avoid overflow //
    if (fi<-0.99999) fi=-1.0; // to avoid overflow //
    fi = asin(fi) + M_PI/2.0;
    return fi;
}

// Calculating the hourangle for solarpanel times
//
double f2(double lat, double declin) {
    double fi,df2;
    // Correction: different sign at S HS
    //df2 = rads * -15.0; if (lat < 0.0) df2 = -df2;
    df2 = rads * -10.0; if (lat < 0.0) df2 = -df2;
    fi = tan(declin + df2) * tan(lat*rads);
    if (fi>0.99999) fi=1.0; // to avoid overflow //
    if (fi<-0.99999) fi=-1.0; // to avoid overflow //
    fi = asin(fi) + M_PI/2.0;
    return fi;
}

//   Find the ecliptic longitude of the Sun
double FNsun (double d)
{
    //   mean longitude of the Sun
    L = FNrange(280.461 * rads + .9856474 * rads * d);

    //   mean anomaly of the Sun
    g = FNrange(357.528 * rads + .9856003 * rads * d);

    //   Ecliptic longitude of the Sun
    return FNrange(L + 1.915 * rads * sin(g) + .02 * rads * sin(2 * g));
}

// Display decimal hours in hours and minutes
void showhrmn(double dhr) {
    int hr,mn;
    hr=(int) dhr;
    mn = (dhr - (double) hr)*60;
    if (hr < 10) cout << '0';
    cout << hr << ':';
    if (mn < 10) cout << '0';
    cout << mn;
}

void Sun::calculate_sunrise_sunset(void)
{
    double y,m,day,h;
    struct timeval t;
    struct tm *p;
    int daylightsaving;

    //  get the date and time from the user
    // read system date and extract the year

    /** First get time **/
    gettimeofday(&t,NULL);
    p = localtime(&t.tv_sec);
    y = p->tm_year+1900;
    m = p->tm_mon+1;
    day = p->tm_mday;
    daylightsaving=p->tm_isdst;

    h = 12;

    tzone=1+daylightsaving;

    // testing
    // m=6; day=10;

    double d = FNday(y, m, day, h);

    //   Use FNsun to find the ecliptic longitude of the
    //   Sun

    double lambda = FNsun(d);

    //   Obliquity of the ecliptic

    double obliq = 23.439 * rads - .0000004 * rads * d;

    //   Find the RA and DEC of the Sun

    double alpha = atan2(cos(obliq) * sin(lambda), cos(lambda));
    double delta = asin(sin(obliq) * sin(lambda));

    // Find the Equation of Time
    // in minutes
    // Correction suggested by David Smith
    double LL = L - alpha;
    if (L < M_PI) LL += tpi;
    double equation = 1440.0 * (1.0 - LL / tpi);
    double ha = f0(latit,delta);
    double hb = f1(latit,delta);
    double hc = f2(latit,delta);
    double twx = hb - ha;       // length of twilight in radians
    double cwx = hc - ha;       // length of solarpaneltime in radians
    twx = 12.0*twx/M_PI;                // length of twilight in hours
    cwx = 12.0*cwx/M_PI;                // length of solarpaneltime in hours
    //cout << "ha=" << ha << "  hb=" << hb << endl;
    // Conversion of angle to hours and minutes //
    daylen = degs*ha/7.5;
    if (daylen<0.0001) {daylen = 0.0;}
    // arctic winter     //

    sunrise = 12.0 - 12.0 * ha/M_PI + tzone - longit/15.0 + equation/60.0;
    sunset = 12.0 + 12.0 * ha/M_PI + tzone - longit/15.0 + equation/60.0;
    double noont = sunrise + 12.0 * ha/M_PI;
    double altmax = 90.0 + delta * degs - latit;
    // Correction for S HS suggested by David Smith
    // to express altitude as degrees from the N horizon
    if (latit < delta * degs) altmax = 180.0 - altmax;

    double twam = sunrise - twx;  // morning twilight begin
    double twpm = sunset + twx;  // evening twilight end

    tsolarup = sunrise - cwx;  // morning solarpanel begin
    tsolardown = sunset + cwx;  // evening solarpanel end

    if (sunrise > 24.0) sunrise-= 24.0;
    if (sunset > 24.0) sunset-= 24.0;
    if (tsolarup > 24.0) tsolarup-= 24.0;
    if (tsolardown > 24.0) tsolardown-= 24.0;

   /* 
    cout << "\n Sunrise and set\n";
    cout << "===============\n";

    cout.setf(ios::fixed);
    cout.precision(0);
    cout << "  year  : " << y << '\n';
    cout << "  month : " << m << '\n';
    cout << "  day   : " << day << "\n\n";
    cout << "Days until Y2K :  " << d << '\n';
    cout.precision(2);
    cout << "Latitude :  " << latit << ", longitude:  " << longit << '\n';
    cout << "Timezone :  " << tzone << "\n\n";
    cout << "Declination   : " << delta * degs << '\n';
    cout << "Daylength     : "; showhrmn(daylen); cout << " hours \n";
    cout << "Civil twilight: ";
    showhrmn(twam); cout << '\n';
    cout << "Sunrise       : ";
    showhrmn(sunrise); cout << '\n';
    cout << "Solarpanel up: ";
    showhrmn(tsolarup); cout << '\n';

    cout << "Sun altitude at noontime ";
    // Amendment by D. Smith
    showhrmn(noont); cout << " = " << altmax << " degrees"
        << (latit>=0.0 ? " S" : " N") << endl;
    cout << "Sunset        : ";
    showhrmn(sunset); cout << '\n';
    cout << "Civil twilight: ";
    showhrmn(twpm); cout << '\n';
    cout << "Solarpanel down: ";
    showhrmn(tsolardown); cout << '\n';
   */ 
}

bool Sun::is_down(void)
{
    struct timeval t;
    struct tm *p;

    gettimeofday(&t,NULL);
    p = localtime(&t.tv_sec);

    double mytime;
    mytime = (double)p->tm_hour + ((double)p->tm_min)/60.;

/*
    cout << "Sun::is_down mytime=";
    showhrmn(mytime);
    cout << " sunset="; 
    showhrmn(sunset);
    cout << " tsolarup="; 
    showhrmn(tsolarup);
    cout << endl;
*/

    if(mytime > 23.) return true; // after 23:00 its dark anyways

    //if(mytime>sunset) return true;
    if(mytime>tsolardown) return true;
    else return false;
}

bool Sun::is_up(void)
{
    struct timeval t;
    struct tm *p;
//return true;
    gettimeofday(&t,NULL);
    p = localtime(&t.tv_sec);

    double mytime;
    mytime = (double)p->tm_hour + ((double)p->tm_min)/60.;

/*
    cout << "Sun::is_up mytime=";
    showhrmn(mytime);
    cout << " sunrise="; 
    showhrmn(sunrise);
    cout << " tsolarup="; 
    showhrmn(tsolarup);
    cout << endl;
*/

    //if(mytime>sunrise && !is_down()) return true;
    if(mytime>tsolarup && !is_down()) return true;
    else return false;
}

const char * Sun::getsuninfo(void)
{
    ostringstream strs;
    int hr,mn;

    hr=(int)sunrise;
    mn = (sunrise-(double)hr)*60;
    strs << "RISE=" << setw(2) << setfill('0') << hr << ":" 
                  << setw(2) << setfill('0') << mn;

    hr=(int)sunset;
    mn = (sunset-(double)hr)*60;

    strs << " SET=" << setw(2) << setfill('0') << hr << ":" 
                    << setw(2) << setfill('0') << mn;

    return strs.str().c_str();
}

const char * Sun::getstatus(void)
{
    ostringstream strs;
    int hr,mn;

    //hr=(int)sunrise;
    hr=(int)tsolarup;
    //mn = (sunrise-(double)hr)*60;
    mn = (tsolarup-(double)hr)*60;
    strs << "ON=" << setw(2) << setfill('0') << hr << ":" 
                  << setw(2) << setfill('0') << mn;

    //hr=(int)sunset;
    hr=(int)tsolardown;
    //mn = (sunset-(double)hr)*60;
    mn = (tsolardown-(double)hr)*60;

    strs << " OFF=" << setw(2) << setfill('0') << hr << ":" 
                    << setw(2) << setfill('0') << mn;

    return strs.str().c_str();
}

