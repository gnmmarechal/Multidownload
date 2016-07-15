#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "download.h"

int main() {
		SwkbdState swkbd;
		char mybuf[960];
		SwkbdButton button = SWKBD_BUTTON_NONE;
		bool didit = false;
                              //Result ret=1;
		
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
			button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
			
                                                httpcInit(0);
                                                Result ret=1;			
			if ((didit) && button != SWKBD_BUTTON_NONE)
		                ret=http300(mybuf);
                                                if(ret==0)
                                               {printf("Downloaded/n");
                                             }
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
