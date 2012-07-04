#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#ifndef _FREQBEENCHECKED_HH_
#define _FREQBEENCHECKED_HH_

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
#endif
