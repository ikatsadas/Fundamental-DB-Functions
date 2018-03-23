
#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include "dbtproj.h"
#include <limits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


void quickSort(block_t* buffer, int left[3], int right[3], unsigned char field)
{

    bool check;
    int i = left[0], j = right[0];

    //Store starting positions
    int left1 = left[1], left2 = left[2];
    int right1 = right[1], right2 = right[2];

    int newLeft[3], newRight[3]; //Arrays to use in recursion
    for (int i = 0; i < 3; i++) {
        newLeft[i] = left[i];
        newRight[i] = right[i];
    }

    record_t tmp;
    int pivot;
    char p[STR_LENGTH];
    //Set pivot
    if (field == '0') {
        pivot = buffer[left[1]].entries[left[2]].recid;
    }
    else if (field == '1') {
        pivot = buffer[left[1]].entries[left[2]].num;
    }
    else if (field == '2') {
        strcpy(p, buffer[left[1]].entries[left[2]].str);
    }
    else {
        strcpy(p, buffer[left[1]].entries[left[2]].str);
        pivot = buffer[left[1]].entries[left[2]].num;
    }

    while (i <= j) {
        //Set check variable considering the field
        if (field == '0') {
            check = buffer[left[1]].entries[left[2]].recid < pivot;
        }
        else if (field == '1') {

            check = buffer[left[1]].entries[left[2]].num < pivot;
        }
        else if (field == '2') {
            check = strcmp(buffer[left[1]].entries[left[2]].str, p) < 0;
        }
        else {
            check = buffer[left[1]].entries[left[2]].num < pivot;
            if (buffer[left[1]].entries[left[2]].num == pivot)
                check = strcmp(buffer[left[1]].entries[left[2]].str, p) < 0;
        }

        while (check) {
            i++;

            //Increase entries position if possible else increase buffer position
            if (left[2] == MAX_RECORDS_PER_BLOCK - 1) {
                left[2] = 0;
                left[1]++;
            }
            else {
                left[2]++;
            }

            //Re-set check
            if (field == '0') {
                check = buffer[left[1]].entries[left[2]].recid < pivot;
            }
            else if (field == '1') {
                check = buffer[left[1]].entries[left[2]].num < pivot;
            }
            else if (field == '2') {
                check = strcmp(buffer[left[1]].entries[left[2]].str, p) < 0;
            }
            else {
                check = buffer[left[1]].entries[left[2]].num < pivot;
                if (buffer[left[1]].entries[left[2]].num == pivot)
                    check = strcmp(buffer[left[1]].entries[left[2]].str, p) < 0;
            }
        }

        //Set check variable considering the field
        if (field == '0') {
            check = buffer[right[1]].entries[right[2]].recid > pivot;
        }
        else if (field == '1') {
            check = buffer[right[1]].entries[right[2]].num > pivot;
        }
        else if (field == '2') {
            check = strcmp(buffer[right[1]].entries[right[2]].str, p) > 0;
        }
        else {
            check = buffer[right[1]].entries[right[2]].num > pivot;
            if (buffer[right[1]].entries[right[2]].num == pivot)
                check = strcmp(buffer[right[1]].entries[right[2]].str, p) > 0;
        }

        while (check) {
            j--;
            //Decrease entries position if possible else decrease buffer position
            if (right[2] == 0) {
                right[1]--;
                right[2] = MAX_RECORDS_PER_BLOCK - 1;
            }
            else {
                right[2]--;
            }
            //Re-set check
            if (field == '0') {
                check = buffer[right[1]].entries[right[2]].recid > pivot;
            }
            else if (field == '1') {
                check = buffer[right[1]].entries[right[2]].num > pivot;
            }
            else if (field == '2') {
                check = strcmp(buffer[right[1]].entries[right[2]].str, p) > 0;
            }
            else {
                check = buffer[right[1]].entries[right[2]].num > pivot;
                if (buffer[right[1]].entries[right[2]].num == pivot)
                    check = strcmp(buffer[right[1]].entries[right[2]].str, p) > 0;
            }
        }

        if (i <= j) {
            //Swap elements
            tmp = buffer[left[1]].entries[left[2]];
            buffer[left[1]].entries[left[2]] = buffer[right[1]].entries[right[2]];
            buffer[right[1]].entries[right[2]] = tmp;
            i++;
            //Increase entries position if possible else increase buffer position
            if (left[2] == MAX_RECORDS_PER_BLOCK - 1) {
                left[2] = 0;
                left[1]++;
            }
            else {
                left[2]++;
            }
            j--;
            //Decrease entries position if possible else decrease buffer position
            if (right[2] == 0) {
                right[1]--;
                right[2] = MAX_RECORDS_PER_BLOCK - 1;
            }
            else {
                right[2]--;
            }
        }
    };

    //Recursion
    //If starting position is smaller than current value of j
    if (left[0] < j) {
        //Recall quicksort with the starting left and a different right
        newRight[0] = j;
        newRight[1] = right[1];
        newRight[2] = right[2];

        newLeft[0] = left[0];
        newLeft[1] = left1;
        newLeft[2] = left2;
        quickSort(buffer, newLeft, newRight, field);
    }
    //If starting position is bigger than current value of i
    if (i < right[0]) {
        //Recall quicksort with the starting right and a different left
        newLeft[0] = i;
        newLeft[1] = left[1];
        newLeft[2] = left[2];

        newRight[0] = right[0];
        newRight[1] = right1;
        newRight[2] = right2;
        quickSort(buffer, newLeft, newRight, field);
    }
}

