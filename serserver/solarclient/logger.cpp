#include <errno.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/time.h>
#include <syslog.h>
#include "logger.h"

using namespace std;

bool Logger::switchfile(const char *fn)
{
     int retval=true;
     FILE *newfd;

     if((newfd=fopen(fn,"a"))==NULL) {
         syslog(LOG_ERR,"Error open file %s for writing reason: %s. keeping old.\n",
                        fn,strerror(errno));
         cerr << "Error opening file " << fn << " for writing reason: "
              << strerror(errno) << " keeping old." << endl;
         retval=false;
     } else {
         strcpy(filename,fn);
         
         if(fd!=NULL) fclose(fd);
         fd=newfd;

         syslog(LOG_INFO,"opened new file: %s\n",fn);
         cerr << "opened new file: " << fn << endl;

         sec_in_day_old=-1;
     }
     return retval;
}

int Logger::process_nicad(char *inbuf)
{
    fprintf(fd,"%s",inbuf);
    fflush(fd);
    printf("%s",inbuf);
    return 0;
}

int Logger::process(char *inbuf)
{
    char buf[200];

    if(sscanf(inbuf,"%s %d-%d-%d %d:%d:%d 0x%d %d %d %d %f %f %f %f %f",
              l.prefix,&l.year,&l.month,&l.day,&l.hour,&l.min,&l.sec,
              &l.cmd,&l.stat1,&l.stat2,&l.pwm,&l.vsolar,
              &l.vbatt,&l.current,&l.vsolaropen,&l.vsetpoint)>0) {


        if(l.cmd==50) {
            if(l.current>sl->get_maxcurrent()) { sl->set_maxcurrent(l.current); }

            sec_in_day=l.hour*3600 + l.min*60 + l.sec;
            if(sec_in_day_old==-1) {
                sec_in_day_old=sec_in_day;
            } else {
                int dt=sec_in_day-sec_in_day_old;
                float mAh=sl->get_mAh();
                mAh += ((float)dt)/3600. * l.current;
                sl->set_mAh(mAh);
                sec_in_day_old=sec_in_day;
            }


            sprintf(buf,
   "%04d-%02d-%02d %02d:%02d:%02d 0x%d  %03d %.01f %.01f %.01f %.01f %.01f %.01f %.01f",
              l.year,l.month,l.day,l.hour,l.min,l.sec,
              l.cmd,l.pwm,l.vsolar,
              l.vbatt,l.current,l.vsolaropen,l.vsetpoint,sl->get_maxcurrent(),sl->get_mAh());


            //fprintf(fd,"%s stat1=%d stat2=%d\n",buf,l.stat1,l.stat2);
            fprintf(fd,"%s\n",buf);
            fflush(fd);
            //printf("%s\n",buf);
        }
    }
    return 0;
}

void Logger::getsample(Solarlogline *sample)
{
    *sample=l;
}

string Logger::getsample(void)
{
    ostringstream strs;
    strs << l.year << "-" 
         << setw(2) << setfill('0') << l.month << "-" 
         << setw(2) << setfill('0') << l.day << " "

         << setw(2) << setfill('0') << l.hour << ":"
         << setw(2) << setfill('0') << l.min << ":"
         << setw(2) << setfill('0') << l.sec 

         << " 0x" << l.cmd << " " << setw(3) << setfill('0') << l.pwm
         << "  " << fixed << setprecision(1) << l.vsolar
         << " " << setprecision(1) << l.vbatt
         << " " << setprecision(1) << l.current
         << " " << setprecision(1) << l.vsolaropen
         << " " << setprecision(1) << l.vsetpoint
         << " " << setprecision(1) << sl->get_maxcurrent()
         << " " << setprecision(1) << sl->get_mAh();

    return strs.str();
}

const char * Logger::getcurrentfilename(void)
{
    return filename;
}

const char * Logger::getstatus(void)
{
    return (DTRstatus ? "ON" : "OFF" );
}

