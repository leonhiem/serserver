#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <iostream>
#include <sstream>
#include "serial.h"

class Solarlogline {
public:
    char prefix[10];
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int cmd;
    int stat1;
    int stat2;
    int pwm;
    float vsolar;
    float vbatt;
    float current;
    float vsolaropen;
    float vsetpoint;
};

class Solarstats {
protected:
    float maxcurrent;
    float mAh;
public:
    Solarstats() {
        mAh=0.;
        maxcurrent=0.;
        //printf("constructor Solarstats\n");
    };
    float get_mAh(void) { return mAh; }
    float get_maxcurrent(void) { return maxcurrent; }
    void set_mAh(float a) { mAh=a; }
    void set_maxcurrent(float c) { maxcurrent=c; }
};

class Logger : public Serial
{
private:
    FILE *fd;
    char filename[100];
    int tm_day_old;
    Solarlogline l;
    Solarstats *sl;
    int sec_in_day,sec_in_day_old;
public:
Logger(int devnr, Serial_cfg *cfg, const char *fn, Solarstats *s) : Serial(devnr, cfg) {
    sl=s;
    sec_in_day_old=-1;
    fd=NULL;
    //printf("constructor Logger\n");

    if((fd=fopen(fn,"a"))==NULL) {
        syslog(LOG_ERR,"Error open file %s for writing reason: %s.\n",
               fn,strerror(errno));
        throw("Error opening file for writing");
    }
    strcpy(filename,fn);
};

~Logger() {
    //printf("destructor Logger\n");
    if(fd!=NULL) fclose(fd);
}

int process(char *inbuf);
int process_nicad(char *inbuf);
bool switchfile(const char *fn);
void getsample(Solarlogline *sample);
std::string getsample(void);
const char * getcurrentfilename(void);
const char * getstatus(void);

};
#endif // LOGGER_H

