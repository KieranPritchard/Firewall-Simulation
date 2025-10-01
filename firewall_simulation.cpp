#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <random>
#include <ctime>
#include <cstdlib>

using namespace std;

// Structs for certain elements
struct Packet {
    string ip;
    int port;
    string proto; // "tcp" or "udp"
};

struct Rule{
    string ip;
    int port;
    string action; // "allow" or "deny"
};

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

vector<Packet> make_sender_packets(){
    vector<Packet> packets;
    int packet_count = rand() % 301 + 200;

    vector<int> common_ports = {80, 443, 53, 25, 110, 22, rand() % 255 + 1};
    vector<int> weights = {40, 40, 5, 5, 5, 5, 5};

    for(int i = 0; i < packet_count; i++){
        // Adds 10 random ports 
        vector<int> all_ports = common_ports;
        vector<int> all_weights = weights;
        for(int j = 0; j < 10; j++) {
            all_ports.push_back(rand() % (65535 - 1024 + 1) + 1024);
            all_weights.push_back(1);
        }
        // Weighted random choice
        int total_weight = 0;
        for (int w : all_weights) total_weight += w;
        int r = rand() % total_weight;
        int chosen_port = -1;
        for (size_t k = 0; k < all_ports.size(); k++) {
            r -= all_weights[k];
            if (r < 0) {
                chosen_port = all_ports[k];
                break;
            }
        }

        Packet new_packet;
        new_packet.ip = make_ip_range(true); // to make the ip private
        new_packet.port = chosen_port;
        new_packet.proto = (rand() % 2 == 0 ? "tcp" : "udp");

        packets.push_back(new_packet);
    }
}

int main(){
    // This seeds the random number so it is completely different each time
    srand(time(0));
}