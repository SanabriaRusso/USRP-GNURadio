#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#ifndef _DISPLAYARRAY_HH_
#define _DISPLAYARRAY_HH_

void displayArray(float *array, int length){

	for(int i = 0; i < length; i++){
		cout << array[i] << " ";
	}
	cout << endl;
}
#endif
