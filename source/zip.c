#include "libs.h"

void writeFile(char *filename, void *data, long bytes) {
    FILE *out;
    int i;
    // simplistic directory creation support
    for(i=0; filename[i]; i++) {
        if(filename[i] != '/')
            continue;

        filename[i] = '\0'; // terminate string at this point
        printf("dir to be created is %s\n" ,filename );
        /*if(!(mkdir(e , 0777)))
		printf("Path didn't exist made one with success\n");
	else 
	{   if (ENOENT == errno)
		{printf("error with path\n");
		}
	     if (EEXIST == errno)
		 { printf("exists\n");
	   }			 
	} */

        filename[i] = '/'; // Put the separator back
    }

    if(!i || filename[i-1] == '/')
        return; // empty filename or directory entry
	char local[1024]="/3ds/";
	char *ext =strrchr(loca, '/');
        if (ext) {
        ext = ext +1;
        int len = strlen(ext);
		char reloc[1024];
		memcpy(reloc, loca, strlen(loca) - len);
        reloc[strlen(loca) - len] = '\0';
		printf("location now is %s\n",reloc);
		
	strcat(reloc, filename);

    out = fopen(reloc, "wb");

    if(out != NULL) {
        fwrite(data, 1, bytes, out); // best effort is enough here
        fclose(out);
    } else {
        printf("Couldn't open %s for writing!\n", filename);
    }
  }
}

int processFile(JZFile *zip) {
    JZFileHeader header;
    char filename[1024];
    unsigned char *data;

    if(jzReadLocalFileHeader(zip, &header, filename, sizeof(filename))) {
        printf("Couldn't read local file header!");
        return -1;
    }

    if((data = (unsigned char *)malloc(header.uncompressedSize)) == NULL) {
        printf("Couldn't allocate memory!");
        return -1;
    }

    printf("%s, %d / %d bytes at offset %08X\n", filename,
            header.compressedSize, header.uncompressedSize, header.offset);

    if(jzReadData(zip, &header, data) != 0) {
        printf("Couldn't read file data!");
        free(data);
        return -1;
    }

    writeFile(filename, data, header.uncompressedSize);
    free(data);

    return 0;
}

int recordCallback(JZFile *zip, int idx, JZFileHeader *header, char *filename, void *user_data) {
    long offset;

    offset = zip->tell(zip); // store current position

    if(zip->seek(zip, header->offset, SEEK_SET)) {
        printf("Cannot seek in zip file!");
        return 0; // abort
    }

    processFile(zip); // alters file offset

    zip->seek(zip, offset, SEEK_SET); // return to position

    return 1; // continue
}
Result ezip(char*argv)
{
	FILE *fp;
    int retval = -1;
    JZEndRecord endRecord;
    JZFile *zip;
	if(!(fp = fopen(argv, "rb"))) {
        printf("Couldn't open \"%s\"!",argv);
        return -1;
    }

    zip = jzfile_from_stdio_file(fp);

    if(jzReadEndRecord(zip, &endRecord)) {
        printf("Couldn't read ZIP file end record.");
        goto endClose;
    }

    if(jzReadCentralDirectory(zip, &endRecord, recordCallback, NULL)) {
        printf("Couldn't read ZIP file central record.");
        goto endClose;
    }
	retval = 0;
    endClose:
    zip->close(zip);
	return retval;
    return 0;
	}