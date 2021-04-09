/* This program simulates a file system within an actual file.  The
 * simulated file system has only one directory, and two types of
 * metadata structures defined in simfstypes.h.
 */

/* Simfs is run as:
 * simfs -f myfs command args
 * where the -f option specifies the actual file that the simulated file system
 * occupies, the command is the command to be run on the simulated file system,
 * and args represents a list of arguments for the command.  Note that
 * different commands take different numbers of arguments.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "simfs.h"

// We use the ops array to match the file system command entered by the user.
#define MAXOPS  6
char *ops[MAXOPS] = {"initfs", "printfs", "createfile", "readfile",
                     "writefile", "deletefile"};
int find_command(char *);

int
main(int argc, char **argv)
{
    int oc;       /* option character */
    char *cmd;    /* command to run on the file system */
    char *fsname; /* name of the simulated file system file */

    char *usage_string = "Usage: simfs -f file cmd arg1 arg2 ...\n";

    /* Get and check the arguments */
    if(argc < 4) {
        fputs(usage_string, stderr);
        exit(1);
    }

    while((oc = getopt(argc, argv, "f:")) != -1) {
        switch(oc) {
        case 'f' :
            fsname = optarg;
            break;
        default:
            fputs(usage_string, stderr);
            exit(1);
        }
    }

    /* Get the command name */
    char *end_ptr;
    char *end_ptr2;
    cmd = argv[optind];
    optind++;
    switch((find_command(cmd))) {
    case 0: /* initfs */
        initfs(fsname);
        break;
    case 1: /* printfs */
        printfs(fsname);
        break;
    case 2: /* createfile */
        if (argc == 5){
            createFile(argv[optind], fsname);
        }
        else if (argc > 5){
            fprintf(stderr, "Error: Too many arguments provided.\n");
        }
        
        //fprintf(stderr, "Error: createfile not yet implemented\n");
        break;
    case 3: /* readfile */
        //check if end_ptr and 2 are empty otherwise return error
        if (argc == 7){
            int start = strtol(argv[optind+1], &end_ptr,10);
            int length = strtol(argv[optind+2], &end_ptr2, 10);
            if (strlen(end_ptr) ==0 && strlen(end_ptr2)==0){
                readFile(argv[optind], start, length, fsname);
            }
            else{
                fprintf(stderr, "Error: Incorrect parameters.\n");
            }
        }
        else if (argc > 7){
            fprintf(stderr, "Error: Too many arguments provided.\n");
        }
        else if (argc< 7){
            fprintf(stderr, "Error: Not enough arguments provided.\n");
        }
        //fprintf(stderr, "Error: readfile not yet implemented\n");
        break;
    case 4: /* writefile */
        if (argc == 7){
            int start = strtol(argv[optind+1], &end_ptr,10);
            int length = strtol(argv[optind+2], &end_ptr2, 10);
            if (strlen(end_ptr) ==0 && strlen(end_ptr2)==0){
                writeFile(argv[optind], start, length, fsname);
            }
            else{
                fprintf(stderr, "Error: Incorrect parameters.\n");
            }
        }
        else if (argc > 7){
            fprintf(stderr, "Error: Too many arguments provided.\n");
        }
        else if (argc< 7){
            fprintf(stderr, "Error: Not enough arguments provided.\n");
        }
        //fprintf(stderr, "Error: writefile not yet implemented\n");
        break;
    case 5: /* deletefile */
        if (argc == 5){
            deleteFile(argv[optind], fsname);
        }
        else if (argc > 5){
            fprintf(stderr, "Error: Too many arguments provided.\n");
        }
        //fprintf(stderr, "Error: deletefile not yet implemented\n");
        break;
    default:
        fprintf(stderr, "Error: Invalid command\n");
        exit(1);
    }

    return 0;
}

/* Returns a integer corresponding to the file system command that
 * is to be executed
 */
int
find_command(char *cmd)
{
    int i;
    for(i = 0; i < MAXOPS; i++) {
        if ((strncmp(cmd, ops[i], strlen(ops[i]))) == 0) {
            return i;
        }
    }
    fprintf(stderr, "Error: Command %s not found\n", cmd);
    return -1;
}
