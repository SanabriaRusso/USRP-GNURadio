#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string.h>

using namespace std;

#include "def.hh"
#include "lengthOfFile.hh"
#include "adaptedPlot.hh"

#ifndef _COMPARE_HH_
#define _COMPARE_HH_

//To see the print of this function, assign print = 5

void compare(std::vector<duplex> allSweep, const char *calculatedWhiteSpaces, double sampleRate, int print){
	
	whiteSPC *measuredWS; //imported from file
	whiteSPC *estimatedWS; //estimated with usrp
	int numberOfLines = 0;
	int numberOfLines2 = 0;
	char const *albertFile = "channels_albert.txt";
	//char const *comparisonResult = "comparisonResult.txt";
	
	int firstChannel = 21;
	int lastChannel = 69;
	
	ifstream fin(albertFile);
	
	numberOfLines = lengthOfFile(&fin);
	//cout << numberOfLines << endl;
	
	measuredWS = new whiteSPC[numberOfLines];
	
	
	ifstream inputFile(albertFile);
	string input;
	
	//Getting the values from the oscillator-measured White Spaces
	
	int iterator = 0;
	int numOfFreeChannels = 0;
	while(getline(inputFile, input)){
		
		istringstream tokenizer(input);
		string token;
		
		getline(tokenizer, token, ' ');
		istringstream float_iss(token);
		int channel;
		float_iss >> channel;
		measuredWS[iterator].channel = channel;
		
		getline(tokenizer, token, ' ');
		istringstream float_iss2(token);
		float freq;
		float_iss2 >> freq;
		measuredWS[iterator].freq = freq;
		
		getline(tokenizer, token, ' ');
		istringstream float_iss3(token);
		int license;
		float_iss3 >> license;
		measuredWS[iterator].license = license;
		
		getline(tokenizer, token, ' ');
		istringstream float_iss4(token);
		int use;
		float_iss4 >> use;
		measuredWS[iterator].use = use;
		if((measuredWS[iterator].use) == 0) numOfFreeChannels++; 
		
		iterator++;
	}
	
	/*cout << "Channel: " << measuredWS[0].channel << " " << "Freq: " << measuredWS[0].freq
	<< " " << "Licensed?: " << measuredWS[0].license << " " << "Used?: " << measuredWS[0].use << endl;*/
	
	
	
	
	//Now the USRP-estimated White Spaces are imported for comparison
	
	ifstream fin2(calculatedWhiteSpaces);
	
	numberOfLines2 = lengthOfFile(&fin2);
	//cout << numberOfLines2 << endl;
	
	//Import 
	
	estimatedWS = new whiteSPC[numberOfLines2];
	
	ifstream inputFile2(calculatedWhiteSpaces);
	string input2;
	
	int iterator2 = 0;
	while(getline(inputFile2, input2)){
		
		istringstream tokenizer(input2);
		string token2;
		
		getline(tokenizer, token2, ' ');
		istringstream float_iss2_1(token2);
		int channel2;
		float_iss2_1 >> channel2;
		estimatedWS[iterator2].channel = channel2;
		
			
		getline(tokenizer, token2, ' ');
		istringstream float_iss2_2(token2);
		float power;
		float_iss2_2 >> power;
		estimatedWS[iterator2].power = power;
		
			
		iterator2++;
	}
	
	//Comparison
	
	int searchChannel, availability;
	int freeMatches = 0;
	int usedMatches = 0;
	int fP = 0; //false positives
	int fN = 0; //false negatives
	int falsePositive [numberOfLines];
	int falseNegative [numberOfLines];
	int freeMatchedWS [numberOfLines];
	int usedMatchedWS [numberOfLines];
	
	//filling the vectors with zeroes
	
	for(int i = 0; i < numberOfLines; i++){
		falsePositive[i] = 0;
		falseNegative[i] = 0;
		freeMatchedWS[i] = 0;
		usedMatchedWS[i] = 0;
	}

	
	for(int i = 0; i < numberOfLines; i++){
		searchChannel = (measuredWS[i].channel);
		availability = (measuredWS[i].use);
		for(int j = 0; j < numberOfLines2; j++){
			if(availability == 0){
				if(searchChannel == (estimatedWS[j].channel)){ //these are the freeMatches
					freeMatchedWS[i] = searchChannel;
					cout << i << ") " << "Matched channel: " << freeMatchedWS[i] << " as free" << endl;
					freeMatches++;
					break;
				}
				else if((j == (numberOfLines2 - 1)) && (searchChannel != (estimatedWS[j].channel))){ //false negative: USRP occupied channel, that was free in osc. measurement
					cout << i << ") " << "False negative in channel: " << searchChannel << " channel was in osc-use" << endl;
					falseNegative[i] = searchChannel;
					fN++; //USRP detected it occupied		
				}
			}
			else if(availability == 1){ //in case it is used
				if(searchChannel == (estimatedWS[j].channel)){ //if I found it free
					cout << i << ") " << "False positive in channel: " << searchChannel << " channel was usrp-free" << endl;
					falsePositive[i] = searchChannel; //then it is a false positive: USRP-found free but it was not
					fP++; //USRP detected it free
					break;
				}
				else if((j == (numberOfLines2 - 1)) && (searchChannel != (estimatedWS[j].channel))){ //if it is not in the free channels, then USRP also found it used
					usedMatchedWS[i] = searchChannel;
					cout << i << ") " << "Matched channel: " << usedMatchedWS[i] << " as used" << endl;
					usedMatches++;
				}
					
			}
		}
	}
	
	//float efficiencyRatio = 1.0 * freeMatches/numOfFreeChannels;
	
	cout << "The number of matches, whether free or used, are: " << freeMatches + usedMatches << " from " << lastChannel - firstChannel << " channels analyzed" << endl;
	cout << "The number of oscillator-measured free channels were: " << numOfFreeChannels << endl;
	cout << "The number of USRP-measured free channels were: " << iterator2 << endl;
	cout << "The number of freeMatches with USRP were: " << freeMatches << endl;
	cout << "Number of channels USRP-seen free but documented used: " << fP << endl;
	cout << "Number of channels USRP-seen used but documented free: " << fN << endl;
	
	//Just to be used in the paper
	
	adaptedPlot(allSweep, measuredWS, numberOfLines, print);
	
	
}

#endif
