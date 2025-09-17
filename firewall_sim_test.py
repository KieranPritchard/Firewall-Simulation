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
    return

def check_packets():
    return

def main():
    return

if __name__ == "__main__":
    main()