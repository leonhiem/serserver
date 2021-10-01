//
//
// Copyright (c) 2012 L. Hiemstra
//

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <list>
//#include <boost/regex.hpp>

#include <dirent.h>
#include <fnmatch.h>

#include <pthread.h>
#include <syslog.h>
#include <signal.h>

#include "getopts.h"  
#include "serial.h"  
#include "parse.h"  
#include "include/serserver.h"  

/* Global var */
volatile int ServerQuit = 0;
pthread_attr_t att; // Create attribute for pthread


#define VERSION "v1.0"

const char* SERSERVER_VERSION=VERSION " " __DATE__ " " __TIME__; 

const SERServer::cmd_list_type SERServer::cmd_list[] = {

    {QUIT,"quit","","\tdisconnect"},
    {GETVERSION,"version","","prints software version"},
    {MYNAME,"myname","","prints my name"},
    {OPEN,"open","","\tOpen serial port"},
    {CLOSE,"close","","Close serial port"},
    {WRITE,"write","cmd","Send this command to Serial port"},
    {READ,"read","","\tread data from serial port"},
    {STATUS,"status","","show current open status"},
    {HELP,"help","","\tprint this text"},
    {DEFAULT, "Command not exist or argument error","",""}
};


SERServer::SERServer(const char* ctrlif, uint16_t tcpport) throw(const char*)
{
    // We allow for a maximum of one client
    TCPSSocket sock(tcpport, ctrlif, 1);
    controlSocket = new TCPCSSocket(sock.listen());
}

SERServer::~SERServer()
{
    delete controlSocket;
}

void * thread_Run( void *ptr )
{
  Serverdat *sd = (Serverdat *)ptr;

  while(!ServerQuit) {
      SERServer *srv = new SERServer(sd->ctlnetif,sd->tcpport);
      srv->Run(sd);
      delete srv;
  }
  return NULL;
}

