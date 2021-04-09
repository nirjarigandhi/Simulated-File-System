/* This file contains functions that are not part of the visible "interface".
 * They are essentially helper functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simfs.h"

/* Internal helper functions first.
 */

FILE *
openfs(char *filename, char *mode)
{
    FILE *fp;
    if((fp = fopen(filename, mode)) == NULL) {
        perror("openfs");
        exit(1);
    }
    return fp;
}

void
closefs(FILE *fp)
{
    if(fclose(fp) != 0) {
        perror("closefs");
        exit(1);
    }
}

/* File system operations: creating, deleting, reading, and writing to files.
 */
void createFile(char *filename, char *fsname){
    FILE *fptr = openfs(fsname, "rb+");

    //check if file name is appropriate size(11 characters max)
    if (strlen(filename) > 11){
        fprintf(stderr,"Error: File name is too long.\n");
        closefs(fptr);
        exit(1);
    }
    //create fentry(struct defined in simfstypes.h) using filename, filesize and pointer to first node
    fentry newfile;
    strcpy(newfile.name, filename);
    newfile.size = 0; //new file size is 0 because there is nothing in the file
    newfile.firstblock = -1; //new empty file doesnt need any space and therefore has no file node associated to it currently

    //take created fentry and add it to list of fentrys(add file to system of existing files) (find empty spot in list of fentries and add newfile there)
    fentry files[MAXFILES]; //gets list of fentries (from printfs)

    if (fread(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read file entries.\n");
        closefs(fptr);
        exit(1);
    }
    //recurse through fentries to find first empty spot to put new file and if fentry is full then return an error
    //check if file name exists if it does return error
    int empty_node = -100;
    
    for (int i = 0; i < MAXFILES; i++){
        if (strcmp(files[i].name,filename) == 0){
            fprintf(stderr, "Error: File name is already used.\n");
            closefs(fptr);
            exit(1);
        }
        if (strlen(files[i].name) == 0 && empty_node == -100){
            empty_node = i;
        }

    }
    files[empty_node] = newfile;
    rewind(fptr);

    if (empty_node == -100){ //returns error if fs is full
        fprintf(stderr, "Error: File system is full\n");
        closefs(fptr);
        exit(1);
    }
    if (fwrite(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //writes to list of files to update file system
        fprintf(stderr, "Error: Writing error prevented the function from completing\n");
        closefs(fptr);
        exit(1);
    }
    
    closefs(fptr);
}
void deleteFile(char *filename, char *fsname){
    //open file system
    //check if given filename exists in file system
    //return error if it doesnt
    //check if file is empty
        //if file is empty then change name parameter to empty character array to null parameter
    //if file isnt empty
        //locate first block and all corresponding blocks
        //recurse through each block one by one and change each value USED in block to 0
        //use BYTES_USED variable and bytes to read variable(printfs)
        //once all nodes are empty set each node to size 0 and nextnode to -1 
        //then change file name to null parameter, file size to 0 and first block to -1

    FILE *fptr = openfs(fsname, "rb+");
    if (strlen(filename) > 11){
        fprintf(stderr,"Error: File name is too long.\n");
        closefs(fptr);
        exit(1);
    }
    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];

     if (fread(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read file entries.\n");
        closefs(fptr);
        exit(1);
    }
    if (fread(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read fnodes.\n");
        closefs(fptr);
        exit(1);
    }
    int size=0;
    int firstblock=-1;
    int count =0;
    for (int i=0; i <MAXFILES; i++){
        if(strcmp(files[i].name, filename) == 0){
            //save size and first block to delete info in each node
            size = files[i].size;
            firstblock = files[i].firstblock;
            //reset file name, size and firstblock
            //need to reset each element of files[i].name individually, simply doing strcpy doesn't work
            for (int j = 0; j <strlen(filename); j++){
                (files[i].name)[j]= '\0';
            }
            files[i].size = 0;
            files[i].firstblock = -1;
            break;
        }
        count++;
    }
    if (count == MAXFILES){
        fprintf(stderr, "Error: Given file name does not exist.\n");
        closefs(fptr); 
        exit(1);
    }
    //find how many nodes will be needed
    int next_block = firstblock;
    char buffer[BLOCKSIZE] ={0};
    while (next_block != -1){
        fnode curr_node = fnodes[next_block];
        int offset = BLOCKSIZE*next_block;
        fseek(fptr, offset, SEEK_SET);
        if (size>=BLOCKSIZE){
            fwrite(buffer, sizeof(char),BLOCKSIZE, fptr);
            size -= BLOCKSIZE;
        }
        if(size<BLOCKSIZE){
            fwrite(buffer, sizeof(char),size, fptr);
        }
        next_block = curr_node.nextblock;
        curr_node.blockindex *= (-1);
        curr_node.nextblock = -1;
        fnodes[curr_node.blockindex*(-1)] = curr_node;
    }
    
    rewind(fptr);
    if (fwrite(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //writes to list of files to update file system
        fprintf(stderr, "Error: Writing error prevented the function from completing\n");
        closefs(fptr);
        exit(1);
    }
    if (fwrite(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //writes to list of files to update file system
        fprintf(stderr, "Error: Writing error prevented the function from completing\n");
        closefs(fptr);
        exit(1);
    }
    closefs(fptr);
}
void readFile(char *filename, int start, int length, char *fsname){
    //open fs --
    //locate file in fentries--
    //if file doesnt exist return error--
    //check if offset and length values are of a reasonable size (start value isnt greater than all nodes)--
    //if file does exist check if its empty--
    //if its empty return empty line in output stream or that the file is empty?
    //if file isn't empty then find file node(first block)
    //seek to node
    //find which node offest is part off
    //write into output stream

    //checks for inputs:
        //check if filename exists --
        //check if file is empty --
        //check if start+length (amount of info to read) > files size --
        //check if value of start and length is positive --
        //check if start and length are within reasonable size amount
    FILE *fptr = openfs(fsname, "r");
    if (strlen(filename) > 11){
        fprintf(stderr,"Error: File name is too long.\n");
        closefs(fptr);
        exit(1);
    }
    if(start<0){
        fprintf(stderr, "Error: Start value must be positive.\n");
        closefs(fptr);
        exit(1);
    }
    if(length<0){
        fprintf(stderr, "Error: Length of file to read must be positive.\n");
        closefs(fptr);
        exit(1);
    }

    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];

    if (fread(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read file entries.\n");
        closefs(fptr);
        exit(1);
    }
    if (fread(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read fnodes.\n");
        closefs(fptr);
        exit(1);
    }
    int size=0;
    int firstblock=-1;
    int count =0;
    for (int i=0; i<MAXFILES; i++){
        if(strcmp(files[i].name, filename) == 0){
            //save size and first block to delete info in each node
            size = files[i].size;
            firstblock = files[i].firstblock;
            break;
            }
        count++;
    }
    if (count == MAXFILES){
        fprintf(stderr, "Error: Given file name does not exist.\n");
        closefs(fptr); 
        exit(1);
    }
    if (firstblock == -1){
        fprintf(stderr, "Error: File is empty.\n");
        closefs(fptr); 
        exit(1);
    }
    if (start > size){
         fprintf(stderr, "Error: Not enough data in file to read from.\n");
        closefs(fptr); 
        exit(1);
    }
    if (start+length > size){
        fprintf(stderr, "Error: Not enough data in file to read from.\n");
        closefs(fptr); 
        exit(1);
    }
    int next_block = firstblock;
    
    int length_read = 0;
    char info[size];
    //if length read isnt equal to length, then we have to move on and read from next block associated to file
    //find start location and check if blocksize-that location is equal to length
    //if it isnt then we need to move onto next block and read from there
    //&info[100] = next block of info

    while(next_block != -1){
        fnode curr_node = fnodes[next_block];
        int offset = BLOCKSIZE*next_block;
        fseek(fptr, offset, SEEK_SET);
        if (size>=BLOCKSIZE){
            fread(&info[length_read], sizeof(char), BLOCKSIZE, fptr);
            length_read += BLOCKSIZE;
            size -= BLOCKSIZE;
        }
        else if (size<BLOCKSIZE){
            fread(&info[length_read], sizeof(char), sizeof(char)*size, fptr);
            length_read += size;
            break;
        }
        next_block = curr_node.nextblock;
    }
    fwrite(&info[start], 1, length, stdout);
    printf("\n");
    closefs(fptr);
}

void writeFile(char *filename, int start, int length, char *fsname){
    //open file system--
    //go through fentries and check if filename exists or not, --
    //if file name doesnt exist return error--
    //if file name does exist then check if file is empty
    //if file is empty
        //make sure offset starts at 0--
        //check how many fnode blocks will be needed to write the given amount into the file--
        //if that many fnodes are avaliable then write to file 
            //if more than one node is needed than make sure to include references to next node
        //if enough fnodes aren't avaliable then return error
    //if file isn't empty then navigate to offset and add in characters 

    //check if maxfiles and maxblocks is more than 128
    //fseek to the node and then do node*block size to get start of file
    //check for invalid parameters
    //IMPORTANT FUNCTIONS FROM PRINFS:
    //fseek: Sets the position indicator associated with the stream to a new position.
    //fputs: Writes the C string pointed by str to the stream.
    //fgets: Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.
    //fscanf: Reads data from the stream and stores them according to the parameter format into the locations pointed by the additional arguments.
    //fread: Reads an array of count elements, each one with a size of size bytes, from the stream and stores them in the block of memory specified by ptr.
    //read using fread where parameter for file stream is stdin
    FILE *fptr = openfs(fsname, "rb+");
    
    if (strlen(filename) > 11){
        fprintf(stderr,"Error: File name is too long.\n");
        closefs(fptr);
        exit(1);
    }
    if(start<0){
        fprintf(stderr, "Error: Start value must be positive.\n");
        closefs(fptr);
        exit(1);
    }
    if(length<0){
        fprintf(stderr, "Error: Length of file to read must be positive.\n");
        closefs(fptr);
        exit(1);
    }

    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];

    if (fread(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read file entries.\n");
        closefs(fptr);
        exit(1);
    }
    if (fread(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //reads into list of fentries
        fprintf(stderr, "Error: Could not read fnodes.\n");
        closefs(fptr);
        exit(1);
    }
    int size=0;
    int firstblock=-1;
    int count =0;
    int file_index = -100;
    for (int i=0; i<MAXFILES; i++){
        if(strcmp(files[i].name, filename) == 0){
            //save size and first block to delete info in each node
            size = files[i].size;
            firstblock = files[i].firstblock;
            file_index = i;
            break;
            }
        count++;
    }
    if (count == MAXFILES){
        fprintf(stderr, "Error: Given file name does not exist.\n");
        closefs(fptr); 
        exit(1);
    }
    if ((firstblock == -1 && start != 0) || start>size){
        fprintf(stderr, "Error: Invalid start position.\n");
        closefs(fptr); 
        exit(1);
    }
    //read in info from stdin
    char info[length+1];
    
    if (fread(info, sizeof(char), length, stdin)!=length){
        fprintf(stderr, "Error: Couldn't read Input.\n");
        closefs(fptr);
        exit(1);
    }
    //check if we need to start from a new block of existsing block based on start
    //check if we have enough empty nodes to write data

    int length2 = length;

    if (firstblock == -1){
        int nodes_needed = length/BLOCKSIZE;
        if (length%BLOCKSIZE != 0){
            nodes_needed += 1;
        }
        
        //get all empty nodes needed
        int empty_nodes[nodes_needed];
        int count1  = 0;
        int written =0;
        int counter = 0;
        for (int i=0; i<MAXBLOCKS;i++){
            if (fnodes[i].blockindex <0 && count1 != nodes_needed){
                empty_nodes[count1] = fnodes[i].blockindex;
                count1 += 1;
            }
            if (count1 == nodes_needed){
                break;
            }
        }
        while (written != length2 || counter < nodes_needed){
            int curr_node = empty_nodes[counter];
            
            fnode node = fnodes[(-1)*curr_node];
            
            if (files[file_index].firstblock == -1){
                files[file_index].firstblock = (-1)*curr_node;
            }
            if (length>= BLOCKSIZE){
                fseek(fptr, (-1)*(curr_node)*BLOCKSIZE, SEEK_SET);
                fwrite(&info[written],sizeof(char), BLOCKSIZE,fptr);
                files[file_index].size += BLOCKSIZE;
                node.blockindex = (-1)*curr_node;
                if (counter+1 < nodes_needed){
                    node.nextblock = (-1)*empty_nodes[counter+1];
                }
                counter += 1;
                //node.nextblock = prev_block;
                written += BLOCKSIZE;
                length -= BLOCKSIZE;
            }
            else if (length < BLOCKSIZE){
                fseek(fptr, (-1)*(curr_node)*BLOCKSIZE, SEEK_SET);
                fwrite(&info[written],sizeof(char), length,fptr);
                files[file_index].size += length;
                node.blockindex = (-1)*curr_node;
                if (counter+1 < nodes_needed){
                    node.nextblock = (-1)*empty_nodes[counter+1];
                }
                counter += 1;
                //node.nextblock = prev_block;
                written += length;
                length -= length;
            }
            fnodes[(-1)*curr_node] = node;
            
        }
        rewind(fptr);
        if (fwrite(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //writes to list of files to update file system
            fprintf(stderr, "Error: Writing error prevented the function from completing\n");
            closefs(fptr);
            exit(1);
        }
        if (fwrite(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //writes to list of files to update file system
            fprintf(stderr, "Error: Writing error prevented the function from completing\n");
            closefs(fptr);
            exit(1);
        }
        closefs(fptr);
    }
    else{

        //size%blocksize will give us where current written data end in last block
        //blocksize - start%blocksize gives us where to start in block
        //if calculation <= length then write data directly in
        //otherwise first write in blocksize - start%blocksize bytes and add to written, then either find next block or empty block and write remaining data
        
        int start_nodes = start/BLOCKSIZE; //which node start value is located in
        int offset = start%BLOCKSIZE; //where in the node start is located
        int written =0;

        int num_nodes = size/BLOCKSIZE; //numebr of nodes contained in the file
        if (start%BLOCKSIZE != 0){
            num_nodes += 1;
        }
        //fseek to startonce we get which block start is located in
        //find which block(index) start is in
        //fseek to that location
        //write blocksize - start%blocksize bytes of information 

        //locate which node start is in
        int next_block = firstblock;

        for (int i=0; i <start_nodes;i++){
            if (fnodes[next_block].nextblock != -1){
                next_block = fnodes[next_block].nextblock;
            }
        }

        if (BLOCKSIZE-offset >= length2 && start%BLOCKSIZE != 0){
            fseek(fptr, next_block*BLOCKSIZE+offset, SEEK_SET);
            fwrite(&info[written],sizeof(char), length2,fptr);
            files[file_index].size += length2;
            //written += length;
            rewind(fptr);
            if (fwrite(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //writes to list of files to update file system
                fprintf(stderr, "Error: Writing error prevented the function from completing\n");
                closefs(fptr);
                exit(1);
            }
            if (fwrite(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //writes to list of files to update file system
                fprintf(stderr, "Error: Writing error prevented the function from completing\n");
                closefs(fptr);
                exit(1);
            }
            closefs(fptr);
        }
        else{
            //get number of extra empty nodes will be needed for that length
            //offset represents where in node we are starting so BLOCKSIZE-offset will give us how many more bytes of info we can write into that node
            //length-(BLOCKSIZE-offset)
            int start_nodes = start/BLOCKSIZE; //which node start value is located in
            int offset = start%BLOCKSIZE; //where in the node start is located

            int nodes_needed =0;
            if (offset != 0){
                nodes_needed =(length2 - (BLOCKSIZE-offset))/BLOCKSIZE;
            }
            else{
                nodes_needed = length2/BLOCKSIZE;
            }
            if ((length2 - (BLOCKSIZE-offset))%BLOCKSIZE != 0){
                nodes_needed += 1;
            }
            //get all empty nodes needed
            int empty_nodes[nodes_needed];
            int count1  = 0;
            int written =0;
           
            for (int i=0; i<MAXBLOCKS;i++){
                if (fnodes[i].blockindex <0 && count1 != nodes_needed){
                    empty_nodes[count1] = fnodes[i].blockindex;
                    count1 += 1;
                }
                if (count1 == nodes_needed){
                    break;
                }
            }
            //write into existing node
            int num_nodes = size/BLOCKSIZE; //numebr of nodes contained in the file
            if (start%BLOCKSIZE != 0){
                num_nodes += 1;
            }
            //locate node where start is
            int next_block = firstblock;
            for (int i=0; i <start_nodes;i++){
                if (fnodes[next_block].nextblock != -1){
                    next_block = fnodes[next_block].nextblock;
                }
            }

            //write into existsing node
            if (start%BLOCKSIZE != 0){
                fseek(fptr, next_block*BLOCKSIZE+offset, SEEK_SET);
                fwrite(&info[written],sizeof(char), length2,fptr);
                written += BLOCKSIZE-offset;
                length -= BLOCKSIZE-offset;
            }
            
            files[file_index].size += length2;
            //write into other nodes
            int counter = 0;
            while (written != length2 && nodes_needed != counter){
                int curr_node = fnodes[next_block].nextblock;
                if ((curr_node) == -1){
                    fnodes[next_block].nextblock = (-1)*empty_nodes[counter];
                    curr_node = fnodes[next_block].nextblock;
                    fnodes[curr_node].blockindex *= (-1);
                    counter += 1;
                }
                if (length>= BLOCKSIZE){
                    fseek(fptr, curr_node*BLOCKSIZE, SEEK_SET);
                    fwrite(&info[written],sizeof(char), BLOCKSIZE,fptr);
                    written += BLOCKSIZE;
                    length -= BLOCKSIZE;
                }
                else if (length < BLOCKSIZE){
                    fseek(fptr, curr_node*BLOCKSIZE, SEEK_SET);
                    fwrite(&info[written],sizeof(char), length,fptr);
                    written += length;
                    length -= length;
                }
                next_block = fnodes[next_block].nextblock;  
            }
            rewind(fptr);
            if (fwrite(files, sizeof(fentry), MAXFILES, fptr) != MAXFILES){ //writes to list of files to update file system
                fprintf(stderr, "Error: Writing error prevented the function from completing\n");
                closefs(fptr);
                exit(1);
            }
            if (fwrite(fnodes, sizeof(fnode), MAXBLOCKS, fptr) != MAXBLOCKS){ //writes to list of files to update file system
                fprintf(stderr, "Error: Writing error prevented the function from completing\n");
                closefs(fptr);
                exit(1);
            }
            closefs(fptr);
        }
    }
}

// Signatures omitted; design as you wish.

