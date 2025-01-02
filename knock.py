from scapy.all import IP, TCP, sr1, send
import time
import sys


def send_syn_packet(host, port):
    #create IP and TCP layers, S for SYN
    ip = IP(dst=host)
    tcp = TCP(sport=12345, dport=port, flags='S')

    packet = ip / tcp
    send(packet, verbose=0) 
    print(f"Sent SYN packet to {host}:{port}")


def check_port_access(host, port):
    #create IP and TCP layers for checking access
    ip = IP(dst=host)
    tcp = TCP(sport=12345, dport=port, flags='S') 

    #send SYN and wait for a response
    response = sr1(ip / tcp, verbose=0, timeout=2)

    if response and response.haslayer(TCP):
        print(f"Response received from {host}:{port}")
    else:
        print(f"No response from {host}:{port}")


def print_help():
    help_message = (
        "Usage: python knock.py <host> <port1> <port2> <etc...>\n"
        "This script is designed to perform port knocking on servers running Speakeasy.\n"
        "It requires escalated privileges to run properly.\n"
        "The last port in the sequence will be tested for access.\n"
    )
    print(help_message)
    sys.exit(1)


def main():
    if len(sys.argv) < 3 or sys.argv[1] in ["-h", "--help"]:
        print_help()

    host = sys.argv[1]
    ports = [int(port) for port in sys.argv[2:]]

    for port in ports[:-1]:
        send_syn_packet(host, port)
        time.sleep(1)

    check_port_access(host, ports[-1])


if __name__ == "__main__":
    main()
