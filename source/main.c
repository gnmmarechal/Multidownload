#include "libs.h"
#include <3ds/services/hid.h>
#include "download.h"
char loca[1024];
char buffer[1024];
void readcfg()
{ FILE* file = fopen("multi.cfg", "rb");
    if (file == NULL) {
        printf("Downloading to the root of the sdmc\n");
        fclose(file);
        //didloc = true;
    }
    else {
        fseek(file, 0, SEEK_END);
        off_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        //if(!buffer)
        //printf("error while allocating buffer\n");
        off_t bytesRead = fread(buffer, 1, size, file);

        fclose(file);
        if (size != bytesRead)
            printf("error");
		  strcpy(loca,buffer);
    }
	
}
PrintConsole top,bottom;
int main()
{   touchPosition touch;
    SwkbdState swkbd;
    char mybuf[960];
    //char loca[1024];
    SwkbdButton button = SWKBD_BUTTON_NONE;
    bool didit = false;
    bool didloc = false;
    
    //Result ret=1;
    gfxInitDefault();
    consoleInit(GFX_BOTTOM,&bottom);
    consoleInit(GFX_TOP, &top);
	consoleSelect(&top);
   
	readcfg();
    printf("          MultiDownload by Kartik\n");
	printf("              Version x.2\n");
    printf("           Select an option\n");
    printf("          Press START to exit\n");
    printf("Will download to location %s\n", buffer);
	consoleSelect(&bottom);
	printf("          EDIT DOWNLOAD LOCATION\n");
	printf("\n\n\n\n");
	printf("              ENTER A URL\n");
    //strcpy(loca, buffer);
    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();
		hidTouchRead(&touch);
		consoleSelect(&top);
        if (kDown & KEY_TOUCH) {
			if(touch.py < 030 && touch.px < 240) {
            didloc = true;
            const char* texgen = "Enter download to location";
            swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
            swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
            swkbdSetHintText(&swkbd, texgen);
            button = swkbdInputText(&swkbd, loca, sizeof(loca));
            if (buffer[0] == '\0') {
                strcpy(buffer, loca);
            }
            if ((didloc) && button != SWKBD_BUTTON_NONE)
                button = SWKBD_BUTTON_NONE;
            didit = false;
            FILE* fp;
            fp = fopen("multi.cfg", "wb");
            fprintf(fp, loca);
            fclose(fp);
            printf("Location changed to %s\n", loca);
        }
        if (touch.py < 060&& touch.py >030&& touch.px <240 ) {
            didit = true;
            const char* texgen2 = "Enter download url";
            swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
            swkbdSetHintText(&swkbd, texgen2);
            swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
            button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
            httpcInit(0);
            Result ret = 1;
            if ((didit) && button != SWKBD_BUTTON_NONE)
			{
                ret = http300(mybuf);
			
            if (ret == 0) {
                printf("Downloaded\n");
            }
            httpcExit();
            //cleaning vars
            for (int i = 0; i < 960; i++)
                mybuf[i] = ' ';
            button = SWKBD_BUTTON_NONE;
            didit = false;
			}
        }
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
