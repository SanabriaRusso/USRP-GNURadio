#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>


using namespace std;

#ifndef _FREQSORT_HH_
#define _FREQSORT_HH_

bool freqSort(duplex const& freq1, duplex const& freq2)
{
    if(freq1.freq < freq2.freq) return true;
    if(freq1.freq > freq2.freq) return false;
    
    return false;
}

#endif
