#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cybertrust.h"
#include "digicert.h"
//extern char *mybuf;
Result http_download(httpcContext *context , const char *url)
{                       
                                             httpcInit(0);
                                             //Result ret=0;
                                             gfxInitDefault();
                                             //httpcContext context;
                                             consoleInit(GFX_TOP, NULL);
                                                                      Result ret=0;
                                                                      u32 statuscode=0;
                                                                      u32 size=0, contentsize=0;
				      u8 *buf;
                			      char *a=strrchr(url, '/');
                  		                      char *b=strrchr(a ,'.');
	httpcAddRequestHeaderField(context, (char*)"User-Agent",  (char*)"MULTIDOWNLOAD");
                httpcAddTrustedRootCA(context, cybertrust_cer, cybertrust_cer_len);
	httpcAddTrustedRootCA(context, digicert_cer, digicert_cer_len);
	                                                      ret = httpcBeginRequest(context);
	                                                      if(ret!=0)return ret;
                                                                      ret = httpcGetResponseStatusCode(context, &statuscode, 0);
			                      if(ret!=0)return ret;
                			      if(statuscode!=200)return -2;
                			      ret=httpcGetDownloadSizeState(context, NULL, &contentsize);
				      if(ret!=0)return ret;

	                                                     printf("size: %08"PRIx32"\n",contentsize);
	                                                     gfxFlushBuffers();
 
	                                                    buf = (u8*)malloc(contentsize);
	                                                    if(buf==NULL)return -1;
				    memset(buf, 0, contentsize);
                
	                                                     ret = httpcDownloadData(context, buf, contentsize, NULL);
                                                                     Handle fileHandle;
			                      u32 bytesWritten;
                                              //strcat (c,a);
			                           FS_Path filePath=fsMakePath(PATH_ASCII,a);
                                                                           FSUSER_OpenFileDirectly(&fileHandle, FS_ArchiveID::ARCHIVE_SDMC,
                                                                           fsMakePath(FS_PathType::PATH_EMPTY, ""),
                                                                           filePath, FS_OPEN_CREATE|   FS_OPEN_WRITE,0x00000000 );
			                           FSFILE_Write(fileHandle, &bytesWritten, 0, buf, contentsize,0x10001);
			                           FSFILE_Close(fileHandle);
			                           svcCloseHandle(fileHandle);
			//free(buf);
                                                                        if(ret!=0)
	                                                    {
		                                      free(buf);
		                                     return ret;
	                                                        }

	                                                  gfxFlushBuffers();
	                                                  gfxSwapBuffers();
	                                                  gspWaitForVBlank();

	                                                  free(buf);

	                                                   httpcExit();
	                                                      gfxExit();
	                                                     return 0 ;
                                                                     }
