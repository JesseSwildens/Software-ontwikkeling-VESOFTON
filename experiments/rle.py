import numpy as np
import bitmap

class RLE():
    def __init__(self) -> None:
        pass

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
        output = []

        rle_data = self.encode_2d(input)
        for line in rle_data:
            # if the compressed line is shorter then the original data, use
            # the compressed variant
            if len(line) < input.shape[0]:
                line.insert(0, header_rle) # This might be slow!
                output.append(line)
            else:
                line.insert(0, header_raw)
                output.append(line)

        return output

if __name__ == "__main__":
    rle = RLE()
    bmap = bitmap.BitmapGenerator()

    raw_data = bmap.convert_to_bitmap('./data/img/webcam.jpg', 512)
    data = rle.encode_img(raw_data)

    total = 0
    for line in data:
        print(line)
        total += len(line)
    print(total)

