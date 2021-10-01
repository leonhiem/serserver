#ifndef SUN_H
#define SUN_H


#define OMMERSCHANS_LAT       52.589469909668
#define OMMERSCHANS_LON       6.39221000671387
#define OMMERSCHANS_TIMEZONE  1

class Sun
{
private:
    double latit,longit;
    double tzone;
    double sunrise;
    double sunset;
    double tsolarup;
    double tsolardown;
public:
Sun() {
    latit = OMMERSCHANS_LAT;
    longit = OMMERSCHANS_LON;
    sunrise=0.;
    sunset=0.;
    tsolarup=0.;
    tsolardown=0.;
};

~Sun() {
}

void calculate_sunrise_sunset(void);
bool is_down(void);
bool is_up(void);
const char * getstatus(void); 
const char * getsuninfo(void); 

};
#endif 

