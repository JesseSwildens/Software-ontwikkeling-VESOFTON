import time

import cv2 as cv
from bitmap import BitmapGenerator
from rle import RLE
from serial_handler import SerialHandler


class Stream:
    def __init__(self, ser: SerialHandler) -> None:
        self.ser = ser

        self.rle = RLE()
        self.bmap = BitmapGenerator()

        self.is_streaming = False
        self.is_ready = False

        self.vid = cv.VideoCapture(0)
        self.frame_counter = 0

    def is_enabled(self) -> bool:
        return self.is_streaming

    def enable(self) -> None:
        self.is_streaming = True

        # send a command to enable streaming on the STM
        self.ser.send('hyperdrive')
        # wait for the command to be transmitted, maybe wait for a response
        time.sleep(0.01)

        # reopen the port at a higher baudrate
        self.ser.ser.close()
        # zet je baudrate hier MIRKO
        self.ser.ser.baudrate = 2000000
        self.ser.ser.open()

    def disable(self) -> None:
        self.ser.send('slowdrive')
        cv.destroyWindow('Stream')
        self.is_streaming = False

        self.ser.ser.close()
        self.ser.ser.baudrate = 115200
        self.ser.ser.open()

    def add_verbose(self, frame, compressed):
        text = str(round((1 - self.rle.get_compression()) * 100, 2))
        frame = cv.putText(
            frame,
            text,
            (20, 40),
            cv.FONT_HERSHEY_SIMPLEX,
            1,
            (255, 0, 0),
            2,
            cv.FILLED,
        )
        text = str(self.frame_counter)
        frame = cv.putText(
            frame,
            text,
            (20, 80),
            cv.FONT_HERSHEY_SIMPLEX,
            1,
            (255, 0, 0),
            2,
            cv.FILLED,
        )
        text = f'{compressed.shape[0]}x{compressed.shape[1]}'
        frame = cv.putText(
            frame,
            text,
            (20, 120),
            cv.FONT_HERSHEY_SIMPLEX,
            1,
            (255, 0, 0),
            2,
            cv.FILLED,
        )
        return frame

    def run(self) -> None:
        if not self.is_streaming:
            return

        _, frame = self.vid.read()
        compressed_frame = self.bmap.convert_frame_bitmap(frame, 200)
        encoded_frame = self.rle.encode_img(compressed_frame)


        if self.is_ready:
            start = time.time()
            for idx, data in enumerate(encoded_frame):
                # print([hex(x) for x in data])
                self.ser.ser.write(bytes(data))
            self.frame_counter += 1
            self.is_ready = False
            print(time.time() - start)

        frame = self.add_verbose(frame, compressed_frame)
        cv.imshow('Stream', frame)
