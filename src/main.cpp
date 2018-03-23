

#include "dbtproj.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/time.h>
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{

    block_t* buffer;
    unsigned int nmem_blocks = 5;
    unsigned int nsorted_segs, npasses, nunique, nres, nios ;
    unsigned int recid = 0;
    buffer = (block_t*)malloc(nmem_blocks * sizeof(block_t));

    int nblocks = 1000; // number of blocks in the file
    record_t record;
    block_t block;
    char ch[STR_LENGTH];
    FILE *infile, *outfile, *outfile2, *infile2;

    // generate two files
    outfile = fopen("file.bin", "wb");
    outfile2 = fopen("file2.bin", "wb");

    for (int b = 0; b < nblocks; b++) { // for each block

        block.blockid = b;

        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; r++) { // for each record

            // prepare a record
            record.recid = recid++;
            record.num = rand() % 1000;
            for (int n = 0; n < STR_LENGTH - 1; n++) {
                ch[n] = rand() % 26 + 'a';
                ch[9] = '\0';
            }
            strcpy(record.str, ch);
            record.valid = true;

            memcpy(&block.entries[r], &record, sizeof(record_t)); // copy record to block
        }

        block.nreserved = MAX_RECORDS_PER_BLOCK;
        block.valid = true;

        fwrite(&block, 1, sizeof(block_t), outfile); // write the block to the file
    }

    fclose(outfile);

    recid = 0;
    for (int b = 0; b < nblocks; b++) { // for each block

        block.blockid = b;

        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; r++) { // for each record

            // prepare a record
            record.recid = recid++;
            record.num = rand() % 1000;
            for (int n = 0; n < STR_LENGTH - 1; n++) {
                ch[n] = rand() % 26 + 'a';
                ch[9] = '\0';
            }
            strcpy(record.str, ch);
            record.valid = true;

            memcpy(&block.entries[r], &record, sizeof(record_t)); // copy record to block
        }

        block.nreserved = MAX_RECORDS_PER_BLOCK;
        block.valid = true;

        fwrite(&block, 1, sizeof(block_t), outfile2); // write the block to the file
    }

    fclose(outfile2);

    ofstream out("filename.txt");
    ofstream out2("filename2.txt");
    ofstream out3("filename3.txt");
    ofstream out4("filename4.txt");
    ofstream out5("filename5.txt");

    MergeSort("file.bin", '1', buffer, nmem_blocks, "mergesortOutput.bin", &nsorted_segs, &npasses, &nios);     
    EliminateDuplicates("mergesortOutput.bin", '1', buffer, nmem_blocks,"eliminateOutput.bin", &nunique, &nios);
    MergeSort("file2.bin", '1', buffer, nmem_blocks, "mergesortOutput2.bin", &nsorted_segs, &npasses, &nios);
    
    
    
    record_t r;
    infile = fopen("mergesortOutput2.bin", "rb");
    int nreserved;
    while (true) { // while end-of-file has not been reached ...
        fread(&block, 1, sizeof(block_t), infile); // read the next block
        if (feof(infile))
            break;
        nreserved = block.nreserved;

        // print block contents
        for (int i = 0; i < nreserved; ++i) {
            if (out)
                out << "This is block id: " << block.entries[i].recid << " " << block.entries[i].num << " " << block.entries[i].str << endl;
        }
    }
    fclose(infile);
    
    
    
     EliminateDuplicates("mergesortOutput2.bin", '1', buffer, nmem_blocks,"eliminateOutput2.bin", &nunique, &nios);



    infile = fopen("eliminateOutput2.bin", "rb");
    while (true) { // while end-of-file has not been reached ...
        fread(&r, 1, sizeof(record_t), infile); // read the next block
        if (feof(infile))
            break;
            if (out3)
                out3 << "This is block id: " << r.recid << " " << r.num << " " << r.str << endl;
    }
    fclose(infile);
    
    
    
    
    MergeJoin("eliminateOutput.bin","eliminateOutput2.bin",'1',buffer, nmem_blocks,"mergeJoinOutput.bin",&nres,&nios);

    
    
   infile = fopen("mergeJoinOutput.bin", "rb");
    while (true) { // while end-of-file has not been reached ...
        fread(&r, 1, sizeof(record_t), infile); // read the next block
        if (feof(infile))
            break;
            if (out5)
                out5 << "This is block id: " << r.recid << " " << r.num << " " << r.str << endl;
    }
    fclose(infile);
	
	HashJoin("eliminateOutput.bin","eliminateOutput2.bin",'1',buffer, nmem_blocks,"hashJoinOutput.bin",&nres,&nios);
	
	
	infile = fopen("hashJoinOutput.bin", "rb");
    while (true) { // while end-of-file has not been reached ...
        fread(&r, 1, sizeof(record_t), infile); // read the next block
        if (feof(infile))
            break;
            if (out4)
                out4 << "This is block id: " << r.recid << " " << r.num << " " << r.str << endl;
    }
    fclose(infile);

    return 0;
}



