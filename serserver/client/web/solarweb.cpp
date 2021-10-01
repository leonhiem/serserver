#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sstream>
//#include "parse.h"
//#include "client.h"

#define URL_SERVER "http://10.0.2.3/~leon/solar/"
#define HKSNEW "solarclient"

#define CGIBIN "/cgi-bin"

#define TD_GREEN  "<td style='background:#eeffee' "
#define TD_RED    "<td style='background:#ff1020' "
#define TD_ORANGE "<td style='background:#f08060' "
#define TD_BLUE   "<td style='background:#5050ff' "
#define TD_LIGHTBLUE   "<td style='background:#8080f0' "

#define TD_NEW    "<td style='background:#efefc0' "
#define TD_OLD    "<td style='background:#d5d5c0' "


void show_header(void)
{
    printf("Content-type: text/html\n");
    printf("\n");

    printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n");
    printf("       \"http://www.w3.org/TR/REC-html40/loose.dtd\">\n");

    printf("<HTML>\n");
    printf("<HEAD><TITLE>Solar Data Logger</TITLE>\n");
    printf("<META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\n");
    printf("<LINK REL=STYLESHEET "
           "HREF=\""
           URL_SERVER
           "/solar/solar.css\" "
           "TYPE=\"text/css\">\n");
    printf("</HEAD>\n");
    printf("<BODY BGCOLOR=\"#FFFFFF\"\">\n");
}

void show_ajaxroutines(int minboard, int maxboard)
{
    int board,i;

    printf("<script type=\"text/javascript\">\n");
    printf("var req;\n"
           "var k=0;\n"
           "var first;\n"
           "var init;\n"
           "window.onload = init();\n"
           "function init()\n"
           "{\n"
           "    first=1;\n"
           "    init=1;\n"
           "    setTimeout(displ,3000);\n"
           "}\n");
    printf("function displ()\n"
           "{\n"
           "    if(init==1) {\n"
           "        loadXMLDoc('" CGIBIN "/" HKSNEW "?init');\n"
           "        init=0;\n"
           "    } else if(first==1) {\n"
           "        loadXMLDoc('" CGIBIN "/" HKSNEW "?first');\n"
           "        first=0;\n"
           "    } else {\n"
           "        loadXMLDoc('" CGIBIN "/" HKSNEW "');\n"
           "    }\n"
           "    setTimeout(displ,2000);\n"
           "}\n"
           "function loadXMLDoc(url)\n"
           "{\n");
    printf("    // branch for native XMLHttpRequest object\n"
           "    if (window.XMLHttpRequest) {\n"
           "        req = new XMLHttpRequest();\n"
           "        req.onreadystatechange = processReqChange;\n"
           "        req.open(\"GET\", url, true);\n"
           "        req.send(null);\n");
    printf("    // branch for IE/Windows ActiveX version\n"
           "    } else if (window.ActiveXObject) {\n"
           "        req = new ActiveXObject(\"Microsoft.XMLHTTP\");\n"
           "        if (req) {\n"
           "            req.onreadystatechange = processReqChange;\n"
           "            req.open(\"GET\", url, true);\n"
           "            req.send();\n");
    printf("        }\n"
           "    }\n"
           "}\n");

    printf("function processReqChange()\n"
           "{\n"
           "    // only if req shows \"complete\"\n"
           "    if (req.readyState == 4) {\n"
           "        // only if \"OK\"\n"
           "        if (req.status == 200) {\n");
    printf("            // ...processing statements go here...\n"
           "            document.getElementById('systime').innerHTML = \n"
           "            req.responseXML.getElementsByTagName(\"systime\")[0].childNodes[0].nodeValue;\n"
           "            k++;\n");

        printf("            var datdir = req.responseXML.getElementsByTagName(\"datdir\");\n");
        printf("            var ndirsxml = datdir.length;\n");
        printf("            var pidx;\n");

        printf("            for(i=0;i<ndirsxml;i++) {\n");
            printf("              dir =  datdir.item(i);\n");
            printf("              document.getElementById(tagname).src = \n"
                  "              strip.getAttribute('file')+'?'+Math.random()*9999;\n");
                                                         /* ^^^ force a img refresh */
            printf("              document.getElementById(tagname).style.backgroundColor = \n"
                   "                     strip.getAttribute(\"lifestyle\");\n");
        printf("             }\n");
    
    printf("        } else {\n"
           "            alert(\"There was a problem retrieving the XML data\");\n"
           "        }\n"
           "    }\n"
           "}\n");

    printf("</script>\n");
}

void show_trailer(void)
{
    printf ("</td></tr></table>\n</BODY></HTML>\n");
}

void show_title(void)
{
    printf("<table width=100%% border=1 cellpadding=0>\n");
    printf("<tr><td align=left style='background:#e5E5f5'>\n");
    printf("<h1>Solar Data Logger</h1>");
    printf("</td></tr><tr><td>\n");
}


