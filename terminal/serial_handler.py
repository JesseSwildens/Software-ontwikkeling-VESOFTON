import serial
import serial.tools.list_ports

def get_available_ports():
    ports = sorted(serial.tools.list_ports.comports())
    return ports

class SerialHandler():
    def __init__(self) -> None:
        self.ser = serial.Serial(baudrate=115200, timeout=1)

    def connect(self, port : str) -> bool:
        """Toggles a connection to a comport

        Args:
            port (str): port name

        Returns:
            bool: true if connected, false if disconnected
        """
        if not self.ser.is_open:
            self.ser.port = port
            self.ser.open()
        else:
            self.ser.close()
        return self.ser.is_open

    def poll(self) -> (str | None):
        """Receives data if it is ready

        Returns:
            (str | None): String if data could be received, else None
        """
        if not self.ser.is_open:
            return None

        if self.ser.in_waiting > 0:
            data = self.ser.read_until(b'\n')
            return data.decode(errors='backslashreplace')\
                       .strip('\r')\
                       .strip('\n')
        return None

    def send(self, string : str):
        """Send data to the open port, adds a newline if it doesn't have one

        Args:
            string (str): string to send
        """
        if not string.endswith('\n'):
            string = string + '\n'
        self.ser.write(string.encode())

if __name__ == "__main__":
    get_available_ports()
