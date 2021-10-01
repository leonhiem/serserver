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


Client *client=NULL;


bool server_query(const char *cmd, const char *arg) throw(const char*)
{
    server_response response;
    int errors=0;
    if((errors=client->command(cmd,arg,&response)) ==0) {
        istringstream vecstr(response.result);

        cout << "CMD=" << cmd << " ARG=" << arg
             << " RESULT=" << response.result
             << endl;
    }
    return true;
}

int main (int argc, char* argv[])
{
    const char* host = "localhost";
    int port = 3340;
    int exitstatus=EXIT_SUCCESS;
    char *cmd=new char[100];
    char *arg=new char[100];


    try { 
        if(argc<3) {
            throw("expect: host command");
        }

        host = argv[1];

        char *inputptr=argv[2];
        char *cmdptr=cmd;
        char *argptr=arg;
        while(*inputptr!=0) {
            *cmdptr++=*inputptr++;
            if(*inputptr=='-' || *inputptr=='=') {
                *cmdptr=0; inputptr++; break;
            }
        }
        while(*inputptr!=0) {
            *argptr++=*inputptr++;
        }
        //cout << "cmd: " << cmd << endl;
        //cout << "arg: " << arg << endl;


        if(cmd==NULL) throw("bad command");
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
            // read banner from server:
            int len;
            unsigned char buf[1000];
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

                server_query(cmd, arg);
            }
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

