#include "libs.h"
#include "download.h"
int main() {
SwkbdState swkbd;
char mybuf[960];
char loca[1024];
SwkbdButton button = SWKBD_BUTTON_NONE;
bool didit = false;
bool didloc = false;
 //Result ret=1;
gfxInitDefault();
consoleInit(GFX_TOP, NULL);
printf("\x1b[47;32m"); 
printf("MultiDownload by Kartik\n");
printf("Press A to begin\n");
printf("Press X to edit Download location\n");
printf("Press START to exit\n");
while (aptMainLoop())
{
gspWaitForVBlank();
hidScanInput();
u32 kDown = hidKeysDown();
if (kDown & KEY_X)
{
didloc = true;
const char* texgen="Enter download location";
swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
swkbdSetHintText(&swkbd, texgen);
button = swkbdInputText(&swkbd, loca, sizeof(loca));
if ((didloc) && button != SWKBD_BUTTON_NONE)
button = SWKBD_BUTTON_NONE;
didit = false;
   FILE *fp;
   fp = fopen("multi.cfg", "wb");
   fprintf(fp,loca);
   fclose(fp);
printf("Location changed to %s\n",loca);
}
if (kDown & KEY_A)
{
didit = true;
const char* texgen2="Enter download url";
swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
swkbdSetHintText(&swkbd, texgen2);
swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
httpcInit(0);
Result ret=1;
if ((didit) && button != SWKBD_BUTTON_NONE)
ret=http300(mybuf,loca);
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
printf("\x1b[47;32m");
printf("MultiDownload by Kartik\n");
printf("Press A to begin\n");
printf("Press START to exit\n");
printf("Press X to edit Download location\n");
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
