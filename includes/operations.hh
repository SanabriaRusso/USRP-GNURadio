#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#include "def.hh"
#include "whiteSearch.hh"

using namespace std;

bool freqSort(duplex const& freq1, duplex const& freq2)
{
    if(freq1.freq < freq2.freq) return true;
    if(freq1.freq > freq2.freq) return false;
    
    return false;
}

int lengthOfFile(ifstream* file){
    
    string line;
    int size = 0;   //determining the size of the arrays
    while(!file->eof()){
        getline(*file, line);
        size++;
    }
    return size;
}

void displayContainer(duplex *container, int length){
    
    for(int i = 0; i < length; i++){
    	    cout << "The frecuency: "<< container[i].freq << " Hz, has: " << container[i].power << " dBm" << endl;
    }

}

void displayArray(float *array, int length){

	for(int i = 0; i < length; i++){
		cout << array[i] << " ";
	}
	cout << endl;
}

int freqBeenChecked(float *checkedFreq, int length, float freq, int print){
    
    bool checked = false;
    
    if(print == 1){
    	    cout << "Vector to be searched for: " << freq << endl;
    	    displayArray(checkedFreq, length);
    }
    
    for(int h = 0; h < length; h++){
        if (checked == false) {
            if(freq == checkedFreq[h]){
                checked = true;
            }
            else{ 
                checked = false;
            }

        }
    }
    
    
    if(print == 1) cout << "Final return: " << checked << endl;
    
    return (checked);
}

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
