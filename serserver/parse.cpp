#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <syslog.h>

#include "parse.h"


using namespace std;

Parse::Parse()
{
}


void Parse::scanvector(uint8_t phases[], const char* vector, int max, int len)
{
  istringstream vecstr(vector);
  char c;
  unsigned x;
  // Remove '[' character
  vecstr >> c;
  for (int i=0; i<len; ++i) {
      vecstr >> x;
      phases[i] = x;
      if (phases[i]>max) {
        syslog(LOG_ERR,"Error, number exceeded maximum in vector parameter!\n");
        return;
      }
      vecstr >> c; // Extract ',' and the closing ']'
    }
  if (c!=']') {
    syslog(LOG_ERR,"Error, vector not properly closed\n");
    return;
  }
  return;
}

void Parse::scanvector(uint8_t *vec, const char* vector, int len)
{
  istringstream vecstr(vector);
  char c;
  unsigned x;
  int end = 0;
  // Remove '[' character
  vecstr >> c;
  if (c!='[') {
    syslog(LOG_ERR,"Error, Expected [ character\n");
    return;
  }
  for (int i=0; i<len;) {
    if (!end) {
      vecstr >> hex >> x;
      vecstr >> c; // Extract ',' or the closing ']'
    }
    vec[i] = x;
    i++;
    if (i < len && c == ']')
      end = 1;
  }
  if (c!=']') {
    syslog(LOG_ERR,"Error, vector not properly closed\n");
    return;
  }
  cout << endl;
  return;
}

int Parse::strtoint(const char* in)
{
  istringstream vecstr(in);
  unsigned x;
  if (in[0]=='0' && in[1]=='x')
     vecstr >> hex;
  vecstr >> x;
  return x;
}

int Parse::GetDouble(const char* buf, double &d)
{
  string sbuf(buf);

  size_t pos = sbuf.find("=");
  if (pos==string::npos)
    return 0;
  if (++pos>=sbuf.length())
    return 0;
  istringstream t(sbuf.substr(pos));
  t >> d;
  if (t.fail())
    return 0;
  return 1;
}

int Parse::GetInt(const char* buf, int &i)
{
  string sbuf(buf);

  size_t pos = sbuf.find("=");
  if (pos==string::npos)
    return 0;
  if (++pos>=sbuf.length())
    return 0;
  istringstream t(sbuf.substr(pos));
  t >> i;
  if (t.fail())
    return 0;
  return 1;
}

int Parse::GetStr(const char* buf, const char* &s)
{
  string sbuf(buf);

  size_t pos = sbuf.find("=");
  if (pos==string::npos)
    return 0;
  ++pos;
  // Skip spaces
  for (; buf[pos]==' '; ++pos);
  if (pos>=sbuf.length())
    return 0;
  s = &buf[pos];

  return 1;
}

