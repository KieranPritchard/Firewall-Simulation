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
        new_packet.ip = make_ip_range(true);
        new_packet.port = chosen_port;
        new_packet.proto = (rand() % 2 == 0 ? "tcp" : "udp");

        packets.push_back(new_packet);
    }

    return packets;
}

vector<Packet> make_receiver_packets() {
    vector<Packet> packets;
    int packet_count = rand() % 301 + 200; // 200–500

    vector<int> common_ports = {80, 443, 53, 25, 110, 22, rand() % 255 + 1};
    vector<int> weights       = {40, 40, 5, 5, 5, 5, 5};

    for (int i = 0; i < packet_count; i++) {
        vector<int> all_ports = common_ports;
        vector<int> all_weights = weights;

        for (int j = 0; j < 10; j++) {
            all_ports.push_back(rand() % (65535 - 1024 + 1) + 1024);
            all_weights.push_back(1);
        }

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
        new_packet.ip = make_ip_range(false); // public
        new_packet.port = rand() % 500 + 1;   // 1–500
        new_packet.proto = (rand() % 2 == 0 ? "tcp" : "udp");

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
        int random_index = rand() % packet_count;
        int random_port = vulnerable_ports[rand() % vulnerable_ports.size()];
        packets[random_index].port = random_port;
    }

    return packets;
}

vector<Rule> create_basic_rules(const vector<Packet>& packets){
    // List of vulnerable ports
    vector<int> vulnerable_ports = {
        20,21,22,23,25,53,67,68,69,80,443,110,143,
        137,138,139,445,123,161,5060,5061,554,3306,
        5432,1433,27017,3389,5900,5000,8000,8080,8443
    };

    vector<int> selected_indices;
    for (size_t i = 0; i < packets.size(); ++i) {
        int p = packets[i].port;
        for (int vp : vulnerable_ports) {
            if (p == vp) {
                selected_indices.push_back(static_cast<int>(i));
                break;
            }
        }
    }

    if (selected_indices.empty()) {
        return {}; // no vulnerable packets -> no rules
    }

    int rules_to_make = rand() % 246 + 5; // 5..250
    vector<Rule> rules;
    rules.reserve(min((size_t)rules_to_make, selected_indices.size()));

    for (int i = 0; i < rules_to_make; ++i) {
        // pick a random vulnerable packet
        int pick_idx = selected_indices[rand() % selected_indices.size()];
        const Packet& sp = packets[pick_idx];

        Rule r;
        r.ip = sp.ip;
        r.port = sp.port;
        r.action = (rand() % 2 == 0 ? "allow" : "deny");

        // optional de-duplication: ensure the same rule isn't already present
        bool exists = false;
        for (const Rule& ex : rules) {
            if (ex.ip == r.ip && ex.port == r.port && ex.action == r.action) {
                exists = true;
                break;
            }
        }
        if (!exists) rules.push_back(r);
    }

    return rules;
}

pair<Packet,string> check_packets(const vector<Rule>& rules, const Packet& packet) {
    for (const Rule& rule : rules) {
        if (rule.ip == packet.ip && rule.port == packet.port) {
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

void process_packets(vector<Packet>& packets, const vector<Rule>& rules, const string& direction){
    while (!packets.empty()) {
        int idx = rand() % packets.size();
        Packet packet = packets[idx];
        packets.erase(packets.begin() + idx);

        auto [checked_packet, action] = check_packets(rules, packet);
        
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
    srand(time(0));

    vector<Packet> sender_packets = make_sender_packets();
    vector<Packet> receiver_packets = make_receiver_packets();
    vector<Rule> rules = create_basic_rules(receiver_packets);

    // Combine into one mixed queue
    vector<pair<string, Packet>> combined;
    for (auto& p : sender_packets) combined.push_back({"OUTGOING", p});
    for (auto& p : receiver_packets) combined.push_back({"INCOMING", p});

    // Shuffle combined
    for (int i = combined.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(combined[i], combined[j]);
    }

    // Process combined
    while (!combined.empty()) {
        auto [direction, packet] = combined.back();
        combined.pop_back();

        auto [checked_packet, action] = check_packets(rules, packet);

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
