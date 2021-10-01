#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include "parse.h"
#include "client.h"
#include "sun.h"

Client *client=NULL;

bool server_query(const char *cmd, const char *arg, FILE *fd=NULL) throw(const char*)
{
    server_response response;
    int errors=0;
    //cout << "cmd=" << cmd << " arg=" << arg << endl;
    if((errors=client->command(cmd,arg,&response)) ==0) {
        istringstream vecstr(response.result);

        //cout << "CMD=" << cmd << " ARG=" << arg
        //     << " RESULT=" << response.result
        //     << endl;
        if(fd!=NULL) {
            //cout << "write:" << response.result <<  endl;
            fprintf(fd,"%s\n",response.result);
        }
    }
    return true;
}

int main (int argc, char* argv[])
{
    const char* host = "localhost";
    const char* serialcmd = "l";
    int port = 3340;
    int exitstatus=EXIT_SUCCESS;
    char *cmd=new char[100];
    char *arg=new char[100];
    Sun sun;
    sun.calculate_sunrise_sunset();

    if(!sun.is_up()) {
        //cout << "Sun is down" << endl;
        exit(exitstatus);
    }

    //cout << "Sun is up" << endl;

    struct timeval t;
    struct tm *td = new(struct tm);
    gettimeofday(&t,NULL);
    td = localtime(&t.tv_sec);
    char fn[100];
    FILE *fd;

#define SOLAR_DATA_DIR "/home/leon/aerostar/public_html/solar/data/"
    sprintf(fn,"%ssolarlog-%04d-%02d-%02d.dat",SOLAR_DATA_DIR,
            td->tm_year+1900,td->tm_mon+1,td->tm_mday);

    try { 
        if(argc<3) {
            throw("expect: host serialcmd");
        }

        host = argv[1];
        serialcmd = argv[2];

        if((fd=fopen(fn,"a"))==NULL) {
            throw("open file");
        }
    } catch(const char *str) {
        cerr << "Error: " << str << endl;
        exitstatus=EXIT_FAILURE;
        exit(exitstatus);
    }

    int retries=3;
    while(retries--) {
        try {
            client = new Client(host,port); 
            break;
        } catch(const char *str) {
            cerr << "Error: " << str << " (retrying...)" << endl;
            usleep(5000000);
        }
    }
    if(retries<=0) exit(EXIT_FAILURE);


    retries=3;
    while(retries--) {
        try {
            int len;
            unsigned char buf[1000];
            // read banner from server:
            len = client->rx(buf, 1000);
            if (len<1) {
                delete client;
                client=NULL;
                throw("server is disconnected");
            } else {
                buf[len-1]=0; // remove endl
                //cout << "Connected to server: " << host 
                //     << " at port: " << port
                //     << " banner: " << buf << endl;

                server_query("open", "");
                //usleep(1000000);
                //try { server_query("read", ""); 
                //} catch(...) { /*cout << "ignoring error" << endl;*/ }
                //usleep(100000);
                server_query("write", serialcmd);
                usleep(1000000);
                server_query("read", "",fd);
                //usleep(1000000);
                //server_query("close", "");
            }
            fclose(fd);
            exitstatus=EXIT_SUCCESS;
            delete client;
            break;
        } catch(const char *str) {
            cerr << "Error: " << str << " (retrying...)" << endl;
            exitstatus=EXIT_FAILURE;
            usleep(3000000);
        }
    }
    exit(exitstatus); 
}

