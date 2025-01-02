import socket
import time
import sys


def send_syn_packet(host, port):
    try:
        #open a TCP socket quickly
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        #very short timeout
        sock.settimeout(0.5)
        sock.connect((host, port))
        sock.close()
        
    except(socket.timeout, socket.error):
        pass

    finally:
        print(f"Sent SYN-like packet (TCP connect attempt) to {host}:{port}")


def print_help():
    help_message = (
        "Usage: python knock.py <host> <port1> <port2> <etc...>\n"
        "This script performs port knocking by attempting to connect to ports in sequence.\n"
        "It does not require root privileges."
    )
    print(help_message)
    sys.exit(1)


def main():
    if len(sys.argv) < 3 or sys.argv[1] in ["-h", "--help"]:
        print_help()

    host = sys.argv[1]
    ports = [int(port) for port in sys.argv[2:]]

    for port in ports:
        send_syn_packet(host, port)
        time.sleep(0.2)  #small delay


if __name__ == "__main__":
    main()
