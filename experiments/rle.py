import numpy as np
import bitmap
import cv2 as cv

class RLE():
    def __init__(self) -> None:
        self._last_compression_rate = 0
        self._last_bytes = 0

    def encode_1d(self, input : np.ndarray) -> list:
        if input.ndim > 1:
            raise TypeError("Received ndim > 1")
        count = 0
        current_val = input[0]
        output = []

        for val in input:
            if val != current_val:
                output.append(count)
                output.append(current_val)
                count = 0
                current_val = val
            count += 1
        output.append(count)
        output.append(current_val)
        return output

    def encode_2d(self, input : np.ndarray) -> list:
        output = []
        for data in input:
            output.append(self.encode_1d(data))
        return output

    def encode_img(self, input : np.ndarray) -> list:
        header_raw = 0x00
        header_rle = 0x01
        total_bytes = 0
        output = []

        rle_data = self.encode_2d(input)
        for idx, line in enumerate(rle_data):
            # if the compressed line is shorter then the original data, use
            # the compressed variant
            if len(line) < input.shape[0]:
                line.insert(0, header_rle) # This might be slow!
                output.append(line)
            else:
                line.insert(0, header_raw)
                output.append(input[idx])
                print("raw")
            total_bytes += len(output[-1])

        self._last_bytes = total_bytes
        self._last_compression_rate = total_bytes/(input.shape[0]*
                                                   input.shape[1])
        return output

    def get_compression(self):
        return self._last_compression_rate

    def get_bytes_encoded(self):
        return self._last_bytes

if __name__ == "__main__":
    rle = RLE()
    bmap = bitmap.BitmapGenerator()
    vid = cv.VideoCapture(0)
    vid.set(cv.CAP_PROP_FPS, 10)

    while True:
        ret, frame = vid.read()

        compressed_frame = bmap.convert_frame_bitmap(frame, 256)
        data = rle.encode_img(compressed_frame)

        print(f"{rle.get_bytes_encoded()} bytes encoded with a rate of {rle.get_compression()*100:.2f}%")

        cv.imshow('Frame', frame)
        if cv.waitKey(1) & 0xFF == ord('q'):
            break

    vid.release()
    cv.destroyAllWindows()