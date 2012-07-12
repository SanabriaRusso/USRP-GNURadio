#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

#include "whiteDisplay.hh"
#include "def.hh"
#include "adaptedPlot.hh"

//Assign print variable a value of 3 to see this output

using namespace std;

#ifndef _WHITESEARCH_HH_
#define _WHITESEARCH_HH_

void whiteSearch(std::vector<duplex> container, double sampleRate, const char *outputFile, int print){
	
	vector<duplex>::iterator it;
	vector<channelPower> availableChannels;
	double cumPowers = 0.0;
	double meanPower = 0.0;
	double cumSTD = 0.0;
	double stdPower = 0.0;
	
	//-----Simple limits for the gathered data-----//
	
	if(print == 3) cout << "Finding the mean power" << endl;

	for(it = container.begin(); it != container.end(); it++){
		cumPowers += (it->power);
	}

	meanPower = cumPowers / container.size();
	
	if(print == 3) cout << "The mean power is: " << meanPower << endl;

	if(print == 3) cout << "Calculating the standard deviation" << endl;
	
	for(it = container.begin(); it != container.end(); it++){
		cumSTD += fabs(meanPower - (it->power));
	}
		
	stdPower = sqrt((1.0/(container.size() - 1)) * cumSTD);
	
	if(print == 3) cout << "The STD is: " << stdPower << endl;
	
	//-----end-----//
	
	//-----Second option: the midpoint between max and min values-----//
	
	float minPower, maxPower;
	
	minPower = 0.0;
	maxPower = -300.0;
	
	//Finding the minimum power = noise floor
	for(it = container.begin(); it != container.end(); it++){
		if((it->power) < minPower) minPower = it->power;
		else continue;
	}
	
	//Finding the maximum power = signal ceiling
	for(it = container.begin(); it != container.end(); it++){
		if((it->power) > maxPower) maxPower = it->power;
		else continue;
	}
	
	//Finding the second threshold based on noise floor and signal ceiling
	
	float threshold;
	
	threshold = minPower + (maxPower - minPower) / 2;
	
	if(print == 3) cout << "Noise floor: " << minPower << ", signal ceiling: " << maxPower << ", threshold: " << threshold << endl;
	
	
	
	
	
	
	//-----Determining which powers are definetely identifyable-----//
	
	/*the limits constants allows for moving the limits that define
	the state of the incomming powers. As these constants get bigger,
	more tolerance is allowed to their respective orientation.*/
	
	float alpha = 0.0; //upperLimit constant
	float beta = 0.0; //lowerLimit constant
	
	/*Using the average as threshold
	float upperLimit = meanPower + (alpha * stdPower);
	float lowerLimit = meanPower - (beta * stdPower);*/
	
	//Using the midpoint between floor and ceiling as threshold
	float upperLimit = threshold + (alpha * stdPower);
	float lowerLimit = threshold - (beta * stdPower);
	
	channelPower available;
	
	if(print == 3) cout << "mean: " << meanPower<< ", up: " << upperLimit << ", down: " << lowerLimit << endl;
	
	//------------------------------------//
	//Averaging the powers based on channels
	//------------------------------------//
	int firstChannel, lastChannel;
	firstChannel = 21;
	lastChannel = 69;
	
	int TVChannels = lastChannel - firstChannel;
	whiteSPC channelPowers [TVChannels];
	int count = 0;
	
	
	
	//Filling channelPowers with channel numbers
	
	for(int i = firstChannel; i <= lastChannel; i++){
		channelPowers[count].channel = i;
		channelPowers[count].use = 0;
		channelPowers[count].license = 0; //see developing below
		channelPowers[count].power = 0.0;
		if(i != lastChannel) count++;
	}
	
	int referenceCh;
	vector<duplex>::iterator it2;
	
	/*There is one problem when analyzing the average power on the whole channel,
	that is: there might be a narrowband transmission that would unable us to transmit,
	but if an average is performed, then the algorithm will see the channel as free.
	
	In order to cope with that, you need to make exceptions individually, if there is something
	on that channel, then you must consider it as occupied.
	
	That is why we use .license on the whiteSPC structure to determine if there is at least
	one reading with a USRP-received power over the average (1)*/
	
	for(int i = 0; i <= count; i++){
		referenceCh = channelPowers[i].channel;
		for(it2 = container.begin(); it2 != container.end(); it2++){
			if(whichChannel(it2->freq) == referenceCh){
				channelPowers[i].use++;
				channelPowers[i].power += (it2->power);
				if(it2->power >= upperLimit){ 
					channelPowers[i].license = 1; //only if a reading is higher, then is considered occupied
				}
			}
			else continue;
		}
	}
	
	//averaging the new added powers
	
	for(int i = 0; i <= count; i++){
		if(channelPowers[i].use == 0) channelPowers[i].use++;
		channelPowers[i].power = (channelPowers[i].power) / channelPowers[i].use;
		//if(print == 3) cout << "Channel: " << channelPowers[i].channel << " Power: " << channelPowers[i].power << endl;
	}
	
	//---------------------------------------------------------------------------------//
	//Determining the state of the channel with averaged power and considering .license//
	//---------------------------------------------------------------------------------//
	
	for(int j = 0; j <= count; j++){
		if(channelPowers[j].power == 0){ 
			channelPowers[j].power += upperLimit + 1.0;
			channelPowers[j].license = 1;
		}
		
		if(channelPowers[j].power <= lowerLimit){
			if(channelPowers[j].license == 0){
				available.channel = channelPowers[j].channel;
				available.power = channelPowers[j].power;
			
				availableChannels.push_back(available);
				if(print == 3) cout << "Channel: " << channelPowers[j].channel << " " << channelPowers[j].power << " is lower" << endl;
			}
			
			else{ //in the case of having some narrowband transmission over upperLimit power
				if(print == 3) cout << "Channel: " << channelPowers[j].channel << " may have a narrowband transmission with power over upperLimit" << endl;	
			}
			
			
		}
		else if(channelPowers[j].power >= upperLimit){
			if(print == 3) cout << "Channel: " << channelPowers[j].channel << " " << channelPowers[j].power << " is greater" << endl;
		}
	}
	
	
	
	if(print == 3) cout << "The mean power is: " << meanPower << endl;
	
	if(print == 3) cout << "Noise floor: " << minPower << ", signal ceiling: " << maxPower << ", threshold: " << threshold << endl;
	
	
	FILE *channelUsage; //plots channel, power, usage
	channelUsage = fopen("plotRawData.txt","a");
	for(int i = 0; i <= count; i++){
		fprintf(channelUsage, "%d %f %d\n", channelPowers[i].channel, channelPowers[i].power, channelPowers[i].license);	
	}
	fclose(channelUsage);
	
	
	whiteDisplay(container, availableChannels, sampleRate, outputFile, print);
}	

#endif