void show_legend(void)
{
    printf("<table align=right border=1 cellspacing=2 cellpadding=2 style='font-size:80%%'>\n");
    printf("<tr><td align=center style='background:#e5E5f5'>\n");
    printf("Limit Legend</td></tr>\n<tr><td>\n");

    printf("<table valign=top border=1 cellspacing=0 cellpadding=2>\n");
    printf("\t<tr>" TD_BLUE ">" "Minlimit</td>\n");
    printf("\t" TD_LIGHTBLUE ">" "Lowlimit</td>\n");
    printf("\t" TD_GREEN ">" "Normal</td>\n");
    printf("\t" TD_ORANGE ">" "Highlimit</td>\n");
    printf("\t" TD_RED ">" "Maxlimit</td>\n");
    printf("</table>\n</td></tr></table>\n");
}
void show_buttonbar(void)
{
    printf("<table align=right border=1 cellspacing=2 cellpadding=2 style='font-size:80%%'>\n");
    printf("<tr><td align=center style='background:#e5E5f5'>\n");

    //printf("Selectable Screens</td></tr>\n<tr><td>\n");
    printf("RUN</td></tr>\n<tr><td>\n");

    printf("<table valign=top border=1 cellspacing=4 cellpadding=4>\n");
/*
    printf("\t<tr><td>"
           "<a href=\""
           CGIBIN "/" HKCLIENT "\" onClick=\"hkwin=open('" CGIBIN "/" HKCLIENT "','HK',"
           "'resizable=1,scrollbars=1,width=1270,height=928,menubar=0,toolbar=1,"
           "location=1,status=1,fullscreen=1');hkwin.focus();return false;\""
           " target=\"HK\"><b>Stop</b>|</a>"
           "<a href=\""
           CGIBIN "/" HKCLIENT "\" onClick=\"hkwin=open('" CGIBIN "/" HKCLIENT "','HK',"
           "'resizable=1,scrollbars=1,width=1270,height=928,menubar=0,toolbar=1,"
           "location=1,status=1,fullscreen=1');hkwin.focus();return false;\""
           " target=\"HK\"><b>Start</b></a></td>\n");
*/

    printf("</tr></table>\n</td></tr></table>\n");
}

void show_javascript_periodic_reload(void)
{
    printf("<script language=\"javascript\">\n");
    printf("setTimeout(\"window.location.reload()\",2000)\n");
    printf("</script>\n");
}

void show_shmerror(void)
{
    printf("<p><b>ERROR semaphore:semget: %s</b><br>\n",strerror(errno));
    printf("<p><em>Is the Housekeeping server 'srv_hk' running?</em><br>\n");
    show_javascript_periodic_reload(); /* Try again */
    show_trailer();
}
/*
void show_TSCU_data(int board, struct HK *shm, int display_raw)
{
    int i;
    struct timeval ts;

    printf("<table border=1>\n");
    printf("<tr><td align=center "
             "style='background:#d0d0f0'>TSCU:%s</td></tr>\n",
             shm->tscu.b[board].name);

    printf("<tr><td>\n");

    for(i=0;i<shm->tscu.b[board].nr_par;i++) {
        time_t lt = shm->tscu.b[board].par[i].tstamp;
        gettimeofday(&ts,NULL);

        if(shm->tscu.b[board].par[i].stripattr > 0) {
              printf("<table border=1 cellspacing=0 cellpadding=2 style='font-size:80%%'>\n");
              printf("\t\t<td style='background:#eeeeee'>name: \n");
              printf("<b>%s </b> ",shm->tscu.b[board].par[i].name);
              printf(" [%s]</td>",shm->tscu.b[board].par[i].unit);

              printf("\t<tr>\n");
              printf("\t\t<td><img id='b%dimg%d' src='" URL_SERVER "/fig/bl.gif'> "
                     "</td>",board,i);
              printf("\t</tr>\n");
              printf("</table>\n");
        }
    }
    printf("</td></tr></table>\n");
}
*/

int main(int argc,char **argv)
{
    show_header();
    show_ajaxroutines(1,5);

    show_title();

    printf("<table align=right  border=0 cellpadding=0>\n");
    printf("<tr><td align=right>\n");
    printf("</td><td align=right>\n");
    show_legend();
    printf("</td><td align=right>\n");
    show_buttonbar();
    printf("</td></tr></table>\n");

    printf("<table align=left width=100%% border=0>\n");
    printf("<tr><td align=left>\n");
    printf("<b>Playing | Systime(UTC):</td><td id='systime'></td></b>\n");
    printf("</tr><tr><td align=left>"
           "<b>Last HK Time:</td><td align='left' id='hktime'></td></b></tr>\n");

    printf("<tr><td align=right>Refresh: ");
        printf("<a href=\"" CGIBIN "/%s?dir\" "
               "ALT=\"Switch OFF\">DIR</a>\n",HKSNEW);
    printf("</td></tr></table>\n");
    printf("</td></tr></table>\n");


    printf("<table width=100%% border=1><tr><td style='background:#e5E5f5'>\n");


    /* TSCU Tables: */
    printf("<table width=100%% border=0 cellpadding=2>\n<tr>"
           "<td valign=top align=center>\n\n");
//    show_TSCU_data(1,shm,display_raw_flag);
    printf("\n</td><td valign=top align=center>\n\n");
//    show_TSCU_data(2,shm,display_raw_flag);
    printf("\n</td><td valign=top align=center>\n\n");
//    show_TSCU_data(3,shm,display_raw_flag);
    printf("\n</td><td valign=top align=center>\n\n");
 //   show_TSCU_data(4,shm,display_raw_flag);
    printf("\n</td><td valign=top align=center>\n\n");
  //  show_TSCU_data(5,shm,display_raw_flag);
    printf("\n</td></tr></table>\n\n");


    printf("</td></tr></table>\n");

    show_trailer();
    return 0;
}

