#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <random>
#include <ctime>
#include <cstdlib>

using namespace std;

// Structs for packets
struct Packet {
    string ip; // String to store ip
    int port; // int to store port
    string proto; // "tcp" or "udp"
};

// Struct for rules
struct Rule{
    string ip; // String to store ip
    int port; // int to store port
    string action; // "allow" or "deny"
};

// Function to make ip range
string make_ip_range(bool range_type){
    // Checks if range type is true
    if(range_type){
        // Private ranges: [star, end] as a 2d arrary
        int ranges[][8] = { 
            {10,0,0,0,10,255,255,255},
            {172,16,0,0,172,31,255,255},
            {192,168,0,0,192,168,255,255}
        };
        // selects arrary from 2d arrary
        int random_range = rand() % 3; 
        // Creates a empty arrary of 4 elements
        int ip[4];

        // Generate random octets between start and end
        for(int i = 0; i < 4; i++){
            int low = ranges[random_range][i]; // selects a low number from the ranges
            int high = ranges[random_range][i+ 4]; // selects a high number from the range
            ip[i] = low + (rand() % (high - low + 1)); // creates the octect from the high and low added together and randomised
        }

        // Builds private ip from converting the parts of ip arrary
        string private_ip = to_string(ip[0]) + "." + to_string(ip[1]) + "." + to_string(ip[2]) + to_string(ip[3]);
        // Returns the private ip
        return private_ip;
    } else{
        // builds public ip from random numbers converted to strings
        string public_ip = to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256);
        // Returns the public ip
        return public_ip;
    }
}

// Function to create sender packets
vector<Packet> make_sender_packets(){
    vector<Packet> packets; // Vector to store packets structures
    int packet_count = rand() % 301 + 200; // creates a random number of packets

    vector<int> common_ports = {80, 443, 53, 25, 110, 22, rand() % 255 + 1}; // Vector to store common ports
    vector<int> weights = {40, 40, 5, 5, 5, 5, 5}; // Vector to store weights

    // Loops over the packet count
    for(int i = 0; i < packet_count; i++){
        // Adds 10 random ports 
        vector<int> all_ports = common_ports; // Vector to store all ports starting with common ports
        vector<int> all_weights = weights; // Vector to store all weights
        // Loops over the weights
        for(int j = 0; j < 10; j++) {
            all_ports.push_back(rand() % (65535 - 1024 + 1) + 1024); // Adds random port numbers to the all ports vector
            all_weights.push_back(1); // adds weights to the all_weights vector
        }
        // Weighted random choice
        int total_weight = 0; // Stores the total weight
        // Loops through and adds up the total weight from the vector
        for (int w : all_weights) total_weight += w;
        int r = rand() % total_weight; // Chooses a random weight index
        int chosen_port = -1; // Chooses random port
        // Loops over all the ports
        for (size_t k = 0; k < all_ports.size(); k++) {
            // Removes the indexed weight from the random weight number
            r -= all_weights[k];
            // Checks if random weight is higher then 0
            if (r < 0) {
                // Sets the random weight to the chosen port number
                chosen_port = all_ports[k];
                // Breaks the loop
                break;
            }
        }

        // Creates a new packet from the structure
        Packet new_packet;
        new_packet.ip = make_ip_range(true); // Sets a random private ip
        new_packet.port = chosen_port; // Sets the chosen port
        new_packet.proto = (rand() % 2 == 0 ? "tcp" : "udp"); // Chooses between tcp and udp

        // Adds the packet to the packets vector
        packets.push_back(new_packet);
    }
    // Returns the packets
    return packets;
}

// Function to make recevier packets
vector<Packet> make_receiver_packets() {
    // Creates a new vector for the packets
    vector<Packet> packets;
    int packet_count = rand() % 301 + 200; // chooses random packet count

    vector<int> common_ports = {80, 443, 53, 25, 110, 22, rand() % 255 + 1}; // Vector to store common ports
    vector<int> weights = {40, 40, 5, 5, 5, 5, 5}; // Vector to store all weights

    // Loops over the packet count
    for (int i = 0; i < packet_count; i++) {
        vector<int> all_ports = common_ports; 
        vector<int> all_weights = weights; 

        for (int j = 0; j < 10; j++) {
            all_ports.push_back(rand() % (65535 - 1024 + 1) + 1024); // Adds random port numbers to the all ports vector
            all_weights.push_back(1); // adds weights to the all_weights vector
        }

        int total_weight = 0; // Variable to track total weight
        // Loops through and adds up the total weight from the vector
        for (int w : all_weights) total_weight += w;
        int r = rand() % total_weight; // Chooses a random weight
        int chosen_port = -1; // Random port helper variable
        // Loops over all the ports vector
        for (size_t k = 0; k < all_ports.size(); k++) {
            // Chooses random weight
            r -= all_weights[k];
            // checks if random weight is greater then 0
            if (r < 0) {
                // Selects the chosen port
                chosen_port = all_ports[k];
                // Breaks the loop
                break;
            }
        }
        
        // Creates new packet struct
        Packet new_packet;
        new_packet.ip = make_ip_range(false); // creates public ip
        new_packet.port = rand() % 500 + 1;   // 1–500
        new_packet.proto = (rand() % 2 == 0 ? "tcp" : "udp"); // Chooses between tcp and udp

        // Adds packet to packets vector
        packets.push_back(new_packet);
    }

    // Vulnerable ports list
    vector<int> vulnerable_ports = {
        20,21,22,23,25,53,67,68,69,80,443,110,143,
        137,138,139,445,123,161,5060,5061,554,3306,
        5432,1433,27017,3389,5900,5000,8000,8080,8443
    };

    // Randomly overwrite some packets with vulnerable ports
    for (int i = 0; i < (packet_count / 5) * 2; i++) {
        // Selects random index
        int random_index = rand() % packet_count;
        // random port from vulnerabe ports 
        int random_port = vulnerable_ports[rand() % vulnerable_ports.size()];
        // Sets the port the new vulnerable one
        packets[random_index].port = random_port;
    }

    // Returns the packets
    return packets;
}

