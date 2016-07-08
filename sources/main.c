#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "hbkb.h"
#include <3ds.h>

Result http_download(httpcContext *context, const char* url)//This error handling needs updated with proper text printing once ctrulib itself supports that.
{
	Result ret=0;
	u32 statuscode=0;
	u32 size=0, contentsize=0;
	u8 *buf;

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
			FS_Archive sdmcArchive=(FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1,                                               (u8*)""}};
			FS_Path filePath=fsMakePath(PATH_ASCII,strrchr(url, '/'));
			FSUSER_OpenFileDirectly( &fileHandle, sdmcArchive, filePath, FS_OPEN_CREATE|                                                FS_OPEN_WRITE, 0x00000000);
			FSFILE_Write(fileHandle, &bytesWritten, 0, buf, contentsize,0x10001);
			FSFILE_Close(fileHandle);
			svcCloseHandle(fileHandle);
			free(buf);
                                             if(ret!=0)
	{
		free(buf);
		return ret;
	}

	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();

	free(buf);

	return 0;
}

int main(int argc, char **argv)
{
	Result ret=0;
	httpcContext context;
        HB_Keyboard sHBKB;
	gfxInitDefault();
	httpcInit();

	consoleInit(GFX_TOP,NULL);

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		// Your code goes here

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; 
                touchPosition touch;

		//Read the touch screen coordinates
		
                hidTouchRead(&touch);

		// Call Keyboard with Touch Position
		
                u8 KBState = sHBKB.HBKB_CallKeyboard(touch);
		
		// Print Input
		
                std::string InputHBKB = sHBKB.HBKB_CheckKeyboardInput(); // Check Input
		
                 char* gba= const_cast<char*>(InputHBKB.c_str());
                  //printf("Downloading %s\n",gba);

                
                if (KBState == 1) // User finished Input
                 {printf("Downloading %s\n",gba);		
                ret = httpcOpenContext(&context,gba, 1);
	printf("return from httpcOpenContext: %"PRId32"\n",ret);
	gfxFlushBuffers();

	if(ret==0)
	{
		ret=http_download(&context , gba);
		printf("return from http_download: %08"PRIx32"\n",ret);
		gfxFlushBuffers();
		httpcCloseContext(&context);
	}
			
                   sHBKB.HBKB_Clean();
                 
                       }
		
                else if (KBState == 3)
		
               {
			
                sHBKB.HBKB_Clean();
                 break;
                }     // Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	// Exit services
	httpcExit();
	gfxExit();
	return 0;
}
