#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <3ds.h>
#include <sys/dirent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <stdbool.h>
#include "quirc.h"
#define WIDTH 320
#define HEIGHT 240
#define SCREEN_SIZE WIDTH * HEIGHT * 2
#define BUF_SIZE SCREEN_SIZE * 2
#define WAIT_TIMEOUT 300000000ULL
char qurl[1024];

void writePictureToIntensityMap(void *fb, void *img, u16 width, u16 height){
	u8 *fb_8 = (u8*) fb;
	u16 *img_16 = (u16*) img;
	for(u32 i = 0; i < width * height; i++){
		u16 data = img_16[i];
		uint8_t b = ((data >> 11) & 0x1F) << 3;
		uint8_t g = ((data >> 5) & 0x3F) << 2;
		uint8_t r = (data & 0x1F) << 3;
		fb_8[i] = (r + g + b) / 3;
	}
}


void cleanup() {
	camExit();
	acExit();
}

void writePictureToFramebufferRGB565(void *fb, void *img, u16 x, u16 y, u16 width, u16 height) {
	u8 *fb_8 = (u8*) fb;
	u16 *img_16 = (u16*) img;
	int i, j, draw_x, draw_y;
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			draw_y = y + height - j;
			draw_x = x + i;
			u32 v = (draw_y + draw_x * height) * 3;
			u16 data = img_16[j * width + i];
			uint8_t b = ((data >> 11) & 0x1F) << 3;
			uint8_t g = ((data >> 5) & 0x3F) << 2;
			uint8_t r = (data & 0x1F) << 3;
			fb_8[v] = r;
			fb_8[v+1] = g;
			fb_8[v+2] = b;
		}
	}
}

// TODO: Figure out how to use CAMU_GetStereoCameraCalibrationData
void takePicture3D(u16 *buf) {
	u32 bufSize;
   CAMU_GetMaxBytes(&bufSize, WIDTH, HEIGHT);
   CAMU_SetTransferBytes(PORT_BOTH, bufSize, WIDTH, HEIGHT);
   CAMU_Activate(SELECT_OUT1_OUT2);

	Handle camReceiveEvent = 0;
	Handle camReceiveEvent2 = 0;
    CAMU_ClearBuffer(PORT_BOTH);
    CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);
    CAMU_StartCapture(PORT_BOTH);
	CAMU_SetReceiving(&camReceiveEvent, buf, PORT_CAM1, SCREEN_SIZE, (s16) bufSize);
	CAMU_SetReceiving(&camReceiveEvent2, buf + SCREEN_SIZE, PORT_CAM2, SCREEN_SIZE, (s16) bufSize);
	svcWaitSynchronization(camReceiveEvent, WAIT_TIMEOUT);
	svcWaitSynchronization(camReceiveEvent2, WAIT_TIMEOUT);
    //CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_NORMAL);
    CAMU_StopCapture(PORT_BOTH);

	svcCloseHandle(camReceiveEvent);
	svcCloseHandle(camReceiveEvent2);
     CAMU_Activate(SELECT_NONE);
}

Result qr() {
	// Initializations
	acInit();
	httpcInit(0);
	// Save current stack frame for easy exit
	u32 kDown;
	u32 kHeld;
	printf("Initializing camera\n");
	// Enable double buffering to remove screen tearing
    gfxFlushBuffers();
	gfxSwapBuffers();
	printf("camInit: 0x%08X\n", (unsigned int) camInit());

	printf("CAMU_SetSize: 0x%08X\n", (unsigned int) CAMU_SetSize(SELECT_OUT1_OUT2, SIZE_CTR_BOTTOM_LCD, CONTEXT_A));
	printf("CAMU_SetOutputFormat: 0x%08X\n", (unsigned int) CAMU_SetOutputFormat(SELECT_OUT1_OUT2, OUTPUT_RGB_565, CONTEXT_A));

	printf("CAMU_SetNoiseFilter: 0x%08X\n", (unsigned int) CAMU_SetNoiseFilter(SELECT_OUT1_OUT2, true));
	printf("CAMU_SetAutoExposure: 0x%08X\n", (unsigned int) CAMU_SetAutoExposure(SELECT_OUT1_OUT2, true));
	printf("CAMU_SetAutoWhiteBalance: 0x%08X\n", (unsigned int) CAMU_SetAutoWhiteBalance(SELECT_OUT1_OUT2, true));
	//printf("CAMU_SetEffect: 0x%08X\n", (unsigned int) CAMU_SetEffect(SELECT_OUT1_OUT2, EFFECT_MONO, CONTEXT_A));

	printf("CAMU_SetTrimming: 0x%08X\n", (unsigned int) CAMU_SetTrimming(PORT_CAM1, false));
	printf("CAMU_SetTrimming: 0x%08X\n", (unsigned int) CAMU_SetTrimming(PORT_CAM2, false));
	//printf("CAMU_SetTrimmingParamsCenter: 0x%08X\n", (unsigned int) CAMU_SetTrimmingParamsCenter(PORT_CAM1, 512, 240, 512, 384));
    struct quirc *qr;

	qr = quirc_new();
	if (!qr){
		printf("Failed to allocate memory");
		return 0;
	}

	if (quirc_resize(qr, WIDTH, HEIGHT) < 0){
		printf("Failed to allocate video memory");
		return 0;
	}
     
	u16 *buf = malloc(BUF_SIZE);

	gfxFlushBuffers();
	gspWaitForVBlank();
	gfxSwapBuffers();

	// Main loop
	while (aptMainLoop()) {
		// Read which buttons are currently pressed or not
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();

		// If START button is pressed, break loop and quit
		if (kDown & KEY_START) {
			break;
		}
            gfxFlushBuffers();
		    takePicture3D(buf);
			writePictureToFramebufferRGB565(gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT,NULL, NULL), buf, 0, 0, WIDTH, HEIGHT);
			int w=WIDTH, h=HEIGHT;
		    u8 *image = (u8*)quirc_begin(qr, &w, &h);
		    writePictureToIntensityMap(image, buf, WIDTH, HEIGHT);
		    quirc_end(qr);

		    int num_codes = quirc_count(qr);
		
		    for (int i = 0; i < num_codes; i++){
			struct quirc_code code;
			struct quirc_data data;
			quirc_decode_error_t err;

			quirc_extract(qr, i, &code);

			writePictureToFramebufferRGB565(gfxGetFramebuffer(GFX_BOTTOM, GFX_RIGHT, NULL, NULL), buf, 0, 0, WIDTH, HEIGHT);
			err = quirc_decode(&code, &data);
			if (!err){
				CAMU_Activate(SELECT_NONE);
				printf("DECODE SUCCESSFUL %s\n",data.payload);
				Result ret = 1 ;
				memcpy(qurl,data.payload,strlen((char*)data.payload)+1);
				printf("Succesful 1");
				 quirc_destroy(qr);
				 printf("\nSuccesful 2\n");
			     camExit();
				 printf("Succesful 3\n");
				 free(buf);
				 printf("Succesful 4\n");
				 cleanup();
				 printf("Succesful 5\n");
				 httpcExit();
				 printf("Succesful 6\n");
				 printf("Successful till here\n");
				 
				 return 0;
              }                                                              
			}
		gfxFlushBuffers();
		gspWaitForVBlank();
		gfxSwapBuffers();
	}
 return 0;
}