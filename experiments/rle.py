import numpy as np

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
    
    def encode_img(self, input : np.ndarray) -> bytes:
        header_raw = 0xC0
        header_rle = 0xDE



        return

if __name__ == "__main__":
    rle = RLE()

    input = np.array([1, 1, 1, 1, 4, 2, 3, 3, 3, 3, 4, 5, 6, 6, 6, 6, 6])
    output = rle.encode_1d(input)
    print("Expected result:\n [4, 1, 1, 4, 1, 2, 4, 3, 1, 4, 1, 5, 4, 6]")
    print("Result: \n", output)
    print()

    input = np.array([[1, 1, 1, 1], [2, 3, 4, 4]])
    output = rle.encode_2d(input)
    print("Expected result:\n [[4, 1], [1, 2, 1, 3, 2, 4]]")
    print("Result: \n", output)
