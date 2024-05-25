import cv2 as cv
import numpy as np

def resize_image(img : cv.typing.MatLike, max_size : int):
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

    # Resize the raw image
    resized_img = cv.resize(img, new_size, interpolation=cv.INTER_LINEAR)
    return resized_img

def remap_channel(channel, bit_out : int):
    factor = (2**bit_out-1)/256
    out = np.array(channel)
    rows = out.shape[0]
    columns = out.shape[1]

    for i in range(rows):
        for j in range(columns):
            out[i][j] = round(channel[i][j]*factor)
    return out

def recolour_image(img : cv.typing.MatLike):
    b, g, r = cv.split(img)
    new_b = remap_channel(b, 2) # Possibly need to do 3 also, then remove the lsb
                                # We shall see
    new_g = remap_channel(g, 3)
    new_r = remap_channel(r, 3)

    recoloured_img = cv.merge((new_b, new_g, new_r))
    return recoloured_img

def compress_image(img : cv.typing.MatLike):
    compressed = np.zeros((16, 16))

    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            compressed[i][j] = (img[i][j][0] << 0) | \
                               (img[i][j][1] << 2) | \
                               (img[i][j][2] << 5)
    return compressed

def write_to_file(img, filename : str):
    with open('./experiments/'+ filename + '.h', 'w') as file:
        file.write("#ifndef BITMAP_H\n")
        file.write("#define BITMAP_H\n")
        file.write("\n")
        file.write("const unsigned char bitmap_calib[] = {\n")

        for i in range(img.shape[0]):
            for j in range(img.shape[1]):
                file.write(str(hex(int(img[i][j]))) + ', ')
            file.write('\n')

        file.write("};\n")
        file.write("#endif")

def convert_to_bitmap(path : str):
    raw_img = cv.imread(path)
    assert raw_img is not None, "Image failed to load; Image is None"

    resized_img = resize_image(raw_img, 16)
    recolour_img = recolour_image(resized_img)
    compressed_img = compress_image(recolour_img)
    
    