int SERServer::Run(Serverdat *sd)
{
  unsigned char buf[1600];
  int quit = 0;
  int len;
  cmd_parameters_type params;
  ostringstream banner;

  banner << "SER Control Server " << SERSERVER_VERSION << endl;



  ctrlTX(banner.str().c_str()); // Show banner

  while (!quit) {
      int new_cmd, can_read;
      
      new_cmd = can_read = 0;
      while(!new_cmd) {
          len = controlSocket->rx_peek(buf, 1600);
          if (len<1) {
              controlSocket->Shutdown();
              return(1);
          } else {
              for(int i=0;i<len;++i) {
                  if(buf[i]=='\n') {
                      can_read=i+1;
                      new_cmd=1;
                      break;
                  }
              }
          }
      }

      len = controlSocket->rx(buf, can_read);
      if (len<1) {
        controlSocket->Shutdown();
	return(1);
      } else {

        int cmd = GetCommand((char*)buf,len, params);
        switch (cmd) {
            case QUIT:
              quit=1;
              ret_ctrlTX(true,cmd_list[cmd].cmdstr,"");
              controlSocket->Shutdown();
              break;
            case OPEN:
            {
              bool retval=false;
              ostringstream strs;
              if(!sd->ser_is_open) { 
                  sd->ser_cfg.icrnl = 0;
                  sd->ser_cfg.crtscts = 0;
                  sd->ser_cfg.xonxoff = 0;
                  sd->ser_cfg.block = 1;
                  sd->ser_cfg.sendbreak = 0;
                  try {
                      strs << "open:";
                      sd->ser = new Serial(0,&sd->ser_cfg);
                      sd->ser->set_DTR(1); // turn on
                      sd->ser_is_open=true;

                      usleep(2000000);
                      char buf[1000];
                      struct timeval to;
                      to.tv_sec=1; to.tv_usec=0;
                      if(sd->ser->poll(&to)) {
                          int n;
                          if((n=sd->ser->read(buf,sizeof(buf))) <0) { // flush banner
                              throw("read"); 
                          }
                          retval=true;
                      } 

                      retval=true;
                      strs << "ok";
                  } catch(const char *str) {
                      strs << "failed: " << str;
                  }
              } else {
                  retval=true;
                  strs << "ok (already open)";
              }
              ret_ctrlTX(retval,cmd_list[cmd].cmdstr,strs.str().c_str());
              break;
            }
            case CLOSE:
              if(sd->ser_is_open) { delete sd->ser; sd->ser_is_open=false; }
              ret_ctrlTX(true,cmd_list[cmd].cmdstr,"closed");
              break;
            case WRITE:
            {
              bool retval=false;
              ostringstream strs;
              try {
                  strs << "write:";
                  if(!sd->ser_is_open) throw("serialport not open");

                  char buf[1000];
                  struct timeval to; to.tv_sec=0; to.tv_usec=10000;
                  if(sd->ser->poll(&to)) {
                      sd->ser->read(buf,sizeof(buf)); // flush 
                  }
                  string cmd=params.p1.Str;
                  cmd.append("\n");
                  if(sd->ser->write((const char *)cmd.c_str(),cmd.size()) < 0) {
                      throw("write");
                  }
                  usleep(100000);
                  to.tv_sec=1; to.tv_usec=0;
                  if(sd->ser->poll(&to)) {
                      int n;
                      if((n=sd->ser->read(buf,cmd.size())) <0) { // read cmd echo
                          throw("read"); 
                      }
                      retval=true;
                  } else throw("timeout: no data");

                  strs << "ok";
              } catch(const char *str) {
                  strs << "failed: " << str;
              }
              ret_ctrlTX(retval,cmd_list[cmd].cmdstr,strs.str().c_str());
              break;
            }
            case READ:
            {
              bool retval=false;
              ostringstream strs;
              try {
                  if(!sd->ser_is_open) throw("serialport not open");

                  char buf[1000];
                  struct timeval to; to.tv_sec=5; to.tv_usec=0;
                  if(sd->ser->poll(&to)) {
                      int n;
                      for(int i=0;i<10;i++) { // find first char, skip some
                          if((n=sd->ser->read(buf,1)) <0) throw("read"); 
                          if((isalpha(buf[0]) || isdigit(buf[0])) && !isblank(buf[0])) break;
                      }
                      if((n=sd->ser->read(&buf[1],sizeof(buf)-1)) <0) {
                          throw("read"); 
                      } else {
                          n++; // include buf[0]
                          buf[n]=0;

                          for(int i=(n-2);i>=0;i--) {
                              if(buf[i]==0 || !isascii(buf[i]) || buf[i]=='\r') buf[i]=' ';
                          }
                          for(int i=0;i<n;i++) {
                              if(buf[i]=='\n') { buf[i]=0; n=i; break; } // chop off \nCMD> prompt at end
                          }
/*
                          printf("read=%s\n",buf);
                          for(int i=0;i<n;i++) {
                              printf("[%02x]",buf[i]);
                          }
                          printf("\n");
*/

                          strs << buf;
                          retval=true;
                      }
                  } else throw("timeout: no data");
              } catch(const char *str) {
                  strs << str;
              }
              ret_ctrlTX(retval,cmd_list[cmd].cmdstr,strs.str().c_str());
              break;
            }
            case STATUS:
              ret_ctrlTX(true,cmd_list[cmd].cmdstr,(sd->ser_is_open?"open":"closed"));
              break;
            case MYNAME:
               ret_ctrlTX(true,cmd_list[cmd].cmdstr,"serserver");
               break;
            case GETVERSION:
              ret_ctrlTX(true,cmd_list[cmd].cmdstr,SERSERVER_VERSION);
              break;
            case HELP:
              {
                 ostringstream strs;
                 strs << endl << endl << "Every command response is defined as:" << endl
                      << "serserver command OK|Error message" << endl << endl 
                      << "List of commands:" << endl;
                 size_t Size = sizeof(cmd_list)/sizeof(cmd_list[0]);
                 for(size_t i=0;i<Size-1;++i) {
                     strs << cmd_list[i].cmd << ". ";
                     if(strlen(cmd_list[i].syntaxstr) > 0) {
                         strs << cmd_list[i].cmdstr << "=" << cmd_list[i].syntaxstr;
                     } else {
                         strs << cmd_list[i].cmdstr;
                     }
                     if(strlen(cmd_list[i].helpstr) > 0) {
                         strs << "\t" << cmd_list[i].helpstr << endl;
                     } else {
                         strs << endl;
                     }
                 }
                 ret_ctrlTX(true,cmd_list[cmd].cmdstr,strs.str().c_str());
               }
               break;
            case DEFAULT:
               ret_ctrlTX(false,cmd_list[cmd].cmdstr,"");
               break;
	}
     }
   }
   return(0);
}

