import cv2 as cv
import numpy as np

class BitmapGenerator():
    def __init__(self) -> None:
        pass

    def convert_to_bitmap(self, path : str, size : int):
        raw_img = cv.imread(path)
        assert raw_img is not None, "Image failed to load; Image is None"

        resized_img = self.resize_image(raw_img, size)
        recolour_img = self.recolour_image(resized_img)
        compressed_img = self.compress_image(recolour_img)
        return compressed_img

    def resize_image(self, img : cv.typing.MatLike, max_size : int):
        # Get the current height and width
        height = img.shape[0]
        width = img.shape[1]

        # Calculate which factor is needed for the maximum size of MAX_RES
        height_factor = max_size/height
        width_factor = max_size/width
        factor = min(height_factor, width_factor)

        height = height*factor
        width = width*factor
        new_size = (int(height), int(width))
        print(new_size)

        # Resize the raw image
        resized_img = cv.resize(img, new_size, interpolation=cv.INTER_LINEAR)
        return resized_img

    def remap_channel(self, channel, bit_out : int):
        shift = 8 - bit_out
        return channel >> shift

    def recolour_image(self, img : cv.typing.MatLike):
        b, g, r = cv.split(img)
        new_b = self.remap_channel(b, 2)
        new_g = self.remap_channel(g, 3)
        new_r = self.remap_channel(r, 3)

        recoloured_img = cv.merge((new_b, new_g, new_r))
        return recoloured_img

    def compress_image(self, img : cv.typing.MatLike) -> np.ndarray:
        compressed = (img[:, :, 0] << 0) + (img[:, :, 1] << 2) + (img[:, :, 2] << 5)
        return compressed

    def write_to_file(self, img, filename : str):
        name = filename.split('.')[0]

        with open(f'./experiments/{name}.h', 'w') as file:
            file.write(f"#ifndef {name.upper()}_H\n")
            file.write(f"#define {name.upper()}_H\n")
            file.write("\n")
            file.write(f"const unsigned char bitmap_{name}[] = {{ \n")

            for i in range(img.shape[0]):
                for j in range(img.shape[1]):
                    file.write(str(hex(int(img[i][j]))) + ', ')
                file.write('\n')

            file.write("};\n")
            file.write("#endif")