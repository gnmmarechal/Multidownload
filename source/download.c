#include "libs.h"
#include "zip.h"
Result make(u8* buf,u32* size,char*loca)
{
FILE* fptr = fopen(loca, "wb");
      fwrite(buf, 1, size, fptr);
      fclose(fptr);
	  return 0;
	}

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
	{printf("error 0x%08X",ret);
	return ret;
	}
    
    printf("return from httpcOpenContext: %" PRId32 "\n", ret);
    ret = httpcAddRequestHeaderField(&context, (char*)"User-Agent", (char*)"MULTIDOWNLOAD");
    if (ret != 0){
		  printf("error in ARHF 0x%08X",ret);
		  return ret;
         }
        
    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0){
		  printf("error in SSLO 0x%08X",ret);
		  return ret;
         }
    ret = httpcBeginRequest(&context);
    if (ret != 0){
		  printf("error in HPR 0x%08X",ret);
		  return ret;
         }
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
    if (ret != 0){
		 printf("error in HGDSS 0x%08X",ret);
		  return ret;
         }
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
     if (ret!=0)
	 {
		 printf("\nDownload failed\n");
		 httpcCloseContext(&context);
		 return ret;
     }
    Handle fileHandle;
    u32 bytesWritten;
	lastbuf = buf;
	buf = realloc(buf, size);
	printf("\n");
	printf("size : %d \n",size);
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
	    return -1;
		}
 	 char  f[1000];
	 const char *git=  "https://github-cloud.s3.amazonaws.com";
	 const char *b = ".3dsx";
	 const char *k ="/";
	 const char *git1="filename%3D";
	 const char *git2="&";
	 const char *gba="gbatemp.net";
	 char *target =NULL;
	 char *end ,*start;
	 if(strstr(nurl,b)!=NULL)
	 { printf("got 3dsx\n");
      if(start=strstr(nurl,k))
	  {
		  start +=strlen(k);
		  if (end= strstr(start,b))
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
		   char* g = strrchr(target, '/');
		  char a[100]="/3ds";
		  strcat(a,g);
		  strcat(a,d);
          ret = make(buf, size,a);
		  if (ret==0)
			  printf("%s\n",a);
			  printf("success\n");
		      free(target);
		  }
		
	else if(strstr(nurl,git)!=NULL)
	{printf("got git\n");
     
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
          ret = make(buf, size,loca);
		  free(target);
		  char *exton =strrchr(f , '.');
          if(strcmp(exton + 1,"zip") == 0) {
          printf("zip file\n");
		  sdmcInit();
		  fsInit();
		  ret = ezip(loca);
		  if (ret==0)
		    {   
	          printf("Extracted\n");
		      sdmcExit();
		      fsExit();
		    }
		  }
	    }
		else if (strstr(nurl,gba)!=NULL)
		{ 
	    printf("Downloading from gbatemp.net\n");
		
	    char *ext =strrchr(nurl, '.');
        if (ext) {
        int len = strlen(ext);
		char reloc[1024];
		memcpy(reloc, nurl, strlen(nurl) - len);
        reloc[strlen(nurl) - len] = '\0';
		char *ex=strrchr(reloc,'/');
		printf("filename now is %s\n",ex);
		   
		char *r1=strrchr(ex,'-');
        r1=r1+1;		
		
		char *ext2 =strrchr(ex, '-');
        if (ext2) {
        int len = strlen(ext2);
		char reloc2[1000];
		printf("ext2 is %s\n", ext2);
		printf("r1 is %s\n",r1);
		memcpy(reloc2, ex, strlen(ex) - len);
        reloc2[strlen(ex) - len] = '\0';
		printf("reloc2 is %s\n",reloc2);
		strcat(reloc2,".");
		strcat(reloc2,r1);
		strcat(loca,reloc2);
		printf("Correct location is: %s\n",loca);
	    ret=make(buf , size ,loca);
		free (target);
        if(strcmp(r1 ,"zip") == 0) {
          printf("Zip file\n");
		  sdmcInit();
		  fsInit();
		  ret = ezip(loca);
		  if (ret==0)
		    {   
	          printf("Extracted\n");
		      sdmcExit();
		      fsExit();
		    }
		   }
		  }
		 }
		}		 
	  else
	  {
	   printf("not any special format\n");
       printf("Filename will be (excluding the '/') %s\n",d);
	   strcat(loca, d);
	   printf("location is %s\n",loca);
       ret = make(buf , size, loca);
	   char *exton =strrchr(d , '.');
        if(strcmp(exton + 1,"zip") == 0) {
          printf("Zip file\n");
		  sdmcInit();
		  fsInit();
		  ret = ezip(loca);
		  if (ret==0)
		    {   
	          printf("Extracted\n");
		      sdmcExit();
		      fsExit();
		    }
		  
	    }
		
	}
    httpcCloseContext(&context);
	free(buf);
    return 0;
}
