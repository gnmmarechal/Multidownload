#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "hbkb.h"
#include <3ds.h>

Result http_download(httpcContext *context)//This error handling needs updated with proper text printing once ctrulib itself supports that.
{
	Result ret=0;
	u8* framebuf_top;
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
	if(ret!=0)
	{
		free(buf);
		return ret;
	}

	size = contentsize;
	if(size>(240*400*3*2))size = 240*400*3*2;

	framebuf_top = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	memcpy(framebuf_top, buf, size);

	gfxFlushBuffers();
	gfxSwapBuffers();

	framebuf_top = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

	memcpy(framebuf_top, buf, size);

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
                 		
               { ret = httpcOpenContext(&context,gba, 1);
	printf("return from httpcOpenContext: %"PRId32"\n",ret);
	gfxFlushBuffers();

	if(ret==0)
	{
		ret=http_download(&context);
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
