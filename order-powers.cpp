//
//  main.cpp
//  powerPlot
//
//  Created by Luis Sanabria on 5/22/12.
//  Copyright (c) 2012 NeTS Research Group, Universitat Pompeu Fabra. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#include "includes/compare.hh"
#include "includes/def.hh"
#include "includes/displayArray.hh"
#include "includes/displayContainer.hh"
#include "includes/freqBeenChecked.hh"
#include "includes/freqSort.hh"
#include "includes/lengthOfFile.hh"
#include "includes/removeZeros.hh"
#include "includes/sumAndAverage.hh"
#include "includes/whichChannel.hh"
#include "includes/whiteDisplay.hh"
#include "includes/whiteSearch.hh"


int main (int argc, const char * argv[])
{
    duplex *freqContainer;
    int numberOfLines = 0;
    int print = 0;
    double sampleRate = 1e06;
    //You need to import the file first to count the lines, and then to do the manipulation
    char const *routeToFile = "powers.txt";
    char const *routeToTheResultFile = "avgPowers.txt";
    
    if(argc >=2) routeToFile = argv[1];
    if(argc >= 3) routeToTheResultFile = argv[2];
    if(argc >= 4) sampleRate = atof(argv[3]);
    if(argc >= 5) print = atoi(argv[4]);
    
    ifstream fin(routeToFile);
    
    numberOfLines = lengthOfFile(&fin);
    freqContainer = new duplex[numberOfLines];
    
    
    if(print == 1) cout << "The size of the file is: " << numberOfLines << endl;
    
    ifstream inputFile(routeToFile);
    string input;
    
    //Getting the values separatetly and putting them in variables.
    
    int iterator = 0;
    while(getline(inputFile,input)){
        //cout << input <<endl;
        
        istringstream tokenizer(input);
        string token;
        
        getline(tokenizer, token, ' ');
        istringstream float_iss(token);
        float f;
        float_iss >> f;
        //cout << f << endl;
        freqContainer[iterator].freq = f + 1e6;
        
        getline(tokenizer, token, ' ');
        istringstream float_iss2(token);
        float p;
        float_iss2 >> p;
        //cout << g << endl;
        freqContainer[iterator].power = p;
        
        iterator++;
    }
    
    //displayContainer(freqContainer, numberOfLines); 
    sumAndAverage(freqContainer, numberOfLines, routeToTheResultFile, sampleRate, print);
    
    
    return 0;
}

