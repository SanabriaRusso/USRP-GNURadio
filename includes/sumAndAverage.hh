#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#ifndef _SUMANDAVERAGE_HH_
#define _SUMANDAVERAGE_HH_


void sumAndAverage(duplex *container, int length, const char *outputFile, double sampleRate, int print){

    duplex averagePower [length];
    float cumm = 0.0;
    float freq [length];
    float freqIterator = 0.0;
    int match = 1;
    bool check;
    
    for(int f = 0; f < length; f++){
        freq[f] = 0.0;
        averagePower[f].freq = 0.0;
        averagePower[f].power = 0.0;
    }
    
    for(int i = 0; i < length; i++){
        
        if(print == 1) cout << "Performing check in: " << container[i].freq << " Hz" << endl;
        check = freqBeenChecked(freq, length, (container[i].freq), print);
        
        //If the frequency has not been checked before, then:
        if(check == false){
            freqIterator = (container[i].freq);
            cumm = (container[i].power);
            if(print == 1) cout << "testing: " << freqIterator << " Hz" << endl;
            
            for(int j = i + 1; j < length; j++){
                if(freqIterator == (container[j].freq)){
                    if(print == 1) cout << freqIterator << " is equal to " << container[j].freq << " in position " << j << endl;
                    cumm += (container[j].power);
                    match++;
                }
            }
            freq[i] = freqIterator;
            (averagePower[i].freq) = freqIterator;
            (averagePower[i].power) = cumm / match;

        }
        match = 1;
        cumm = 0.0;
    }
    
    if(print == 1){
    	    displayContainer(averagePower, length);
    	    displayArray(freq, length);
    }
    
    //Sending it to a function that removes the zeros, sorts the vector and outputs it to a file
    removeZeros(averagePower, length, outputFile, sampleRate, print);
}

#endif
