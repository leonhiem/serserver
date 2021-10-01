#include <string.h>
#include <stdlib.h>


#include "getopts.h"
#include <iostream>
#include <iomanip>

GetOpts::GetOpts(struct options opts[], int optslen, int argc, char *argv[])
{
    int i,j;
    for(i=0; i<argc; i++) {
      if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 ) {
	usage(argv[0], optslen, opts);
	exit(1);
      }
    }

    for(i=1; i<argc; i++) {
      for(j=0;j<optslen;j++) {	    
	if( strlen(argv[i])>2 &&  strncmp(argv[i]+2, opts[j].name, strlen(opts[j].name)) == 0 ) {
	  if(strlen(opts[j].argex) > 0) {
	    char *p;
	      if ( (p=strstr(argv[i],"=")) == NULL ) {
		cerr << "Required parameter not given for " << argv[i] << endl;
		exit(1);
	      }
	    ++p;
            char *ptr = new char[strlen(p)];
	    *opts[j].vartoset = ptr;
	    strcpy(ptr, p);
	  }
	  else {
            char *ptr = new char[3]; // EW, was 1, but "10" actually is 3 bytes
	    *opts[j].vartoset = ptr;
	    strcpy(ptr, "10");
	  }
	  break;
	}
      }
      if (j>=optslen) {
	usage(argv[0], optslen, opts);
	exit(1);
      }
    } 
}

void GetOpts::usage(char *progn, int optslen, struct options opts[])
{
  int ind=2;
  int pos2=32;
  int pos3=83;

  cout << "Usage: " << progn << " [options]" << endl << endl;
  cout << indent(ind) << "--help," << indent(pos2-ind-7) 
       << "Displays this message" << endl;
  for(int i = 0; i<optslen; i++) {
    int len;
    if(strlen(opts[i].argex) > 0)
      len=strlen(opts[i].name)+strlen(opts[i].argex)+2;
    else
      len=strlen(opts[i].name)+strlen(opts[i].argex)+1;	
    cout << indent(ind) << "--" << opts[i].name;
    if(strlen(opts[i].argex) > 0)
      cout << '=' << opts[i].argex;
    cout << ',';
    // Calculate the number of spaces for the first line
    int space=pos2-ind-2-len;
    int linelen=pos3-pos2; // String is wrapped beyond this length.
    int lentot=strlen( opts[i].descr); // The length of the description string;
    int start = 0;
    int stop = lentot;
    if (stop > linelen)
      stop = linelen;
    if (stop > lentot)
      stop = lentot;
    // The first line may be different
    string description(opts[i].descr);
    if (space>0)
      cout << indent(space) << description.substr(start,stop) << endl;
    else 
      cout << endl << indent(pos2) << description.substr(start,stop) << endl;
    start=stop;
    stop=start+linelen;
    if (stop > lentot)
      stop = lentot;
    while (start < lentot) {
      //cout << "Start=" << start << " Stop=" << stop << " Diff=" << stop-start << endl;
      cout << indent(pos2) << description.substr(start,stop-start) << endl;
      start=stop;
      stop=start+linelen;
      if (stop > lentot)
	stop = lentot;

    }
  }
}
