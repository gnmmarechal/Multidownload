#include "libs.h"
#include "zip.h"
#include <sys/time.h>
#include "mega.h"
#include "fs.h"
#include "cia.h"
u64 top=3000000000;


Result http300(char *nurl);
Result http_down(char* nurl)
{ 
	 char* d = strrchr(nurl, '/');
    Result ret = 0;
    u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
    u8 *buf, *lastbuf;
    httpcContext context;
    printf("\x1b[33;1mDownloading:%s\n\x1b[37;1m", nurl);
	int ds = 0;
	ret = httpcOpenContext(&context, HTTPC_METHOD_GET, nurl, 0);
    if (ret != 0)
	{printf("\x1b[31;1merror 0x%08X\n",(int)ret);
	return 1;
	}
    ret = httpcAddRequestHeaderField(&context, (char*)"User-Agent", (char*)"MULTIDOWNLOAD");
    if (ret != 0){
		 printf("\x1b[31;1merror in ARHF 0x%08X\n",(int)ret);
		  return 1;
         }
    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0){
		 printf("\x1b[31;1merror in SSLO 0x%08X\n",(int)ret);
		  return 1;
         }
    ret = httpcBeginRequest(&context);
    if (ret != 0){
		 printf("\x1b[31;1merror in HPR 0x%08X\x1b[37;1m\n",(int)ret);
		  return 1;
         }
	ret = httpcGetResponseStatusCodeTimeout(&context, &statuscode,6000000000);
    if (ret != 0)
	{printf("Wrong protocol:%s\n",nurl);
    return -1;
	}
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
		 printf("\x1b[31;1merror in HGDSS 0x%08X\x1b[37;1m\n",(int)ret);
		  return 1;
         }
    printf("size(may be wrong): %" PRId32 "\n ",contentsize);
    gfxFlushBuffers();
	gfxSwapBuffers();
    buf = (u8*)malloc(0x1000);
    if (buf == NULL)
	{
		printf("\x1b[31;1merror while setting buffer\n\x1b[37;1m");
        return -1;
    //memset(buf, 0, size);
	}
	if(!(mkdir(loca , 0777)))
		printf("\x1b[32;1 Path made with success\n\x1b[37;1m");
	else 
	{   if (ENOENT == errno)
		{printf("\x1b[31;1merror with path\n\x1b[37;1m");
		}
	     if (EEXIST == errno)
		 { printf("exists\n");
	   }			 
	} 

    do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize;
		printf("Downloaded %d bytes out of %d bytes\r",(int)size,(int)contentsize);
		
		
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
			   
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = realloc(buf, size + 0x1000);
				if(buf==NULL){ 
					httpcCloseContext(&context);
					free(lastbuf);
				    return -2;
					}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);	
     if (ret!=0)
	 {
		 httpcCloseContext(&context);
		 return 1;
     }
	lastbuf = buf;
	buf = realloc(buf, size);
	printf("\n");
	printf("size : %d \n",(int)size);
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
	    return 1;
		}
	 const char *git=  "https://github-cloud.s3.amazonaws.com";
	 const char *git1="filename%3D";
	 const char *git2="&";
	 const char *gba="gbatemp.net";
	 char *target =NULL;
	 char *end ,*start;
	if(strstr(nurl,git)!=NULL)
	{  printf("got git\n");
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
			  printf("\x1b[31;1merror\n\x1b[37;1m");
	        }
			else
			printf("\x1b[31;1merror\n\x1b[37;1m");
		  if (target)printf("Filename is %s\n",target);
		 char f[1024]="/";
		  strcat (f , target);
		  strcat(loca,f);
		  printf("location is %s\n",loca);
          ret = make(buf, size,loca);
		  free(target);
		  if(strstr(nurl,".3dsx")!=NULL)
	 {  
          printf("got 3dsx\n");
          printf("%s\n",f);
		  char nu[1000];
		  strcpy(nu,target);
	      int len = strlen(target);
          nu[len-5] = '\0';
		  char f[1024]="/3ds/";
		  strcat(f,nu);
		  if(!(mkdir(f , 0777)))
		  printf("\x1b[32;1 Path made with success\n\x1b[37;1m");
	      else 
	     {   
          if (ENOENT == errno)
		 {
			printf("\x1b[31;1merror with path\n\x1b[37;1m");
		 }
	     if (EEXIST == errno)
		 { printf("exists\n");
	     }			 
	} 
			strcat(f,"/");
			strcat(f,target);
			
			ret = make(buf, size,f);
		  if (ret==0)
		    {   
			  printf("success\n");
		      ds=1;
			  return 0;
			}
	    }
		  char *exton =strrchr(f , '.');
          if(strcmp(exton + 1,"zip") == 0) {
          printf("zip file\n");
		  sdmcInit();
		  fsInit();
		  ret = ezip(loca);
		  if (ret==0)
		    {   
	          printf("\x1b[32;1mExtracted\n\x1b[37;1m");
		      sdmcExit();
		      fsExit();
		    }
		  }
	    
		else if(ds==0){
	   if(strstr(nurl,".3dsx")!=NULL)
	   { printf("got 3dsx\n");
	   ret = dsx(nurl,buf,size);
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
	          printf("\x1b[31;1mExtracted\n\x1b[37;1m");
		      sdmcExit();
		      fsExit();
		    }
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
	          printf("\x1b[32;1mExtracted\n\x1b[37;1m");
		      sdmcExit();
		      fsExit();
		    }
		  
	    }
	  
	}
    httpcCloseContext(&context);
	free(buf);
    return 0;
}
Result http300(char *nurl)
{  
   Result ret=0;
   if (strstr(nurl,"mega")!=NULL)
   {
	   ret=doMegaInstall(nurl);
	   
        
	   if (ret!=3)
	   return 1;
       else {
		   char *exton =strrchr(loca, '.');
		   if(strcmp(exton + 1,"zip") == 0) {
          printf("Zip file\n");
		  sdmcInit();
		  fsInit();
		  ret = ezip(loca);
		  if (ret==0)
		    {   
	          printf("\x1b[32;1mExtracted\n\x1b[37;1m");
		      sdmcExit();
		      fsExit();
		    }
		  
	    }
	   
     return 0; }
   }
   if(strstr(nurl,"http")!=NULL)
   {
	  ret= http_down(nurl);
   }
   else 
   {
   char a[1024]="https://";
   strcat(a,nurl);
   ret=http_down(a);
   if (ret==-1)
   {char b[1024]="http://";
	strcat(b,nurl);
    ret=http_down(b);	
	 } 
   }
  memset(loca, 0, 1024);   //solve error not the correct way to solve this but works.
  extern char buffer[1024];
  strcpy(loca,buffer);
  if (ret!=0)
  {
	  return 1;
  }
    
  return 0;
}
