import random

def create_basic_rules(packets):
    vulnerable_ports = [
        20,21,22,23,25,53,67,68,69,80,443,110,143,
        137,138,139,445,123,161,5060,5061,554,3306,
        5432,1433,27017,3389,5900,5000,8000,8080,8443
    ]

    selected_packets = [p for p in packets if p["port"] in vulnerable_ports]
    if not selected_packets:
        return []

    rules_to_make = random.randint(5, 150)
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

    for x in range(255):
        new_packet = {"ip": f"1.0.0.{x}", "port": random.randint(1, 500)}
        packets.append(new_packet)
    
    return packets

def make_receiver_packets():
    packets = []

    for x in range(255):
        octet_one = random.randint(1,255)
        octet_two = random.randint(1,255)
        octet_three = random.randint(1,255)
        octet_four = random.randint(1,255)

        new_packet = {"ip": f"{octet_one}.{octet_two}.{octet_three}.{octet_four}", "port": random.randint(1, 500)}
        packets.append(new_packet)

    for i in range(255//5 * 2):
        random_index = random.randint(0,254)
        vulnerable_ports = [
            20,21,22,23,25,53,67,68,69,80,443,
            110,143,137,138,139,445,123,161,5060,
            5061,554,3306,5432,1433,27017,3389,5900,
            5000,8000,8080,8443
        ]
        random_port = random.choice(vulnerable_ports)
        packet = packets[random_index]
        packet.update({"port": random_port})
    
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
        checked_packet, action = check_packets(rules, packet)

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