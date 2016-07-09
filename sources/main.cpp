#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
Result http_download(httpcContext *context, const char* url)//This error handling needs updated with proper text printing once ctrulib itself supports that.
{
	Result ret=0;
	u32 statuscode=0;
	u32 size=0, contentsize=0;
	u8 *buf;
                char *a=strrchr(url, '/');
                  char *b=strrchr(a ,'.');
	//httpcAddRequestHeaderField(context, (char*)"User-Agent",  (char*)"MULTIDOWNLOAD");
                //httpcAddTrustedRootCA(context, cybertrust_cer, cybertrust_cer_len);
	//httpcAddTrustedRootCA(context, digicert_cer, digicert_cer_len);
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

	return 0;
}




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
