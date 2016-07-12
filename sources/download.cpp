#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
//extern char* newurl;
Result http300(char*nurl)
{
  httpcInit(0);
  gfxInitDefault();
  consoleInit(GFX_TOP, NULL);
   Result ret=0;
   u8* buf;
   char *d=strrchr(nurl, '/');
   u32 size=0;
   u32 statuscode=0;
   httpcContext context;
   printf("Download:%s\n",nurl);
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
      printf("Statuscode:%"PRId32"\n",statuscode);
       if(statuscode==301||statuscode==302)
      { printf("300\n");
        char newurl[1024];
        httpcGetResponseHeader(&context, (char*)"Location", newurl,1024);
        httpcCloseContext(&context);
        return http300(newurl);
                      }  
   if(statuscode!=200)return -2;
   ret=httpcGetDownloadSizeState(&context, NULL, &size);
       if(ret!=0)return ret;
     printf("size: %"PRId32"\n",size);
       gfxFlushBuffers();
     buf = (u8*)malloc(size);
       if(buf==NULL)return -1;
     memset(buf, 0, size);
     ret = httpcDownloadData(&context, buf, size, NULL);
             Handle fileHandle;
             u32 bytesWritten;
                                              //strcat (c,a);
             FS_Path filePath=fsMakePath(PATH_ASCII,d);
             FSUSER_OpenFileDirectly(&fileHandle, FS_ArchiveID::ARCHIVE_SDMC,
             fsMakePath(FS_PathType::PATH_EMPTY, ""),
             filePath, FS_OPEN_CREATE|FS_OPEN_WRITE,0x00000000 );
             FSFILE_Write(fileHandle, &bytesWritten, 0, buf, size,0x10001);
             FSFILE_Close(fileHandle);
             svcCloseHandle(fileHandle);
			//free(buf);
                                                                        if(ret!=0)
	                                                    {
		                                      free(buf);
                                                                     httpcCloseContext(&context);
		                                      printf("Downloaded:%s\n",nurl);
	                                                          }

	                                                  gfxFlushBuffers();
	                                                  gfxSwapBuffers();
	                                                  gspWaitForVBlank();
                                                                 
	                                                  free(buf);

	                                                   httpcExit();
	                                                      gfxExit();
	                                                     return 0 ;
                                                                     }
