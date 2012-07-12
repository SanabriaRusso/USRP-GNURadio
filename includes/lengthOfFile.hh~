#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>


#ifndef _LENGTHOFFILE_HH_
#define _LENGTHOFFILE_HH_

int lengthOfFile(ifstream* file){
    
    string line;
    int size = 0;   //determining the size of the arrays
    while(!file->eof()){
        getline(*file, line);
        size++;
    }
    return size;
}

#endif
