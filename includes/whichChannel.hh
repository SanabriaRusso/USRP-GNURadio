#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#include "def.hh"

#ifndef _WHICHCHANNEL_HH_
#define _WHICHCHANNEL_HH_

//returns which channel the input frequency is
//if it is outside the TV UHF band, then a zero is returned

/*Convert it to float return if you want to get the plotRawPowers from
the first file output of order_powers.cpp*/

int whichChannel(float freq){
	int channel = 0;
	float channel21 = 4.7125e08;
	float channel69 = 8.6325e08;
	float channelWidth = 8e06;
	
	//Finding the channel to which this freq corresponds
	if((freq >= channel21) && (freq <= channel69)){
		channel = ((freq - channel21) / channelWidth) + 21.0;
	}
	else channel = 0.0;
	
	return channel;
}

#endif
