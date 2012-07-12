#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <set>

using namespace std;

#include "def.hh"
#include "whichChannel.hh"

#ifndef _ADAPTEDPLOT_HH_
#define _ADAPTEDPLOT_HH_


//This function just orders the available WS by their TV channel number
//and ouputs a file in the manner of Albert's file F(TVchannel) = used/available

/*Reformulate the problem. You need to get the average of a repeated channel. 
Find another way. You already did the code, but increased it to desproportionate 
dimmensions.*/

void adaptedPlot(std::vector<duplex> allSweep, whiteSPC *inputWS, int linesInput, int print){
	
	whiteSPC plotWS [linesInput];
	std::vector<duplex>::iterator it;
	int firstChannel, lastChannel;
	
	firstChannel = 21;
	lastChannel = 69;
	
	//Filling the array with initial values
	for(int i = 0; i <= linesInput; i++){
		plotWS[i].channel = inputWS[i].channel;
		plotWS[i].power = 0.0;
		plotWS[i].use = 0;
	}
	
	
	//Doing the comparison and average
	int referenceChannel, sweptChannel;
	
	for(int j = 0; j <= linesInput; j++){
		referenceChannel = plotWS[j].channel;
		for(it = allSweep.begin(); it != allSweep.end(); it++){
			sweptChannel = whichChannel(it->freq);
			if(sweptChannel == referenceChannel){
				plotWS[j].use++;
				plotWS[j].power += (it->power);
			}
			else continue;
		}
	}
	
	float full = -65.0;
	char const *plotFile = "plotChannels.txt";
	FILE *channelsAndPower;
	channelsAndPower = fopen(plotFile, "a");
	
	for(int l = 0; l <= linesInput; l++){
		// cout << "Channel: " << plotWS[l].channel << " Power: " << plotWS[l].power / plotWS[l].use << " Repetitions: " << plotWS[l].use << endl;
		if((firstChannel <= plotWS[l].channel) && (plotWS[l].channel <= lastChannel)){
			if(fabs(plotWS[l].power) > 0){
				//cout << "Channel: " << plotWS[l].channel << endl;
				fprintf(channelsAndPower, "%d %f\n", plotWS[l].channel, plotWS[l].power / plotWS[l].use);	
			}
			else{
				fprintf(channelsAndPower, "%d %f\n", plotWS[l].channel, full);	
			}
		}
	}
	
	fclose(channelsAndPower);
	
	
	
}

#endif