int SERServer::GetCommand(char* buf, size_t len, cmd_parameters_type &p)
{
        int cmd=DEFAULT;
        Parse parse;

        buf[len]=0;
        size_t Size = sizeof(cmd_list)/sizeof(cmd_list[0]);

        // With telnet, a \r\n is added to the transmitted string
        // Get rid of them.

        string sbuf(buf);
        size_t pos = sbuf.find("\n");
        if (pos!=string::npos)
          buf[pos]=0;
        pos = sbuf.find("\r");
        if (pos!=string::npos)
          buf[pos]=0;
        
        if (len < 1)
           return DEFAULT;

        for (size_t i = 0; i<Size; ++i) {
          //cerr << "cmd_list[" << i << "]=" << cmd_list[i].cmdstr << endl;
          if (strncmp(cmd_list[i].cmdstr, buf, strlen(cmd_list[i].cmdstr))==0) {
            cmd = cmd_list[i].cmd;
            break;      
          }
        }

        // Parse and validate possible parameters
        switch (cmd) {
               //case CMD:
               //   if (parse.GetInt(buf,p.p1.Int)==0)
               //     cmd = DEFAULT;
               //   break;
               case WRITE:
                  if (parse.GetStr(buf,p.p1.Str)==0) cmd = DEFAULT;
                  break;
        }
        return cmd;
}


void process_SIGHUP (int sig)
{
    syslog(LOG_WARNING,"SIGHUP Received, No actions at the moment\n");
    signal(SIGHUP,  process_SIGHUP);
}
void process_SIGPIPE(int sig)
{
    syslog(LOG_WARNING,"SIGPIPE Received, No actions at the moment\n");
}
int init_daemon ()
{
    int status = 0;
    pid_t pid;

    signal(SIGHUP,  process_SIGHUP);
    signal(SIGPIPE, process_SIGPIPE); /* catches peer hangups */

    if ((pid = fork()) < 0) {
        syslog(LOG_ERR,"fork error\n"); //,strerror(errno));
        status = 1; // Error
    } else {
        if (pid != 0) {
            exit(0); /* Parent goes bye-bye */
                    
            /* Child continues */
            setsid();    /* Become session leader */
            chdir("/");  /* Change working directory */
            umask(0);    /* Clear out file mode creation mask */
        }
    }
    return status;
}

int main (int argc, char* argv[])
{	
try{
  const char* version = NULL;
  const char* ctlnetif = NULL;
  const char* dcport = NULL;

  // Defaults
  int tcpport = 3340;

  struct options opts[] = {
      { &version, "version", "Print version of server", "" },
      { &ctlnetif, "ctlnetif", "Specify network interface for control, defaults to all interfaces",
                   "ethX" },
      { &dcport, "dcport", "The TCP (ctrl) port to listen to (default=3340)", "3340" },
  };

  GetOpts _optprser = GetOpts(opts, sizeof(opts)/sizeof(opts[1]), argc, argv);
 

  if (version) {
    cout << argv[0] << ' ' << SERSERVER_VERSION << endl;
    exit(EXIT_SUCCESS);
  }

  if (!ctlnetif) ctlnetif = ""; // Listen on al interfaces
  if (dcport) tcpport = atoi(dcport);

   /*  Open syslog connection */
   openlog(basename(argv[0]),0,LOG_LOCAL0);
   //openlog(basename(argv[0]),0,LOG_USER);

   syslog(LOG_INFO,"Starting server %s\n",SERSERVER_VERSION);

#define RUN_AS_DAEMON 1
#ifdef RUN_AS_DAEMON
   // continue as a daemon:
   if(init_daemon () != 0) {
       exit(EXIT_FAILURE);
   }
#endif

   // SERServer needs to know these things:
   Serverdat serverdat;
   ostringstream sstr;

   serverdat.tcpport = tcpport;
   serverdat.human = 0;
   serverdat.mode = MODE_NOTHING;
   serverdat.on=false;
   serverdat.ser_is_open=false;

   serverdat.ctlnetif = new char(strlen(ctlnetif)+1);
   strcpy((char *)serverdat.ctlnetif,ctlnetif);


   /* Initialize and set thread detached attribute */
   if(pthread_attr_init(&att) != 0) { // initialize attribute
       sstr << "error pthread_attr_init: " << strerror(errno) << endl;
       throw(sstr.str().c_str());
   }
   if(pthread_attr_setstacksize(&att,100000) !=0) {
       sstr << "error pthread_attr_setstacksize: " << strerror(errno) << endl;
       throw(sstr.str().c_str());
   }

   // Start up the user thread:
   pthread_t thread_server;
   if(pthread_create(&thread_server, NULL, &thread_Run, (void*)&serverdat) != 0) {
       sstr << "error pthread_create: " << strerror(errno) << endl;
       throw(sstr.str().c_str());
   }

   pthread_join(thread_server,NULL);

   closelog(); // close syslog connection

  exit(EXIT_SUCCESS); }
catch(const char *str) {
  syslog(LOG_ERR,"catched error at main(): %s\n",str);
  closelog(); // close syslog connection
  exit(EXIT_FAILURE);
}
}
