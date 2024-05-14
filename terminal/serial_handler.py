import time

import serial
import serial.tools.list_ports

class SerialHandler():
    def __init__(self) -> None:
        self.ser = serial.Serial(baudrate=115200, timeout=1)
        self.available_ports = self.get_available_ports()

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
            time.sleep(0.75) # delay to avoid bug that shows old data
            self.ser.flush()
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

    def get_available_ports(self):
        ports = sorted(serial.tools.list_ports.comports())
        return ports

if __name__ == "__main__":
    ser = SerialHandler()

    for prt in ser.available_ports:
        print(prt.device)
