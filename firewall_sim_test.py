from random import randint

def add_rules():
    return

def make_sender_packets():
    packets = []

    for x in range(255):
        new_packet = {"ip": f"1.0.0.{x}", "port": randint(1, 500)}
        packets.append(new_packet)
    
    return packets

def make_receiver_packets():
    packets = []

    for x in range(255):
        octet_one = randint(1,255)
        octet_two = randint(1,255)
        octet_three = randint(1,255)
        octet_four = randint(1,255)

        new_packet = {"ip": f"{octet_one}.{octet_two}.{octet_three}.{octet_four}", "port": randint(1, 500)}
        packets.append(new_packet)
    
    return packets

def send_packet_out(packets, is_response):
    if is_response:
        packet_to_send == packets
        return packet_to_send
    else:
        packet_to_send = packets[randint(0, packets.len())]
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
                receive_packet(packet)
            else:
                print("packet denyed")

def main():
    return

if __name__ == "__main__":
    main()