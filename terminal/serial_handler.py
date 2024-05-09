import serial.tools.list_ports

def get_available_ports():
    ports = sorted(serial.tools.list_ports.comports())
    return ports

if __name__ == "__main__":
    get_available_ports()
