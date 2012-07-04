#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#include "freqSort.hh"
#include "whiteSearch.hh"

#ifndef _REMOVEZEROS_HH_
#define _REMOVEZEROS_HH_

void removeZeros(duplex *container, int length, const char *outputFile, double sampleRate, int print){
    
    vector<duplex> fullContainer;
    vector<duplex>::iterator it; //for frequency and power
    int realLength = 0;
    
    for (int i = 0; i < length; i++) {
        if(container[i].freq > 0){
            fullContainer.push_back(container[i]);
            realLength++;
        }
        else{
            continue;
        }
    }
    
    //Finding White Spaces
    //Assign print variable a value of 3 to see this output
    whiteSearch(fullContainer, sampleRate, outputFile, print);
    
    //Sorting the orderedContainer without zeros
    
    std::sort(it = fullContainer.begin(), it = fullContainer.end(), freqSort);
    
    FILE *avgPowers;
    avgPowers = fopen(outputFile, "a");
    
    for(it = fullContainer.begin(); it != fullContainer.end(); it++){
        fprintf(avgPowers, "%f %f\n", (it->freq), (it->power));
    }
     fclose(avgPowers);
}

#endif
