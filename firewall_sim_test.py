import random

def create_basic_rules(packets):
    rules_to_make = random.randint(1, 10)
    selected_packets = []
    rules = []

    for packet in packets:
        packet_port = packet.get("port")
        if packet_port == 22 or 21 or 23:
            selected_packets.append(packet)

    for rule in range(rules_to_make):
        selected_packet = random.choice(selected_packets)

        rule_ip = selected_packet.get("ip")
        rule_port = selected_packet.get("port")

        rules.append({"ip": rule_ip, "port": rule_port, "action": "deny"})

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
        vulnerable_ports = [21,22,23]
        random_port = random.choice(vulnerable_ports)
        packet = packets[random_index]
        packet.update({"port": random_port})
    
    return packets

def send_packet_out(packets, is_response):
    if is_response:
        packet_to_send == packets
        return packet_to_send
    else:
        random_packet = random.choice(packets)
        packet_to_send = random_packet
        return packet_to_send

def receive_packet(packet):
    packet_port = packet.get("port")

    if packet_port == 80:
        send_packet_out(packet, True)

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
            
    # if no rule matched, default to deny
    return packet, "deny"

def main():
    sender_packets = make_sender_packets()
    receiver_packets = make_receiver_packets()
    rules = create_basic_rules(receiver_packets)
    
    while True:
        send_packet = send_packet_out(sender_packets, False)
        if not send_packet:
            break  # stop when no packets left

        checked_packet, action = check_packets(rules, send_packet)
        
        if action == "allow":
            print(f"Allowed Packet: {checked_packet}")
            receive_packet(checked_packet)
        else:
            print(f"Denied Packet: {checked_packet}")

        send_packet = send_packet_out(receiver_packets, False)
        if not send_packet:
            break

        checked_packet, action = check_packets(rules, send_packet)
        
        if action == "allow":
            print(f"Allowed Packet: {checked_packet}")
            receive_packet(checked_packet)
        else:
            print(f"Denied Packet: {checked_packet}")

if __name__ == "__main__":
    main()