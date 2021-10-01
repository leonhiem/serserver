#ifndef SERSERVER_H
#define SERSERVER_H 1

using namespace std;

#include <list>
#include <string.h>

#include "serial.h"  
#include "socket.h"  
#include "getopts.h"  

#define MODE_NOTHING 0
#define MODE_SOLAR   1
#define MODE_NICAD   2


class Serverdat {
public:
    int tcpport;
    const char* ctlnetif;
    int human;
    int mode;
    bool on;
    Serial *ser;
    Serial_cfg ser_cfg;
    bool ser_is_open;
};



/*
   The SOLARServer handles tile settings from client to all tiles
   It can be controlled via a control socket.
*/
class SERServer {

private:  
  TCPCSSocket* controlSocket;

  typedef union {
    int Int;
    double Double;
    const char *Str;
  } parameter_type;

  typedef struct {
    int valid;
    parameter_type p1;
    parameter_type p2;
    parameter_type p3;
  } cmd_parameters_type;

  typedef struct {
    int cmd;
    const char *cmdstr;
    const char *syntaxstr;
    const char *helpstr;
  } cmd_list_type;

  enum { 
	QUIT,
        GETVERSION,
        MYNAME,

	OPEN,
	CLOSE,
	WRITE,
	READ,
	STATUS,

	HELP, 
	DEFAULT
  };

  static const cmd_list_type cmd_list[];
  static void * thread_function( void *ptr );
  static void * thread_Run( void *ptr );

 public:

  SERServer(const char* ctrlif, uint16_t tcpport=3340 )
	    throw(const char*);
  ~SERServer();
  int Run(Serverdat *);
  int GetCommand(char *buf, size_t len, cmd_parameters_type &p);
  int GetDouble(const char* buf, double &d);
  int GetInt(const char* buf, int &i);
  int GetStr(const char* buf, const char* &s);
  typedef int (SERServer::*ModeFie)();

  bool startup(Serverdat *sd, int beam, TCPCSocket *tile) throw(const char *);

  size_t ctrlTX(const char* str) 
  { return controlSocket->tx((const unsigned char*)str, strlen(str)); } 

  size_t ctrlTX(const unsigned char* buf, size_t len);

  size_t ret_ctrlTX(bool ret, const char* cmd, const char *msg)
  { ostringstream strs;
    strs << cmd << " " << (ret ? "OK" : "Error") << " " << msg << endl;
    return ctrlTX(strs.str().c_str());
  }
};


#endif