// Function to create basic rules
vector<Rule> create_basic_rules(const vector<Packet>& packets){
    // List of vulnerable ports
    vector<int> vulnerable_ports = {
        20,21,22,23,25,53,67,68,69,80,443,110,143,
        137,138,139,445,123,161,5060,5061,554,3306,
        5432,1433,27017,3389,5900,5000,8000,8080,8443
    };

    // Creates a vector of selected packets
    vector<int> selected_indices;
    // Loops over the packets variable
    for (size_t i = 0; i < packets.size(); ++i) {
        // Extracts the port number
        int p = packets[i].port;
        // Loops over the vulnerable ports
        for (int vp : vulnerable_ports) {
            // Checks if the port is the same as vulnerable port
            if (p == vp) {
                // adds the packet to the selected indices vector
                selected_indices.push_back(static_cast<int>(i));
                // Breaks
                break;
            }
        }
    }

    // Checks if the selected packets are empty
    if (selected_indices.empty()) {
        return {}; // no vulnerable packets -> no rules
    }

    int rules_to_make = rand() % 246 + 5; // Decides the number of rules to make
    vector<Rule> rules; // Creates a vector of rules
    // Reserves the rules for the number of rules to make and the selected packets
    rules.reserve(min((size_t)rules_to_make, selected_indices.size()));

    // Loops over the rules to make
    for (int i = 0; i < rules_to_make; ++i) {
        // pick a random vulnerable packet
        int pick_idx = selected_indices[rand() % selected_indices.size()];
        // Points to the packets
        const Packet& sp = packets[pick_idx];

        // Creates a new rule structure
        Rule r;
        r.ip = sp.ip; // assigns ip address
        r.port = sp.port; // Assigns the port number
        r.action = (rand() % 2 == 0 ? "allow" : "deny"); // Chooses between allow or deny

        // de-duplication: ensure the same rule isn't already present
        bool exists = false;
        for (const Rule& ex : rules) {
            if (ex.ip == r.ip && ex.port == r.port && ex.action == r.action) {
                exists = true;
                break;
            }
        }
        if (!exists) rules.push_back(r);
    }

    // Returns the rules
    return rules;
}

// function to check packets
pair<Packet,string> check_packets(const vector<Rule>& rules, const Packet& packet) {
    // Loops over the rules in rules
    for (const Rule& rule : rules) {
        // Checks if the ports and ips match
        if (rule.ip == packet.ip && rule.port == packet.port) {
            // Checks if the rule is allow and returns the relevant action and packet
            if (rule.action == "allow") {
                return {packet, "allow"};
            } else {
                return {packet, "deny"};
            }
        }
    }
    // default: allow
    return {packet, "allow"};
}

// Function to process the packets
void process_packets(vector<Packet>& packets, const vector<Rule>& rules, const string& direction){
    // Loops while the packets are not empty
    while (!packets.empty()) {
        // Chooses a random packet index
        int idx = rand() % packets.size();
        // Gets the packet at the random index
        Packet packet = packets[idx];
        // Errases the packet
        packets.erase(packets.begin() + idx);

        // Gets the results from the check packets function
        auto [checked_packet, action] = check_packets(rules, packet);
        
        // Checks the action of the packet and outputs a log 
        if (action == "allow") {
            cout << "[" << direction << "] Allowed Packet: {ip=" << checked_packet.ip
                << ", port=" << checked_packet.port
                << ", proto=" << checked_packet.proto << "}" << endl;
        } else {
            cout << "[" << direction << "] Denied Packet: {ip=" << checked_packet.ip
                << ", port=" << checked_packet.port
                << ", proto=" << checked_packet.proto << "}" << endl;
        }
    }
}

int main() {
    // Seeds the randomness
    srand(time(0));

    vector<Packet> sender_packets = make_sender_packets(); // Creates a vector of sender packets
    vector<Packet> receiver_packets = make_receiver_packets(); // creates a vector of receiver packets
    vector<Rule> rules = create_basic_rules(receiver_packets); // Creates a vector of basic rules

    // Combine into one mixed queue
    vector<pair<string, Packet>> combined;
    for (auto& p : sender_packets) combined.push_back({"OUTGOING", p});
    for (auto& p : receiver_packets) combined.push_back({"INCOMING", p});

    // Shuffle combined
    for (int i = combined.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(combined[i], combined[j]);
    }

    // Loops whil combined is not empty
    while (!combined.empty()) {
        // Gets the final part of teh vector
        auto [direction, packet] = combined.back();
        combined.pop_back();

        // Gets the result of the checked packet function
        auto [checked_packet, action] = check_packets(rules, packet);

        // Checks if the action is allow and outputs the log in both paths
        if (action == "allow") {
            cout << "[" << direction << "] Allowed Packet: {ip=" << checked_packet.ip
                << ", port=" << checked_packet.port
                << ", proto=" << checked_packet.proto << "}" << endl;
        } else {
            cout << "[" << direction << "] Denied Packet: {ip=" << checked_packet.ip
                << ", port=" << checked_packet.port
                << ", proto=" << checked_packet.proto << "}" << endl;
        }
    }

    return 0;
}
