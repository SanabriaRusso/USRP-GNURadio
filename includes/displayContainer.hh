#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#ifndef _DISPLAYCONTAINER_HH_
#define _DISPLAYCONTAINER_HH_

void displayContainer(duplex *container, int length){
    
    for(int i = 0; i < length; i++){
    	    cout << "The frecuency: "<< container[i].freq << " Hz, has: " << container[i].power << " dBm" << endl;
    }

}
#endif


