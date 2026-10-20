#include "service_handler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int handle_service(const char *in, char *out) {
    if (!in || !out) return STATUS_ERROR_SYNTAX;
    char cmd[16] = {0}; const char *a = strchr(in, ' '); size_t n = a ? (size_t)(a-in) : strlen(in);
    if (n >= sizeof(cmd)) return STATUS_ERROR_SYNTAX; memcpy(cmd, in, n); a = a ? a+1 : "";
    if (!strcmp(cmd,"UPPER")) { snprintf(out,MAX_PAYLOAD_SIZE,"%s",a); for(char *p=out;*p;p++) *p=(char)toupper((unsigned char)*p); return 0; }
    if (!strcmp(cmd,"REVERSE")) { snprintf(out,MAX_PAYLOAD_SIZE,"%s",a); n=strlen(out); for(size_t i=0;i<n/2;i++){char t=out[i];out[i]=out[n-i-1];out[n-i-1]=t;} return 0; }
    if (!strcmp(cmd,"STATS")) { size_t words=0; int w=0; for(const char *p=a;*p;p++){if(isspace((unsigned char)*p))w=0;else if(!w){words++;w=1;}} snprintf(out,MAX_PAYLOAD_SIZE,"length=%zu words=%zu",strlen(a),words); return 0; }
    if (!strcmp(cmd,"CALC")) { double x,y; char op; if(sscanf(a,"%lf %c %lf",&x,&op,&y)!=3)return 1; if(op=='/'&&y==0)return 2; double z=op=='+'?x+y:op=='-'?x-y:op=='*'?x*y:x/y; snprintf(out,MAX_PAYLOAD_SIZE,"%g",z); return 0; }
    snprintf(out,MAX_PAYLOAD_SIZE,"unknown command"); return STATUS_ERROR_UNKNOWN;
}
