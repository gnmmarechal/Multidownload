#include "libs.h"
#include "zip.h"
Result http300(char* nurl)
{ 
	char* d = strrchr(nurl, '/');
    Result ret = 0;
    u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
    u8 *buf, *lastbuf;
    httpcContext context;
    printf("Download:%s\n", nurl);
	
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
    printf("Statuscode:%" PRId32 "\n", statuscode);
    if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
        printf("300\n");
        char newurl[1024];
        httpcGetResponseHeader(&context, (char*)"Location", newurl, 1024);
        httpcCloseContext(&context);
        return http300(newurl);
    }
    if (statuscode != 200)
        return -2;
    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0)
        return ret;
    printf("size(may be wrong): %" PRId32 "\n ",contentsize);
    gfxFlushBuffers();
	gfxSwapBuffers();
    buf = (u8*)malloc(0x1000);
    if (buf == NULL)
        return -1;
    //memset(buf, 0, size);
	
	if(!(mkdir(loca , 0777)))
		printf("Path didn't exist made one with success\n");
	else 
	{   if (ENOENT == errno)
		{printf("error with path\n");
		}
	     if (EEXIST == errno)
		 { printf("exists\n");
	   }			 
	} 
   
    
	do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize;
		
					printf("Downloaded %d bytes out of %d bytes\r",size,contentsize);
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
			   
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = realloc(buf, size + 0x1000);
				if(buf==NULL){ 
					httpcCloseContext(&context);
					free(lastbuf);
				    return -1;
					}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);	

    Handle fileHandle;
    u32 bytesWritten;
	lastbuf = buf;
	buf = realloc(buf, size);
	printf("\n");
	printf("size : %d             \n",size);
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
	    return -1;
		}
 	 char  f[1000];
	 const char *git=  "https://github-cloud.s3.amazonaws.com";
	if(strstr(nurl,git)!=NULL)
	{printf("got git\n");
     const char *git1="filename%3D";
	 const char *git2="&";
	 char *target =NULL;
	 char *end ,*start;
	  if(start=strstr(nurl,git1))
	  {
		  start +=strlen(git1);
		  if (end= strstr(start,git2))
		  {
		  target = (char*)malloc(end-start+1);
		  memcpy(target,start,end-start);
		  target[end-start]= '\0';
		  }
		  else 
			  printf("error\n");
	        }
			else
			printf("error\n");
		  if (target)printf("Filename is %s\n",target);
		 char f[1024]="/";
		  strcat (f , target);
		  strcat(loca,f);
		  printf("location is %s\n",loca);
          FILE* fptr = fopen(loca, "wb");
          fwrite(buf, 1, size, fptr);
          fclose(fptr);
		  }
	  else
	  {
	   printf("not git\n");
       printf("Filename will be (excluding the '/') %s\n",d);
	   strcat(loca, d);
	   printf("location is %s\n",loca);
       FILE* fptr = fopen(loca, "wb");
       fwrite(buf, 1, size, fptr);
       fclose(fptr);
	  }
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
        //printf("extension is %s\n", ext + 1);
        }
        if(strcmp(ext + 1,"zip") == 0) {
          printf("It worked\n");
		  sdmcInit();
		  fsInit();
		  char *a="/3ds/";
		  ret = ezip(loca);
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
