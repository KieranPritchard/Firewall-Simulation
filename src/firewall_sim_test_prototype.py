import random

# Function to make range of ip addresses either as private or public.
def make_ip_range(range_type):
    # Checks if the specified range is private
    if range_type == "private":
        # Lists private range numbers
        ranges = [
            (10,0,0,0,10,255,255,255),
            (172,16,0,0,172,31,255,255),
            (192,168,0,0,192,168,255,255)
        ]
        # Chooses a random range from the list of tuples
        random_range = random.choice(ranges)
        # Returns a formatted string of the random numbers stored in the tuples to make a mock private address
        return f"{random.randint(random_range[0],random_range[4])}.{random.randint(random_range[1],random_range[5])}.{random.randint(random_range[2],random_range[6])}.{random.randint(random_range[3],random_range[7])}"
    else:
        # Returns a a string of random numbers joined by . to make a mock ip address
        return ".".join(str(random.randint(1,255)) for _ in range(4))

# This creates basic firewall rules from an input of packets
def create_basic_rules(packets):
    # List to contain vulnerable port numbers to use with building rules
    vulnerable_ports = [
        20,21,22,23,25,53,67,68,69,80,443,110,143,
        137,138,139,445,123,161,5060,5061,554,3306,
        5432,1433,27017,3389,5900,5000,8000,8080,8443
    ]

    # Selects packets based on vulnerable ports with list comprehention
    selected_packets = [p for p in packets if p["port"] in vulnerable_ports]
    # Checks for if there are no selected packets
    if not selected_packets:
        # Returns a empty arrary
        return []

    # creates rules based on the random amount from the selcted packets
    rules_to_make = random.randint(5, 250)
    # Stores the rules
    rules = []

    # Loops over the number of packets to make
    for _ in range(rules_to_make):
        selected_packet = random.choice(selected_packets) # Choooses a random packet from the selected packets
        rule_ip = selected_packet["ip"] # Gets the ip from the packet for the rule
        rule_port = selected_packet["port"] # Gets the port from the packet for the rule
        action = random.choice(["allow", "deny"]) # selects allowing or deny

        # Creates the rule with the ip, port, and action
        rule = {"ip": rule_ip, "port": rule_port, "action": action}
        if rule not in rules:  # optional de-duplication
            # Adds the rule to the rules
            rules.append(rule)

    # Returns the rules
    return rules

# Makes sender packets with bias towards more popular ports
def make_sender_packets():
    packets = [] # empty list to store packets
    packet_count = random.randint(200, 500) # Chooses random number of packets
    common_ports = [80, 443, 53, 25, 110, 22, random.randint(1,255)] # List of common ports with a random ports
    weights = [40, 40, 5, 5, 5, 5, 5] # Adds weights to the ports to make cert ports appear more often

    # Loops over the packets 
    for _ in range(packet_count):
        # Creates a list of all ports using list comprehention
        all_ports = common_ports + [random.randint(1024, 65535) for _ in range(10)]
        # adds all the weights
        all_weights = weights + [1]*10
        port = random.choices(all_ports, weights=all_weights)[0]  # Picks one port

        # Dictionary to store new packet
        new_packet = {
            "ip": make_ip_range("private"), # Creates ip with ip range function
            "port": port, # Gets port from random choice function
            "proto": random.choice(["tcp", "udp"]) # Chooses random choice tcp or udp
        }
        # Adds new packet to packets list
        packets.append(new_packet)
    
    # Returns the packets list
    return packets

# Creates receiver packets with same weighting
def make_receiver_packets():
    packets = [] # Empty list storing packets
    packet_count = random.randint(200, 500) # Chooses a random packet count of packets to make
    common_ports = [80, 443, 53, 25, 110, 22, random.randint(1,255)] # List of random ports and a random port number
    weights = [40, 40, 5, 5, 5, 5, 5] # Adds weights to the ports to make cert ports appear more often

    # Loops over the packet count
    for _ in range(packet_count):
        # Creates a list of all ports using list comprehention
        all_ports = common_ports + [random.randint(1024, 65535) for _ in range(10)]
        # adds all the weights
        all_weights = weights + [1]*10
        port = random.choices(all_ports, weights=all_weights)[0]  # Pick one port

        # Dictionary to store new packet
        new_packet = {
            "ip": make_ip_range("public"),
            "port": random.randint(1, 500),
            "proto": random.choice(["tcp", "udp"])
        }
        # Adds new packet to packets list
        packets.append(new_packet)

    # vulnerable ports
    vulnerable_ports = [
        20,21,22,23,25,53,67,68,69,80,443,
        110,143,137,138,139,445,123,161,5060,
        5061,554,3306,5432,1433,27017,3389,5900,
        5000,8000,8080,8443
    ]
    # Loops over 2/5 of the packet count
    for i in range(packet_count // 5 * 2):
        # Chooses a random index
        random_index = random.randint(0, packet_count - 1)
        # Selects a random vulnerable ports
        random_port = random.choice(vulnerable_ports)
        # Sets a packets port ot a vulnerable
        packets[random_index]["port"] = random_port

    # Returns the packets
    return packets


# This checks packets based rules that are created
def check_packets(rules, packet):
    # Loops over the rules
    for rule in rules:
        rule_ip = rule.get("ip") # Gets the rules ip
        rule_port = rule.get("port") # Gets the rules port
        packet_ip = packet.get("ip") # Gets the packets ip
        packet_port = packet.get("port") # Gets the packets port

        # Checks if there is a matching ip and port
        if rule_ip == packet_ip and rule_port == packet_port:
            # Checks the action of the rul
            if rule.get("action") == "allow":
                # Returns the paket and allow
                return packet, "allow"
            else:
                # Returns the packet plus deny
                return packet, "deny"
            
    # if no rule matched, default to allow
    return packet, "allow"

# This processes the packets
def process_packets(packets, rules, direction):
    # Loops while there are packets
    while packets:
        packet = packets.pop(random.randrange(len(packets))) # Choices a random packet 
        checked_packet, action = check_packets(rules, packet)  # Always check for packets
        # Checks if action is allowed
        if action == "allow":
            # Prints packect and the direction
            print(f"[{direction}] Allowed Packet: {checked_packet}")
        else:
            # Prints packect and the direction
            print(f"[{direction}] Denied Packet: {checked_packet}")

def main():
    sender_packets = make_sender_packets() # Creates the sender packets with the function
    receiver_packets = make_receiver_packets() # Creates the recevier packets with the function
    rules = create_basic_rules(receiver_packets) # Creates te basic rules

    # Combines ingoing and outgoing to randomise the output and rules
    combined = [("OUTGOING", p) for p in sender_packets] + [("INCOMING", p) for p in receiver_packets]

    # Shuffles the combined words
    random.shuffle(combined)

    # Loops while there is combined list
    while combined:
        # Gets the direction and packet from the end of the list
        direction, packet = combined.pop()
        # gets the packet and action from the variable
        checked_packet, action = check_packets(rules, packet)

        # Checks if the action is allow
        if action == "allow":
            # Outputs the status
            print(f"[{direction}] Allowed Packet: {checked_packet}")
        else:
            # Outputs the status
            print(f"[{direction}] Denied Packet: {checked_packet}")

# Starts the program
if __name__ == "__main__":
    main()