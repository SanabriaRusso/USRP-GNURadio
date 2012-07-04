#ifndef _DUPLEX_
#define _DUPLEX_
using namespace std;

struct duplex{
    float freq, power;
};
#endif

#ifndef _WHITESPC_
#define _WHITESPC_

struct whiteSPC{
	int channel, license, use;
	float freq , power; 
};
#endif


#ifndef _CHANNELPOWER_
#define _CHANNELPOWER_
struct channelPower{
	int channel;
	float power; 
};
#endif
