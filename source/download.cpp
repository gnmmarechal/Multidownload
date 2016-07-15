#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
//extern char* newurl;
Result http300(char*nurl)
{

   Result ret=0;
   u8* buf;
   char *d=strrchr(nurl, '/');
    char *a=strrchr(d,'.');
   u32 size=0;
   u32 statuscode=0;
   httpcContext context;
   printf( "Download:%s\n ",nurl);
   ret = httpcOpenContext(&context, HTTPC_METHOD_GET, nurl, 0);
        if(ret!=0)return ret;
    printf("return from httpcOpenContext: %"PRId32"\n",ret);
   ret=httpcAddRequestHeaderField(&context, (char*)"User-Agent",  (char*)"MULTIDOWNLOAD");
        if(ret!=0)return ret;                            
   ret = httpcSetSSLOpt(&context, 1<<9);
        if(ret!=0)return ret;
   ret = httpcBeginRequest(&context);
        if(ret!=0)return ret;
   ret = httpcGetResponseStatusCode(&context, &statuscode, 0);
        if(ret!=0)return ret;
      printf(" Statuscode:%"PRId32"\n ",statuscode);
       if(statuscode==301||statuscode==302)
      { printf(" 300\n ");
        char newurl[1024];
        httpcGetResponseHeader(&context, (char*)"Location", newurl,1024);
        httpcCloseContext(&context);
        return http300(newurl);
                      }  
   if(statuscode!=200)return -2;
   ret=httpcGetDownloadSizeState(&context, NULL, &size);
       if(ret!=0)return ret;
     printf(" size: %"PRId32"\n ",size);
       gfxFlushBuffers();
     buf = (u8*)malloc(size);
       if(buf==NULL)return -1;
     memset(buf, 0, size);
     ret = httpcDownloadData(&context, buf, size, NULL);
             Handle fileHandle;
             u32 bytesWritten;
                                              //strcat (c,a);
                FILE *fptr = fopen(d, "wb");
	fwrite(buf, 1, size, fptr);
	fclose(fptr);
			//free(buf);
                                                                        if(ret!=0)
	                                                    {
		                                      free(buf);
                                                                     httpcCloseContext(&context);
		                                      //printf(" Downloaded:%s\n ",nurl);
                                                                      return ret;
	                                                          }
	                                                     return 0 ;
                                                                     }
