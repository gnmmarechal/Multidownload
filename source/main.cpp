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

int main() {
		SwkbdState swkbd;
		char mybuf[960];
		SwkbdButton button = SWKBD_BUTTON_NONE;
		bool didit = false;
		
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("MultiDownload by Kartik\n");
	printf("Press A to begin\n");
	printf("Press START to exit\n");
                
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_A)
		{
			didit = true;
			swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
			swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			swkbdSetFilterCallback(&swkbd, MyCallback, NULL);
			button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
			
            httpcInit(0);			
			if ((didit) && button != SWKBD_BUTTON_NONE)
				http300(mybuf);
			httpcExit();
			
			//cleaning vars
			for (int i = 0; i < 960; i++)
				mybuf[i] = ' ';
			button = SWKBD_BUTTON_NONE;
			didit = false;
			
			printf("\x1b[2J");
			printf("MultiDownload by Kartik\n");
			printf("Press A to begin\n");
			printf("Press START to exit\n");
		}
		
		if (kDown & KEY_START)
			break;

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
                
	gfxExit();
	return 0;
}
