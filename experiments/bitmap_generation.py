import numpy as np
import cv2 as cv
import time

MAX_RES = 64

start = time.time()
# Load an image
raw_img = cv.imread('data/img/calib_image.png')
assert raw_img is not None, "Image failed to load; Image is None"

# Get the current height and width
height = raw_img.shape[0]
width = raw_img.shape[1]

# Calculate which factor is needed for the maximum size of MAX_RES
height_factor = MAX_RES/height
width_factor = MAX_RES/width
factor = min(height_factor, width_factor)

height = height*factor
width = width*factor
new_size = (int(height), int(width))

# Resize the raw image
resized_img = cv.resize(raw_img, new_size, interpolation=cv.INTER_LINEAR)

# Recolor image
def remap_channel(channel, bit_out : int):
    factor = (2**bit_out-1)/256
    out = np.array(channel)
    rows = out.shape[0]
    columns = out.shape[1]

    for i in range(rows):
        for j in range(columns):
            out[i][j] = round(channel[i][j]*factor)
    return out

b, g, r = cv.split(resized_img)

new_b = b >> 6
new_g = g >> 5
new_r = r >> 5

compressed = (new_b << 0) + (new_g << 2) + (new_r << 5)
# recoloured_img = cv.merge((new_b, new_g, new_r))

# merge the channels into one byte
# compressed = np.zeros((MAX_RES, MAX_RES))

# for i in range(recoloured_img.shape[0]):
#     for j in range(recoloured_img.shape[1]):
#         compressed[i][j] = (recoloured_img[i][j][0] << 0) | \
#                            (recoloured_img[i][j][1] << 2) | \
#                            (recoloured_img[i][j][2] << 5)
print(time.time() - start)


with open('./experiments/bitmap_calib2.h', 'w') as file:
    file.write("#ifndef BITMAP_H\n")
    file.write("#define BITMAP_H\n")
    file.write("\n")
    file.write("const unsigned char bitmap_calib[] = {\n")

    for i in range(compressed.shape[0]):
        for j in range(compressed.shape[1]):
            file.write(str(hex(int(compressed[i][j]))) + ', ')
        file.write('\n')

    file.write("};\n")
    file.write("#endif")

# cv.imshow('raw', raw_img)
cv.imshow('resized', resized_img)
# cv.imshow('recoloured', recoloured_img)
cv.waitKey(0)