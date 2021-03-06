#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string.h>

#include "whichChannel.hh"

#ifndef _WHITEDISPLAY_HH_
#define _WHITEDISPLAY_HH_

//Assign print variable a value of 4 to see this output

using namespace std;

void whiteDisplay(std::vector<duplex> allSweep, std::vector<channelPower> availableChannels, double sampleRate, const char *outputFile, int print){

	vector<channelPower>::iterator it;
	string suffix ("_whiteSpaces");
	stringstream ss;
	string sOutput;
		
	//appending an identificator for the file
	ss << outputFile;
	ss << suffix;
	ss >> sOutput;
	
	//generating a char from the transformed string
	char *output = new char[sOutput.size() + 1];
	output[sOutput.size()] = 0;
	memcpy(output, sOutput.c_str(), sOutput.size());
	
	int channel = 0;
	
	if(print == 4){ 
		cout << "Sample rate = " << sampleRate << endl;
		cout << "The following center frequencies have power below the std:" << endl;
	}
	
	FILE *whiteSpaces;
	whiteSpaces = fopen(output, "a");
	
	//only frequencies contained in the TV UHF band are considered
	
	for(it = availableChannels.begin(); it != availableChannels.end(); it++){
		channel = it->channel;
		if(channel > 0){
			//1. channel, 2. freq, 3. power
			fprintf(whiteSpaces, "%d %f\n", channel, (it->power));
			if(print == 4) cout << "Channel: " << it->channel << " Power: " << it->power << endl;
		}
		continue;
	}
	fclose(whiteSpaces);
	
	//Optionally you can compare the output file with some external file
	if(print == 5) compare(allSweep, output, sampleRate, print);
	
	
	
	
}

#endif
