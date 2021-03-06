#include "libs.h"
#include <3ds/services/hid.h>
#include "download.h"
#include "qrdec.h"
#include "quirc.h"

u32 __stacksize__ =0x40000;

char loca[1024];
char buffer[1024];
void readcfg()
{ 
    FILE* file = fopen("multi.cfg", "rb");
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
   void clearScreen(void) {
	u8 *frame = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	memset(frame, 0, 320 * 240 * 3);
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
     hidInit();
    Result res=1;
    gfxInitDefault();
	acInit();
	httpcInit(0x1000);
    consoleInit(GFX_BOTTOM,&bottom);
    consoleInit(GFX_TOP, &top);
	consoleSelect(&top);
	readcfg();
    printf("\x1b[37;1m          MultiDownload by Kartik\n");
	printf("              Version x.3\n");
    printf("           Select an option\n");
    printf("          Press START to exit\n");
    printf("Will download to location %s\n", buffer);
	
	consoleSelect(&bottom);
	printf("\x1b[37;1m         EDIT DOWNLOAD LOCATION\n");
	printf("\n\n\n\n");
	printf("              ENTER A URL\n");
	printf("\n\n\n\n");
	printf("             SCAN A QR CODE\n");
	printf("\n\n\n\n");
	printf("            UPDATE MULTIDOWNLOAD\n");
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
            Result ret = 1;
            if ((didit) && button != SWKBD_BUTTON_NONE)
			{
                ret = http300(mybuf);
			
            if (ret == 0) {
                printf("\x1b[32;1mDOWNLOAD: Success\n\x1b[37;1m");
            }
			else 
				printf("\x1b[31;1mDOWNLOAD: Failed\n\x1b[37;1m");
            //cleaning vars
            for (int i = 0; i < 960; i++)
                mybuf[i] = ' ';
            button = SWKBD_BUTTON_NONE;
            didit = false;
			}
        }
	}
	
	  if(touch.py < 90 && touch.py >60 && touch.px<240) {
		    Result ret =1;
			consoleClear();
			consoleSelect(&bottom);
			consoleClear();
			consoleSelect(&top);
			gfxSetScreenFormat(GFX_BOTTOM,GSP_BGR8_OES);
			
			ret=qr();
			if (ret==0)
				
			{   
			    consoleClear();
				printf("\x1b[32;1mSuccess\n\x1b[37;1m");
				clearScreen();
				gfxFlushBuffers();
				gfxSwapBuffers();
				extern char qurl[1024];
				char kat[1024];
				strcpy(kat,qurl);
				ret=http300(kat);
		         if (ret==0) {
					 printf("\x1b[32;1mDOWNLOAD: Success\n\x1b[37;1m");
					 
				 }
				 else
				 {
					 printf("\x1b[31;1mDOWNLOAD: Failed\n\x1b[37;1m");
				 }
			}
	  }
	   if(touch.py<120 && touch.py>90 && touch.px<240)
	   {   Result ret=0;
		   printf("Getting the latest release");
		   char *get;
		   bool x= envIsHomebrew() ;
           if(x==true)
		 {
           printf("\nUpdating 3dsx\n");
	       get="multidownload.3dsx";
		  char b[40]="/3ds/multidownload/multidownload.3dsx";
		  }
		   else
		   {
			   printf("\nUpdating cia\n");
			   get="multidownload.cia";
			   }
			   #include "json.h"
		   ret=latu((char*)"https://api.github.com/repos/pirater12/multidownload/releases/latest",get);
		  if (ret==1)
		  {
			  printf("failed\n");
		  }
		   else {
		   extern char *url;
		   ret=http300(url); 
		   }
		   if(ret==0)
		   {
                printf("\x1b[32;1mDOWNLOAD: Success\n\x1b[37;1m");
            }
			else 
				printf("\x1b[31;1mDOWNLOAD: Failed\n\x1b[37;1m");
 
	   }
	
        if (kDown & KEY_START)
            break;
        // Flush and swap framebuffers
        gfxFlushBuffers();
        gspWaitForVBlank();
        gfxSwapBuffers();
    }
    hidExit();
    httpcExit();
    acExit();
    gfxExit();
    return 0;
}
