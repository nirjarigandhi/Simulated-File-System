#include <stdio.h>
#include "simfstypes.h"

/* File system operations */
void printfs(char *);
void initfs(char *);
void createFile(char *filename, char *fsname);
void deleteFile(char *filename, char *fsname);
void readFile(char *filename, int start, int length, char *fsname);
void writeFile(char *filename, int start, int length, char *fsname);
/* Internal functions */
FILE *openfs(char *filename, char *mode);
void closefs(FILE *fp);