void MergeSort(char* infile, unsigned char field, block_t* buffer, unsigned int nmem_blocks, char* outfile, unsigned int* nsorted_segs, unsigned int* npasses, unsigned int* nios)
{

    FILE *input, *tempOut, *mergeOut;
    input = fopen(infile, "rb"); //Open input file

    *nsorted_segs=0;
    *npasses=0;
    *nios=0;
    
    bool flag = false;
    int numOfBlocks, files = 0, totalNumOfBlocks = 0; //Number of blocks used in memory,number of files created
    int i, j, k; //Counters
    int left[3], right[3]; //Arrays for quicksort

    while (!feof(input)) {

        numOfBlocks = 0;
        //Read file in pieces
        for (int i = 0; i < nmem_blocks; i++) {
            fread(&buffer[i], 1, sizeof(block_t), input);
            if (feof(input)) {
                flag = true;
                break;
            }
            numOfBlocks++;
            *nios=*nios+1;
            totalNumOfBlocks++;
        }
        //If file is at the end
        if (numOfBlocks == 0)
            break;
        //Set variables for quicksort
        left[0] = 0;
        left[1] = 0;
        left[2] = 0;
        right[0] = (numOfBlocks * MAX_RECORDS_PER_BLOCK) - 1;
        right[1] = numOfBlocks - 1;
        right[2] = MAX_RECORDS_PER_BLOCK - 1;

        //Call quicksort
        quickSort(buffer, left, right, field);

        //Generate a file and write the sorted piece
        char filename[50];
        sprintf(filename, "file%d%d.bin", 0, files);
        tempOut = fopen(filename, "wb");
        for (i = 0; i < numOfBlocks; i++) {
            fwrite(&buffer[i], 1, sizeof(block_t), tempOut);
            *nios=*nios+1;
        }
        fclose(tempOut);
        files++;
    }
    fclose(input);
    char filename[15];
    char minc[STR_LENGTH]; //Min str value
    int runs = 0, min, invalidBlocks; //Number of runs until now, min value, number of blocks not used anymore
    int entriesPosA[nmem_blocks]; //Array to keep pointer of the position in the entries array of a block
    int mergeFileCount = 0, bufferPos; //Merged files written, num

    //Until there is only one file
    while (files > 1) {
        *npasses=*npasses+1; //Count passes
        runs++;

        //Set min values considering the field
        if (field == '0' || field == '1')
            min = std::numeric_limits<int>::max();
        else if (field == '2')
            strcpy(minc, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
        else {
            strcpy(minc, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
            min = std::numeric_limits<int>::max();
        }

        invalidBlocks = 0;
        FILE* tmpfiles[files];
        int mergeFileCount = 0;

        for (i = 0; i < files; i++) {
            //Open the output file else open the next temp file
            if (files == 2)
                mergeOut = fopen(outfile, "wb");
            else {
                sprintf(filename, "file%d%d.bin", runs, mergeFileCount);
                mergeOut = fopen(filename, "wb");
            }

            //Open previously created input files
            for (j = 0; j < nmem_blocks - 1; j++) {
                sprintf(filename, "file%d%d.bin", runs - 1, i + j);
                entriesPosA[j] = 0;
                if ((i + j) <= files - 1)
                    tmpfiles[i + j] = fopen(filename, "rb");
            }

            numOfBlocks = 0;
            //Read blocks in the first nmem_blocks-1 positions of the buffer
            for (k = 0; k < nmem_blocks - 1; k++) {
                numOfBlocks++;
                if ((i + k) <= files - 1) {
                    fread(&buffer[k], 1, sizeof(block_t), tmpfiles[i + k]);
                    if (!feof(tmpfiles[i + k])) {
                        buffer[k].bdummy1 = i + k; //Keep track of the input file
                        *nios=*nios+1;
                    }
                }
                else {
                    buffer[k].valid = false; //If file is at the end set block invalid
                    invalidBlocks++;
                }
            }
            entriesPosA[nmem_blocks - 1] = 0;
            bufferPos = -1; //Which block has the min

            //Until all blocks are invalid
            while (invalidBlocks < numOfBlocks) {

                //Loop to find the min value of records in buffer
                for (k = 0; k < numOfBlocks; k++) {

                    //Set check variable considering the fields
                    if (field == '0')
                        flag = buffer[k].entries[entriesPosA[k]].recid < min;
                    else if (field == '1')
                        flag = buffer[k].entries[entriesPosA[k]].num < min;
                    else if (field == '2')
                        flag = strcmp(buffer[k].entries[entriesPosA[k]].str, minc) < 0;
                    else {
                        flag = buffer[k].entries[entriesPosA[k]].num < min;
                        if (buffer[k].entries[entriesPosA[k]].num == min)
                            flag = strcmp(buffer[k].entries[entriesPosA[k]].str, minc) < 0;
                    }

                    //If block is valid and smaller than current min
                    if (buffer[k].valid && flag) {
                        buffer[nmem_blocks - 1].entries[entriesPosA[nmem_blocks - 1]] = buffer[k].entries[entriesPosA[k]], sizeof(record_t); //Add record in the last block of the buffer
                        if (bufferPos != -1)
                            entriesPosA[bufferPos]--; //If the last min was from another record decrease it's entries position pointer
                        bufferPos = k; //Set the block from which the min value came

                        //Set new min
                        if (field == '0')
                            min = buffer[k].entries[entriesPosA[k]].recid;
                        else if (field == '1')
                            min = buffer[k].entries[entriesPosA[k]].num;
                        else if (field == '2')
                            strcpy(minc, buffer[k].entries[entriesPosA[k]].str);
                        else {
                            strcpy(minc, buffer[k].entries[entriesPosA[k]].str);
                            min = buffer[k].entries[entriesPosA[k]].num;
                        }

                        entriesPosA[k]++; //Increase the entries position pointer in the block from which the min value came
                    }
                }

                entriesPosA[nmem_blocks - 1]++; //Go to fill the next position
                bufferPos = -1; //Set buffer pointer to -1

                //Reset min value
                if (field == '0' || field == '1')
                    min = std::numeric_limits<int>::max();
                else if (field == '2')
                    strcpy(minc, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
                else {
                    strcpy(minc, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
                    min = std::numeric_limits<int>::max();
                }

                //If last block is full write it to the file
                if (entriesPosA[nmem_blocks - 1] == MAX_RECORDS_PER_BLOCK) {
                    entriesPosA[nmem_blocks - 1] = 0;
                    fwrite(&buffer[nmem_blocks - 1], 1, sizeof(block_t), mergeOut);
                    *nios=*nios+1;
                }

                //If a block has run out of (all of its values are written to the output file) read the next block from the same input file
                for (k = 0; k < numOfBlocks; k++) {
                    if (buffer[k].valid && entriesPosA[k] == MAX_RECORDS_PER_BLOCK) {
                        int fileNum = buffer[k].bdummy1;
                        fread(&buffer[k], 1, sizeof(block_t), tmpfiles[fileNum]);
                        if (feof(tmpfiles[fileNum])) {
                            buffer[k].valid = false;
                            invalidBlocks++;
                        }
                        else {
                            *nios=*nios+1;
                            buffer[k].bdummy1 = fileNum;
                            entriesPosA[k] = 0;
                        }
                    }
                }
            }
            fclose(mergeOut);
            invalidBlocks = 0;
            //Close files
            for (j = 0; j < nmem_blocks - 1; j++) {
                if ((i + j) <= files - 1) {
                    sprintf(filename, "file%d.bin", i + j);
                    fclose(tmpfiles[i + j]);
                }
            }
            i += nmem_blocks - 2;
            mergeFileCount++; //One merge file was created
        }

        //Delete temp files
        for (i = 0; i < files; i++) {
            sprintf(filename, "file%d%d.bin", runs - 1, i);
            remove(filename);
        }
        *nsorted_segs =*nsorted_segs+ files;
        files = mergeFileCount;
    }
    *nsorted_segs=*nsorted_segs+1;
    *npasses=*npasses+1;

    cout << "----MERGESORT----" << endl;
    cout << "Passes :  " << *npasses << endl;
    cout << "Sorted segments :  " << *nsorted_segs << endl;
    cout << "IOs: " << *nios << endl;

    return;
}

void EliminateDuplicates(char* infile, unsigned char field, block_t* buffer, unsigned int nmem_blocks, char* outfile, unsigned int* nunique, unsigned int* nios)
{

    FILE *input, *output;
    input = fopen(infile, "rb");
    output = fopen(outfile, "wb");

    *nunique = 0;
    *nios = 0;
    int  num_of_output = 0;
    
    int last_num = -1;
    char last_str[STR_LENGTH] = "";

    while (true) {

        //Read a block from file
        fread(&buffer[0], 1, sizeof(block_t), input);
        if (feof(input))
            break;
        *nios=*nios+1;

        //For each record in file
        for (int i = 0; i < MAX_RECORDS_PER_BLOCK; i++) {
        	//If it is the first record add it to the second block
            if (i == 0) {
                if (field == '0') {
                    if (last_num != buffer[0].entries[i].recid) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
                if (field == '1') {
                    if (last_num != buffer[0].entries[i].num) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
                if (field == '2') {
                    if (strncmp(last_str, buffer[0].entries[i].str, STR_LENGTH) != 0) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
                if (field == '3') {
                    if (strncmp(last_str, buffer[0].entries[i].str, STR_LENGTH) != 0 || last_num != buffer[0].entries[i].num) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
            }
            else {
                //If it is the last record keep it's values in variables
                //Then check if this record is unique and if it is write it to the buffer
                if (field == '0') {
                    if (i == MAX_RECORDS_PER_BLOCK - 1) {
                        last_num = buffer[0].entries[i].recid;
                    }
                    if (buffer[0].entries[i].recid != buffer[0].entries[i - 1].recid) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
                if (field == '1') {
                    if (i == MAX_RECORDS_PER_BLOCK - 1) {
                        last_num = buffer[0].entries[i].num;
                    }
                    if (buffer[0].entries[i].num != buffer[0].entries[i - 1].num) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
                if (field == '2') {
                    if (i == MAX_RECORDS_PER_BLOCK - 1) {
                        strcpy(last_str, buffer[0].entries[i].str);
                    }
                    if (strncmp(buffer[0].entries[i].str, buffer[0].entries[i - 1].str, STR_LENGTH) != 0) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
                if (field == '3') {
                    if (i == MAX_RECORDS_PER_BLOCK - 1) {
                        last_num = buffer[0].entries[i].num;
                        strcpy(last_str, buffer[0].entries[i].str);
                    }
                    if (strncmp(buffer[0].entries[i].str, buffer[0].entries[i - 1].str, STR_LENGTH) != 0 || buffer[0].entries[i].num != buffer[0].entries[i - 1].num) {
                        memcpy(&buffer[1].entries[num_of_output], &buffer[0].entries[i], sizeof(record_t));
                        *nunique=*nunique+1;
                        num_of_output++;
                    }
                }
            }
            //If block is full write it's records to the file
            if (num_of_output == MAX_RECORDS_PER_BLOCK) {
                num_of_output = 0;

                for (int j = 0; j < MAX_RECORDS_PER_BLOCK; ++j) {
                    fwrite(&buffer[1].entries[j], 1, sizeof(record_t), output);
                    *nios=*nios+1;
                }
            }
        }
    }
    fclose(input);
    
    //Write records that might be in the buffer
    if (num_of_output > 0) {
        for (int j = 0; j < num_of_output; j++){
            fwrite(&buffer[1].entries[j], 1, sizeof(record_t), output);
            *nios=*nios+1;}

    }
    fclose(output);

    
    cout<< "----ELIMINATE DUPLICATES----" << endl;
    cout << "Unique records :  " << *nunique << endl;
    cout << "IOs done :  " << *nios <<" "<<endl;

    return; 
    
}

void MergeJoin(char* infile1, char* infile2, unsigned char field, block_t* buffer, unsigned int nmem_blocks, char* outfile, unsigned int* nres, unsigned int* nios)
{
    FILE *input1, *input2, *join;
    bool flag = true, check;
    bool file1Ended=false,file2Ended=false;

    //Open input files and output file
    input1 = fopen(infile1, "rb");
    input2 = fopen(infile2, "rb");
    join = fopen(outfile, "wb");

    //Pointers to entries array position
    buffer[0].bdummy1 = 0;//Number of records checked
    buffer[0].bdummy2 = 0;//Number of elements in block
    buffer[1].bdummy1 = 0;//Number of records checked
    buffer[1].bdummy2 = 0;//Number of elements in block
    buffer[2].bdummy1 = 0; //Number of records inserted

    //Read 1 block from each file
    for (int i = 0; i < MAX_RECORDS_PER_BLOCK; i++) {
    	//Read records from file unless they reached the end
        if (!file1Ended) {
            fread(&buffer[0].entries[i], 1, sizeof(record_t), input1);
            if (feof(input1))
                file1Ended=true;
            else {
                buffer[0].bdummy2++;
                *nios=*nios+1;
            }
        }
        if (!file2Ended) {
            fread(&buffer[1].entries[i], 1, sizeof(record_t), input2);
            if (feof(input2))
                file2Ended=true;
            else {
                buffer[1].bdummy2++;
                *nios=*nios+1;
            }
        }
        if(file2Ended && file1Ended)
            break;
    }

    while (flag) {
        //Set check considering the field
        if (field == '0')
            check = buffer[0].entries[buffer[0].bdummy1].recid == buffer[1].entries[buffer[1].bdummy1].recid;
        else if (field == '1')
            check = buffer[0].entries[buffer[0].bdummy1].num == buffer[1].entries[buffer[1].bdummy1].num;
        else if (field == '2')
            check = strcmp(buffer[0].entries[buffer[0].bdummy1].str, buffer[1].entries[buffer[1].bdummy1].str) == 0;
        else if (field == '3')
            check = (buffer[0].entries[buffer[0].bdummy1].num == buffer[1].entries[buffer[1].bdummy1].num) && (strcmp(buffer[0].entries[buffer[0].bdummy1].str, buffer[1].entries[buffer[1].bdummy1].str) == 0);
        //If entries have the same field write them to the file
	    if (check) {
            //Put record to buffer
            buffer[2].entries[buffer[2].bdummy1] = buffer[0].entries[buffer[0].bdummy1];
            buffer[2].entries[buffer[2].bdummy1].rdummy1 = 1; //Which file the record came from
            buffer[2].bdummy1++;

            //If block is full write it to file
            if (buffer[2].bdummy1 == MAX_RECORDS_PER_BLOCK) {
                for (int k = 0; k < buffer[2].bdummy1; k++) {
                    fwrite(&buffer[2].entries[k], 1, sizeof(record_t), join);
                    *nios=*nios+1;
                }
                buffer[2].bdummy1 = 0;
            }

            //Repeat for the other record
            buffer[2].entries[buffer[2].bdummy1] = buffer[1].entries[buffer[1].bdummy1];
            buffer[2].entries[buffer[2].bdummy1].rdummy1 = 2;
            buffer[2].bdummy1++;

            if (buffer[2].bdummy1 == MAX_RECORDS_PER_BLOCK) {
                for (int k = 0; k < buffer[2].bdummy1; k++) {
                    fwrite(&buffer[2].entries[k], 1, sizeof(record_t), join);
                    *nios=*nios+1;
                }
                buffer[2].bdummy1 = 0;
            }

            *nres=*nres+1; //Number of pairs
        }

            if (field == '0')
                check = buffer[0].entries[buffer[0].bdummy1].recid > buffer[1].entries[buffer[1].bdummy1].recid;
            else if (field == '1')
                check = buffer[0].entries[buffer[0].bdummy1].num > buffer[1].entries[buffer[1].bdummy1].num;
            else if (field == '2')
                check = strcmp(buffer[0].entries[buffer[0].bdummy1].str, buffer[0].entries[buffer[0].bdummy1].str) > 0;
            else if (field == '3') {
                check = buffer[0].entries[buffer[0].bdummy1].num > buffer[1].entries[buffer[1].bdummy1].num;
                if (buffer[0].entries[buffer[0].bdummy1].num == buffer[1].entries[buffer[1].bdummy1].num)
                    check = strcmp(buffer[0].entries[buffer[0].bdummy1].str, buffer[0].entries[buffer[0].bdummy1].str) > 0;
            }

            //If first block value is bigger increase second block pointer
            if (check)
                buffer[1].bdummy1++;
            else
                buffer[0].bdummy1++;

        //Read next part of the files (if the previous are checked)
        for (int k = 0; k < 2; k++) {
            if (buffer[k].bdummy1 == buffer[k].bdummy2) {
                buffer[k].bdummy2 = 0;
                if (k == 0) {
                    for (int t = 0; t < MAX_RECORDS_PER_BLOCK; t++) {
                        fread(&buffer[k].entries[t], 1, sizeof(record_t), input1);
                        if (feof(input1)) {
                            if (t == 0)
                                flag = false;
                            break;
                        }
                        *nios=*nios+1;
                        buffer[k].bdummy2++;
                    }
                }
                else {

                    for (int t = 0; t < MAX_RECORDS_PER_BLOCK; t++) {
                        fread(&buffer[k].entries[t], 1, sizeof(record_t), input2);
                        if (feof(input2)) {
                            if (t == 0)
                                flag = false;
                            break;
                        }
                        *nios=*nios+1;
                        buffer[k].bdummy2++;
                    }
                }
                buffer[k].bdummy1 = 0;
            }
        }
    }

    //Write any records that might be at the buffer
    if (buffer[2].bdummy1 > 0) {
        for (int k = 0; k < buffer[2].bdummy1; k++) {
            fwrite(&buffer[2].entries[k], 1, sizeof(record_t), join);
            *nios=*nios+1;
        }
    }
    //Close files
    fclose(join);
    fclose(input1);
    fclose(input2);

    cout<< "----MERGE JOIN----" << endl;
    cout << "Res:  " << *nres << endl;
    cout << "IOs: " << *nios << endl;
    return;
}

unsigned int toInt(char c){
	//returns the int value for the field
    switch (c) {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    }
}

int cmprRecs(record_t rec1, record_t rec2, int field){
	//used to compare two records on a certain field
    switch (field) {
    case 0:
        if (rec1.recid < rec2.recid) {
            return -1;
        }
        else if (rec1.recid > rec2.recid) {
            return 1;
        }
        return 0;
    case 1:
        if (rec1.num < rec2.num) {
            return -1;
        }
        else if (rec1.num > rec2.num) {
            return 1;
        }
        return 0;
    case 2:
        if (strcmp(rec1.str, rec2.str) < 0) {
            return -1;
        }
        else if (strcmp(rec1.str, rec2.str) > 0) {
            return 1;
        }
        return 0;
    case 3:
        if ((rec1.num < rec2.num) || (rec1.num == rec2.num && strcmp(rec1.str, rec2.str) < 0)) {
            return -1;
        }
        else if ((rec1.num > rec2.num || (rec1.num == rec2.num && strcmp(rec1.str, rec2.str) > 0))) {
            return 1;
        }
        return 0;
    }
}

unsigned int hashInt(unsigned int num, unsigned int mod, unsigned int seed) {
	// hash function for integers
	return (seed*num)%mod;
}

unsigned int hashString(char *str, unsigned int mod, unsigned int seed) {
	// hash function for strings
   int length = sizeof(str)/sizeof(*str);
   int i, sum;
   for (sum=0, i=0; i < length; i++)
		sum += str[i];
   return (sum*seed) % mod;
}

unsigned int hashRecord(record_t rec, unsigned int mod, unsigned int field) {
    //hashes a record based on a certain field and returns the hash index, for the corresponding bucket
    //uses hashInt() and hashString()
	int s=2654435761;//prime number for even distribution
    switch (field) {
        case 0:
            return hashInt(rec.recid, mod, s);
        case 1:
            return hashInt(rec.num, mod, s);
        case 2:
            return hashString(rec.str, mod, s);
        case 3:
            return hashInt(rec.num + hashString(rec.str, mod, s), mod, s);
    }
}

char* getBucketName(unsigned int i){
	//returns the name of the bucket for the index = i 
    char* str = (char*)malloc((strlen("hj") + 11) * sizeof(char));
    sprintf(str, "hj_%u", i);
    return str;
}

void phaseTwo(char* infile, unsigned int sizeOfFile, block_t* buffer, FILE* out, unsigned int* nres, unsigned int* nios, unsigned int field, int mod){
    bool file1Ended = false;
	buffer[2].bdummy1=0;
    //for the second file
    FILE* file = fopen(infile, "rb"); //to read one-by-one block of records from the second file
    for (unsigned int i = 0; i < sizeOfFile; i++) {//for each block in infile
	buffer[0].bdummy1=0; 
        //Read each record from the file
        for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            //Read records from file unless they reached the end
            if (!file1Ended) {
                fread(&buffer[0].entries[j], 1, sizeof(record_t), file);
                if (feof(file))
                    file1Ended = true;
                else {
                    buffer[0].bdummy1=buffer[0].bdummy1+1;
                    *nios = *nios + 1;
                }
            }
        }
        //for each record loaded in buffer[0]
        for (int j = 0; j < buffer[0].bdummy1; j++) {
            record_t record = buffer[0].entries[j]; //get a record
            if (record.valid) { //if the record is valid
                unsigned int index = hashRecord(record, mod, field); //HASH THE RECORD
                //index is the number of the bucket that it corresponds to
                //empty the buffer[1]
    			buffer[1].bdummy1 = 0;
    			for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
        			buffer[1].entries[l].valid = false;
    			}
    			//load in buffer[1] the bucket[index]
                char* filename = getBucketName(index);
                //get bucket file size
                ifstream buc(filename, ios::binary | ios::ate);
    			unsigned int sizeBuc = buc.tellg() / sizeof(record_t);
    			sizeBuc=ceil(float(sizeBuc)/float(MAX_RECORDS_PER_BLOCK));
    			buc.close();
                bool fileEnded = false;
                FILE* bucket = fopen(filename, "rb"); //to read one-by-one block of records from the second file
                for (unsigned int p = 0; p < sizeBuc; p++) {//for each block in bucket
                	for (unsigned int i = 0; i < MAX_RECORDS_PER_BLOCK; i++) { //for each record in bucket
                    	//Read records from file unless they reached the end
                    	if (!fileEnded) {
                        	fread(&buffer[1].entries[i], 1, sizeof(record_t), bucket);
                        	if (feof(bucket)) {
                            	fileEnded = true;
                        	}else {
                            	buffer[1].bdummy1= buffer[1].bdummy1+1;
                            	*nios = *nios + 1;
                        	}
                    	}
                	}
                	//compare each record in buffer[1](bucket) to the hashed record
                	for (int k = 0; k < buffer[1].bdummy1; k++) {
                    	record_t record2 = buffer[1].entries[k]; //get a record
                    	if (cmprRecs(record, record2, field) == 0) { //compare the records
                        	//if they are equal
                        	//if they are the same copy the pair in buffer[2]
                        	buffer[2].entries[buffer[2].bdummy1] = record;
                        	buffer[2].entries[buffer[2].bdummy1].rdummy1=1;
                        	buffer[2].bdummy1=buffer[2].bdummy1+1;
                        	buffer[2].entries[buffer[2].bdummy1] = record2;
                        	buffer[2].entries[buffer[2].bdummy1].rdummy1=2;
                        	buffer[2].bdummy1=buffer[2].bdummy1+1;
                        	(*nres) += 1;
                        	// if output block is full, write everything on the output file and empty buffer[2]
                        	if (buffer[2].bdummy1 == MAX_RECORDS_PER_BLOCK) {
                            	for (int o = 0; o < MAX_RECORDS_PER_BLOCK; o++) {
                                	fwrite(&buffer[2].entries[o], 1, sizeof(record_t), out);
                                	*nios = *nios + 1;
                            	}
                            	//empty the block
                            	buffer[2].bdummy1 = 0;
                            	for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
                                	buffer[2].entries[l].valid = false;
                            	} //end of empting the file
                        	} //end of coping the buffer[2] to the outputfile
                    	} //end of comparing the records
                	} //end of parsing the bucket buffer
                }//end of processing the ucket file
                fclose(bucket);
            } //end of processing a certain record of the second file
        } //end of the loaded block for examination
    } //end of blocks in the second file
    fclose(file); 
}

void generateBuckets(char* filename, unsigned int size, block_t* buffer, unsigned int mod, unsigned int* nios, unsigned int field){
    //creates bucket files according to the smallest file, so that each bucket can fit a block of records
    //and hashes the smallest file in the buckets
	bool fileEnded=false;
	//empty the block
    buffer[0].bdummy1 = 0;
    for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
        buffer[0].entries[l].valid = false;
    }
    FILE* file = fopen(filename, "rb");
    FILE* bucketFilenames[mod];
     for(int i=0;i<mod;i++)
             bucketFilenames[i]=fopen(getBucketName(i), "wb");
    for (unsigned int i = 0; i < size; i++) { //for each block in infile JK
        //Read each record from the file
        for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            //Read records from file unless they reached the end
            if (!fileEnded) {
                fread(&buffer[0].entries[j], 1, sizeof(record_t), file);
                if (feof(file)){
                    fileEnded = true;
                }else {
                    buffer[0].bdummy1=buffer[0].bdummy1+1;
                    *nios = *nios + 1;
                }
            }
            else
              break;
        }
        // each record of the current block is hashed
        for (int j = 0; j < buffer[0].bdummy1; j++) {
            if (buffer[0].entries[j].valid) {
                unsigned int index = hashRecord(buffer[0].entries[j], mod, field);
                //and added to the bucket file it belongs to            
                fwrite(&buffer[0].entries[j], 1, sizeof(record_t),bucketFilenames[index]);
                *nios = *nios + 1;
            }
        }
        //empty the block
        buffer[0].bdummy1 = 0;
        for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
            buffer[0].entries[l].valid = false;
        } 
    }//end of the file
    fclose(file);
    for(int i=0;i<mod;i++)
            fclose(bucketFilenames[i]);
}

int partition(char* infile1, unsigned int size1, char* infile2, unsigned int size2, unsigned int field, block_t* buffer, unsigned int memSize, unsigned int* nres, unsigned int* nios, bool firstCall, char**& filenames){
    //figures out how many buckets should be created(if any) and calls generateBuckets() to create them
	//returns the number of buckets that exist
	//if the hash join can occur without any buckets(single pass) returns 0
    if (size1 < memSize || size2 < memSize) {//single pass
        return 0;//no buckets
    }else {
        // if the infiles cannot be joined in a single pass, creates subfiles (bucketfiles)
        // and recursively calls partition for each pair of them
        // figure out how many buckets to create
        unsigned int smalls;
        if (size2 < size1) {
            smalls = size2;
        }else {
            smalls = size1;
        }
        unsigned int bucketCount = ceil(float(smalls*MAX_RECORDS_PER_BLOCK )/ float(MAX_RECORDS_PER_BLOCK));
        // calls generateBuckets() for the smallest file
        if (smalls == size1) {
            generateBuckets(infile1, size1, buffer,  bucketCount, nios, field);
        }else {
            generateBuckets(infile2, size2, buffer,  bucketCount, nios, field);
        }
        return bucketCount;
    } //end of partitioning (BUCKETS) the files
}

void HashJoin(char* infile1, char* infile2, unsigned char field, block_t* buffer, unsigned int nmem_blocks, char* outfile, unsigned int* nres, unsigned int* nios){
    //initialize
    (*nres) = 0;
    (*nios) = 0;
    //the names of the buckets are stored here
    char** filenames;
    //FIRST PHASE
    //get the size for each file 
    ifstream file1(infile1, ios::binary | ios::ate);
    unsigned int size1 = file1.tellg() / sizeof(record_t);
    size1=ceil(float(size1)/float(MAX_RECORDS_PER_BLOCK));
    ifstream file2(infile2, ios::binary | ios::ate);
    unsigned int size2 = file2.tellg() / sizeof(record_t);
    size2=ceil(float(size2)/float(MAX_RECORDS_PER_BLOCK));
    file1.close();
    file2.close();
    // partition the smallest file and hash it into buckets, returns a bool value to know which file was hashed(smallest)
    unsigned int mod=partition(infile1, size1, infile2, size2, toInt(field), buffer, nmem_blocks - 1, nres, nios, true, filenames);
    //empty the buffer[0]
    buffer[0].bdummy1 = 0;
    for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
        buffer[0].entries[l].valid = false;
    }
    //empty the buffer[1]
    buffer[1].bdummy1 = 0;
    for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
        buffer[1].entries[l].valid = false;
    }
    //empty the buffer[2]
    buffer[2].bdummy1= 0;
    for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
        buffer[2].entries[l].valid = false;
    }
    //SECOND PHASE
    //open the out file to write the output
    FILE* out = fopen(outfile, "wb");
    if(mod==0){
    	//single pass hash-join
    	char* infile;
		char* file2;
    	int sizeOfFile, sizeOfFile2;
    	if(size1<nmem_blocks - 1){
    		infile=infile1;
    		file2=infile2;
    		sizeOfFile=size1;
    		sizeOfFile2=size2;
    	}else if(size2<nmem_blocks - 1){
    		infile=infile2;
    		file2=infile1;
    		sizeOfFile=size2;
    		sizeOfFile2=size1;
		}
		bool fileEnded=false;
    	FILE* file = fopen(infile, "rb"); //to read one-by-one block of records from the second file
    	for (unsigned int i = 0; i < sizeOfFile; i++) { //for each block in infile
        	//Read each record from the file
        	for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            	//Read records from file unless they reached the end
            	if (!fileEnded) {
                	fread(&buffer[0].entries[j], 1, sizeof(record_t), file);
                	if (feof(file))
                    	fileEnded = true;
                	else {
                    	buffer[0].bdummy1=buffer[0].bdummy1+1;
                    	*nios = *nios + 1;
                	}
            	}
        	}
        }
        fclose(file);
        //By now we have loaded the smallest file in one buffer ([0])-single pass
        bool fileEnded2=false;
    	FILE* otherfile = fopen(file2, "rb"); //to read one-by-one block of records from the second file
    	for (unsigned int i = 0; i < sizeOfFile2; i++) { //for each block in infile
        	//Read each record from the file
        	for (int j = 0; j < MAX_RECORDS_PER_BLOCK; j++) {
            	//Read records from file unless they reached the end
            	if (!fileEnded2) {
                	fread(&buffer[1].entries[0], 1, sizeof(record_t), otherfile);
                	record_t record=buffer[1].entries[0];
                	if (feof(otherfile))
                    	fileEnded2 = true;
                	else {
                    	*nios = *nios + 1;
                	}
                	//compare the record with every record in buffer[0] and write them to buffer[2] if they are the same
                for (int k = 0; k < buffer[0].bdummy1; k++) {
                    record_t record2 = buffer[0].entries[k]; //get a record
                    if (cmprRecs(record, record2, field) == 0) { //compare the records
                        //if they are equal
                        //if they are the same copy the pair in buffer[2]
                        buffer[2].bdummy1=buffer[2].nreserved;
                        buffer[2].bdummy1=buffer[2].bdummy1+1;
                        buffer[2].entries[buffer[2].bdummy1] = record;
                        buffer[2].bdummy1=buffer[2].bdummy1+1;
                        buffer[2].entries[buffer[2].bdummy1] = record2;
                        (*nres) += 1;
                        // if output block becomes full, writes it to the outfile
                        // and empties it
                        if (buffer[2].nreserved == MAX_RECORDS_PER_BLOCK) {
                            for (int o = 0; o < MAX_RECORDS_PER_BLOCK; o++) {
                                fwrite(&buffer[2].entries[o], 1, sizeof(record_t), out);
                                *nios = *nios + 1;
                            }
                            //empty the block
                            buffer[2].bdummy1 = 0;
                            for (int l = 0; l < MAX_RECORDS_PER_BLOCK; l++) {
                                buffer[2].entries[l].valid = false;
                            } //end of empting the file
                        } //end of coping the buffer[2] to the outputfile
                    } //end of comparing the records
                } //end of parsing the bucket
            	}//if the file ended
        	}
		}
        fclose(otherfile);
	}else{
		//NOT single pass hash-join
    	//HASH WHAT THERE IS IN 2ND FILE(the bigger file) WITH BUFFER ON THE OUTPUT
    	if (size1 > size2) {
        	phaseTwo(infile1, size1, buffer, out, nres, nios, toInt(field), mod);
    	}else {
        	phaseTwo(infile2, size2, buffer, out, nres, nios, toInt(field), mod);
    	}
    	for(int i=0;i<mod;i++)
    	remove(getBucketName(i));
	}//end of second phase
	
    //if any pairs remained write them to the output file
    if (buffer[2].bdummy1 != 0) {
        for (int o = 0; o < buffer[2].bdummy1; o++) {
            fwrite(&buffer[2].entries[o], 1, sizeof(record_t), out);
            *nios = *nios + 1;
        }

    }
    fclose(out);
     cout<< "----HASH JOIN----" << endl;
    cout << "Res:  " << *nres << endl;
    cout << "IOs: " << *nios << endl;
}

