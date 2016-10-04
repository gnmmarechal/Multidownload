#include "libs.h"
Result make(u8* buf,u32 size,char* loca)
{
FILE* fptr = fopen(loca, "wb");
      fwrite(buf, 1, size, fptr);
      fclose(fptr);
	  return 0;
	}
Result dsx(char *nurl,u8* buf, u32 size)
{      const char *b = ".3dsx";
	   const char *k ="/";  
	   char* d = strrchr(nurl, '/');
	   printf("%s",d);
	   char *target =NULL;
	   char *end ,*start;
	   Result ret=0;
	  if(start=strstr(nurl,k))
	  {
		  start +=strlen(k);
		  if (end= strstr(start,b))
		  {
		  target = (char*)malloc(end-start+1);
		  memcpy(target,start,end-start);
		  target[end-start]= '\0';
		  }
		  else 
			  printf("error\n");
	        }
			else
			printf("error\n");
		   char* g = strrchr(target, '/');
          printf("%s\n",target);
		   printf("%s\n",g);
		   char a[1002]="/3ds";
		  strcat(a,g);
		  strcat(a,d);
          ret = make(buf, size,a);
		  if (ret==0)
		  {   printf("%s\n",a);
			  printf("success\n");
		      free(target);
		      return 0;
			  }
           return 0;
}

