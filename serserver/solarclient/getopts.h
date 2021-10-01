#ifndef GETOPTS_H
#define GETOPTS_H

#ifndef IOMANIP_INDENT_DEF
#define IOMANIP_INDENT_DEF
#include <iostream>

using namespace  std;

struct _Indent { int _M_n; };

inline _Indent indent(int __n)
{
  _Indent __x;
  __x._M_n = __n;
  return __x;
}

inline ostream& operator<<(ostream& os, _Indent I)
{
  for (int i = 0; i< I._M_n; ++i)
    os << ' ';
  return os;
}
#endif


struct options
{
    const char **vartoset;	//Address to the variable that will hold the value
    const char *name;		//Full name of the parameter
    const char *descr;	//Text description of the parameter
    const char *argex;	//If argument example of argument
};

class GetOpts
{
    public:
        GetOpts(struct options [], int, int, char* []);
    private:
        void usage(char *, int, struct options []);
};

#endif	/*GETOPTS_H*/
