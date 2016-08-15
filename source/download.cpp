#include "libs.h"
#include "zip.h"
Result http300(char* nurl, char* e)
{ 
    //printf("\x1b[2J");
    Result ret = 0;
    u8* buf;
    char* d = strrchr(nurl, '/');
    u32 size = 0;
    u32 statuscode = 0;
    httpcContext context;
    printf("Download:%s\n ", nurl);
	
	ret = httpcOpenContext(&context, HTTPC_METHOD_GET, nurl, 0);
    if (ret != 0)
        return ret;
    printf("return from httpcOpenContext: %" PRId32 "\n", ret);
    ret = httpcAddRequestHeaderField(&context, (char*)"User-Agent", (char*)"MULTIDOWNLOAD");
    if (ret != 0)
        return ret;
    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0)
        return ret;
    ret = httpcBeginRequest(&context);
    if (ret != 0)
        return ret;
    ret = httpcGetResponseStatusCode(&context, &statuscode, 0);
    if (ret != 0)
        return ret;
    printf(" Statuscode:%" PRId32 "\n ", statuscode);
    if (statuscode == 301 || statuscode == 302) {
        printf(" 300\n ");
        char newurl[1024];
        httpcGetResponseHeader(&context, (char*)"Location", newurl, 1024);
        httpcCloseContext(&context);
        return http300(newurl, e);
    }
    if (statuscode != 200)
        return -2;
    ret = httpcGetDownloadSizeState(&context, NULL, &size);
    if (ret != 0)
        return ret;
    printf(" size: %" PRId32 "\n ", size);
    gfxFlushBuffers();
    buf = (u8*)malloc(size);
    if (buf == NULL)
        return -1;
    memset(buf, 0, size);
    ret = httpcDownloadData(&context, buf, size, NULL);
    Handle fileHandle;
    u32 bytesWritten;
    if(!(mkdir(e , 0777)))
		printf("Path didn't exist made one with success\n");
	else 
	{   if (ENOENT == errno)
		{printf("error with path\n");
		}
	     if (EEXIST == errno)
		 { printf("exists\n");
	   }			 
	} 
    strcat(e, d);
    FILE* fptr = fopen(e, "wb");
    fwrite(buf, 1, size, fptr);
    fclose(fptr);
	
	  //free(buf);
    if (ret != 0) {
        free(buf);
		httpcCloseContext(&context);
        printf("Error while Downloading\n");
        return ret;
	}
	else
    {   printf("Success");
		char *ext =strrchr(d , '.');
        if (ext) {
        printf("extension is %s\n", ext + 1);
        }
        if(strcmp(ext + 1,"zip") == 0) {
          printf("It worked\n");
		  sdmcInit();
		  fsInit();
		  char *a="/3ds/";
		  ret = ezip(e);
		  if (ret==0)
		    {   
	          printf("Extracted\n");
		      sdmcExit();
		      fsExit();
		    }
		
	    }
	}
    return 0;
}
