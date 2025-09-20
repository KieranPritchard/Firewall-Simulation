import random

def make_ip_range(range_type):
    if range_type == "private":
        ranges = [
            (10,0,0,0,10,255,255,255),
            (172,16,0,0,172,31,255,255),
            (192,168,0,0,192,168,255,255)
        ]
        random_range = random.choice(ranges)
        return f"{random.randint(random_range[0],random_range[4])}.{random.randint(random_range[1],random_range[5])}.{random.randint(random_range[2],random_range[6])}.{random.randint(random_range[3],random_range[7])}"
    else:
        return ".".join(str(random.randint(1,255)) for _ in range(4))

def create_basic_rules(packets):
    vulnerable_ports = [
        20,21,22,23,25,53,67,68,69,80,443,110,143,
        137,138,139,445,123,161,5060,5061,554,3306,
        5432,1433,27017,3389,5900,5000,8000,8080,8443
    ]

    selected_packets = [p for p in packets if p["port"] in vulnerable_ports]
    if not selected_packets:
        return []

    rules_to_make = random.randint(5, 250)
    rules = []

    for _ in range(rules_to_make):
        selected_packet = random.choice(selected_packets)
        rule_ip = selected_packet["ip"]
        rule_port = selected_packet["port"]
        action = random.choice(["allow", "deny"])

        rule = {"ip": rule_ip, "port": rule_port, "action": action}
        if rule not in rules:  # optional de-duplication
            rules.append(rule)

    return rules

def make_sender_packets():
    packets = []
    packet_count = random.randint(200, 500)
    common_ports = [80, 443, 53, 25, 110, 22, random.randint(1,255)]
    weights = [40, 40, 5, 5, 5, 5, 5]

    for _ in range(packet_count):
        all_ports = common_ports + [random.randint(1024, 65535) for _ in range(10)]
        all_weights = weights + [1]*10
        port = random.choices(all_ports, weights=all_weights)[0]  # Pick one port

        new_packet = {
            "ip": make_ip_range("private"),
            "port": port,
            "proto": random.choice(["tcp", "udp"])
        }
        packets.append(new_packet)
    
    return packets


def make_receiver_packets():
    packets = []
    packet_count = random.randint(200, 500)
    common_ports = [80, 443, 53, 25, 110, 22, random.randint(1,255)]
    weights = [40, 40, 5, 5, 5, 5, 5]

    for _ in range(packet_count):
        all_ports = common_ports + [random.randint(1024, 65535) for _ in range(10)]
        all_weights = weights + [1]*10
        port = random.choices(all_ports, weights=all_weights)[0]  # Pick one port

        new_packet = {
            "ip": make_ip_range("public"),
            "port": random.randint(1, 500),
            "proto": random.choice(["tcp", "udp"])
        }
        packets.append(new_packet)

    # Add some vulnerable ports randomly
    vulnerable_ports = [
        20,21,22,23,25,53,67,68,69,80,443,
        110,143,137,138,139,445,123,161,5060,
        5061,554,3306,5432,1433,27017,3389,5900,
        5000,8000,8080,8443
    ]
    for i in range(packet_count // 5 * 2):
        random_index = random.randint(0, packet_count - 1)
        random_port = random.choice(vulnerable_ports)
        packets[random_index]["port"] = random_port

    return packets

def check_packets(rules, packet):
    for rule in rules:
        rule_ip = rule.get("ip")
        rule_port = rule.get("port")
        packet_ip = packet.get("ip")
        packet_port = packet.get("port")

        if rule_ip == packet_ip and rule_port == packet_port:
            if rule.get("action") == "allow":
                return packet, "allow"
            else:
                return packet, "deny"
            
    # if no rule matched, default to allow
    return packet, "allow"

def process_packets(packets, rules, direction):
    while packets:
        packet = packets.pop(random.randrange(len(packets)))
        checked_packet, action = check_packets(rules, packet)  # Always check
        if action == "allow":
            print(f"[{direction}] Allowed Packet: {checked_packet}")
        else:
            print(f"[{direction}] Denied Packet: {checked_packet}")

def main():
    sender_packets = make_sender_packets()
    receiver_packets = make_receiver_packets()
    rules = create_basic_rules(receiver_packets)

    combined = [("OUTGOING", p) for p in sender_packets] + \
            [("INCOMING", p) for p in receiver_packets]

    random.shuffle(combined)

    while combined:
        direction, packet = combined.pop()
        checked_packet, action = check_packets(rules, packet)

        if action == "allow":
            print(f"[{direction}] Allowed Packet: {checked_packet}")
        else:
            print(f"[{direction}] Denied Packet: {checked_packet}")


if __name__ == "__main__":
    main()