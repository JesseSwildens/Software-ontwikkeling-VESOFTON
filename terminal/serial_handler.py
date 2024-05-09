import serial
import serial.tools.list_ports

def get_available_ports():
    ports = sorted(serial.tools.list_ports.comports())
    return ports

class SerialHandler():
    def __init__(self) -> None:
        self.ser = serial.Serial(baudrate=115200)

    def connect(self, com : str) -> bool:
        if not self.ser.is_open:
            self.ser.port = com
            self.ser.open()
            print("opened")
            return True
        self.ser.close()
        return False

if __name__ == "__main__":
    get_available_ports()
