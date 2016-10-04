#define URL_MAX 1024
#include "json.h"
#include <3ds.h>
#include "libs.h"
char *url=NULL;

Result latu(char*urlp,char*get) {

    bool hasUpdate = false;
    u32 size=0;
    Result res = 0;
    u32 responseCode = 0;
    httpcContext context;
	printf("%s going to get%s",urlp,get);
         res = httpcOpenContext(&context, HTTPC_METHOD_GET, urlp, 1);
		 if(res!=0){
			return 1;
		 }
		 printf("%d",(int)res);
		 res = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
		 if(res!=0){
			return 1;
		 }
		 printf("%d",(int)res);
		 res = httpcAddRequestHeaderField(&context, (char*)"User-Agent", (char*)"MULTIDOWNLOAD");
		 if(res!=0){
			return 1;
		 }
		 printf("%d",(int)res);
         res = httpcBeginRequest(&context);
		 if(res!=0){
			return 1;
		 }
		 printf("%d",(int)res);
         res = httpcGetResponseStatusCode(&context, &responseCode) ;
		 if(res!=0){
			return 1;
		 }
		 printf("%d",(int)res);
         res = httpcGetDownloadSizeState(&context, NULL, &size) ;
		 if(res!=0){
			return 1;
		 }
		 printf("%d",(int)res);
         char* jsonText = (char*) calloc(sizeof(char), size);
         u32 bytesRead = 0;
         res = httpcDownloadData(&context, (u8*) jsonText, size, &bytesRead); 
		 printf("%d",(int)res);
         json_value* json = json_parse(jsonText, size);
            if(json->type == json_object) {
               json_value* name = NULL;
               json_value* assets = NULL;
	
			   for(u32 i = 0; i < json->u.object.length; i++) {
                json_value* val = json->u.object.values[i].value;
                if(strncmp(json->u.object.values[i].name, "name", json->u.object.values[i].name_length) == 0 && val->type == json_string) {
                name = val;
                } else if(strncmp(json->u.object.values[i].name, "assets", json->u.object.values[i].name_length) == 0 && val->type == json_array) {
                                            assets = val;
				}
								
                                    }

                                    if(name != NULL && assets != NULL ) {
                                            for(u32 i = 0; i < assets->u.array.length; i++) {
                                                json_value* val = assets->u.array.values[i];
                                                if(val->type == json_object) {
                                                    json_value* assetName = NULL;
                                                    json_value* assetUrl = NULL;

                                                    for(u32 j = 0; j < val->u.object.length; j++) {
                                                        json_value* subVal = val->u.object.values[j].value;
                                                        if(strncmp(val->u.object.values[j].name, "name", val->u.object.values[j].name_length) == 0 && subVal->type == json_string) {
                                                            assetName = subVal;
                                                        } else if(strncmp(val->u.object.values[j].name, "browser_download_url", val->u.object.values[j].name_length) == 0 && subVal->type == json_string) {
                                                            assetUrl = subVal;
                                                        }
                                                    }

                                                    if(assetName != NULL && assetUrl != NULL) {
                                                        if(strncmp(assetName->u.string.ptr, get, assetName->u.string.length) == 0) {
                                                            url = assetUrl->u.string.ptr;
                                                            break;
                                                        }
                                                    }else printf("error 2\n");
                                                }
                                            }

                                            if(url != NULL) {

												printf("%s",url);
					}
				}else printf("error 1\n");
        httpcCloseContext(&context);
  
			}else printf("error\n");
return 0;}
