#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#ifndef _CHANNELISEQUAL_HH_
#define _CHANNELISEQUAL_HH_

bool channelIsEqual(whiteSPC &channel1, whiteSPC &channel2){
	
	if(channel1.channel == channel2.channel) return true;
	
	return false;
}

#endif
