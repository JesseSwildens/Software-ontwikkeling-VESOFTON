import serial
import serial.tools.list_ports

def get_available_ports():
    ports = sorted(serial.tools.list_ports.comports())
    return ports

class SerialHandler():
    def __init__(self) -> None:
        self.ser = serial.Serial(baudrate=115200, timeout=1)

    def connect(self, com : str) -> bool:
        if not self.ser.is_open:
            self.ser.port = com
            self.ser.open()
        else:
            self.ser.close()
        return self.ser.is_open

    def poll(self) -> str:
        if not self.ser.is_open:
            return None

        if self.ser.in_waiting > 0:
            data = self.ser.read_until(b'\n')
            print(data)
            return data.decode(errors='ignore').strip('\r').strip('\n')

    def send(self, string : str):
        if not string.endswith('\n'):
            string = string + '\n'
        self.ser.write(string.encode())

if __name__ == "__main__":
    get_available_ports()
