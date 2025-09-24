#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// This seeds the random number so it is completely different each time
srand(time(0));

string make_ip_range(bool range_type){
    if(range_type){
        // Private ranges: [star, end]
        int ranges[][8] = { 
            {10,0,0,0,10,255,255,255},
            {172,16,0,0,172,31,255,255},
            {192,168,0,0,192,168,255,255}
        };
        int random_range = rand() % 3;
        int ip[4];

        // Generate random octets between start and end
        for(int i = 0; i < 4; i++){
            int low = ranges[random_range][i];
            int high = ranges[random_range][i+ 4];
            ip[i] = low + (rand() % (high - low + 1));
        }

        // Build string
        string private_ip = to_string(ip[0]) + "." + to_string(ip[1]) + "." + to_string(ip[2]) + to_string(ip[3]);

        return private_ip;
    } else{
        string public_ip = to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256);
        return public_ip;
    }
}