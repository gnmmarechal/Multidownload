#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "download.h"
static SwkbdCallbackResult MyCallback(void* user, const char** ppMessage, const char* text, size_t textlen)
{
	if (strstr(text, "lenny"))
	{
		*ppMessage = "Nice try but I'm not letting you use that meme right now";
		return SWKBD_CALLBACK_CONTINUE;
	}

	if (strstr(text, "brick"))
	{
		*ppMessage = "~Time to visit Brick City~";
		return SWKBD_CALLBACK_CLOSE;
	}

	return SWKBD_CALLBACK_OK;
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
                Result ret=0;
                httpcInit(0);
	httpcContext context;
	printf("MultiDownload by Kartik\n");
	printf("Press A to begin\n");
	printf("Press START to exit\n");
                
	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
		{	break;
                                     }
		static SwkbdState swkbd;
		static char mybuf[960];
		SwkbdButton button = SWKBD_BUTTON_NONE;
		bool didit = false;


		if (kDown & KEY_A)
		{
			didit = true;
			swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
			swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			swkbdSetFilterCallback(&swkbd, MyCallback, NULL);
			button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
		}

		if (didit)
		{
			if (button != SWKBD_BUTTON_NONE)
			{
				printf("Download: %s\n", mybuf);
                                                                ret = httpcOpenContext(&context,HTTPC_METHOD_GET,mybuf, 1);
	                                                printf("\x1b[5;0Hreturn from httpcOpenContext: %"PRId32"\n",ret);
	                                                gfxFlushBuffers();
                                                                if(ret==0)
	                                                    {
		                                       ret=http_download(&context , mybuf);
		                                       printf("return from http_download: %08"PRIx32"\n",ret);                                                                                                             printf("Downloaded:%s\n",mybuf);
		                                       gfxFlushBuffers();
		                                       httpcCloseContext(&context);
                                                           }
                                                                
			} 
		}
                         
		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();
	}
                httpcExit();
	gfxExit();
	return 0;
}
