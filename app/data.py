import os
import struct


current_dir = os.path.dirname(os.path.realpath(__file__))
data_dir = current_dir + '/data/data.bin'

input_data = dict()
input_data.update({"vars_number": 6})
input_data.update({"constraints_number": 5})
input_data.update({"demand": (8,25,13,8,5)})
input_data.update({"costs": (90,20,85,60,35,30)})
input_data.update({"constraints_coefficients": (1,1,1,0,0,0,
                                                0,0,0,1,1,1,
                                                1,0,0,1,0,0,
                                                0,1,0,0,1,0,
                                                0,0,1,0,0,1)})


def pack(data):
    # Define a structured format string with padding and alignment
    format_string = f'ii{len(data["demand"])}f{len(data["costs"])}f{len(data["constraints_coefficients"])}f'

    packed_data = struct.pack(format_string, data["vars_number"], data["constraints_number"],
                            *data["demand"], *data["costs"], *data["constraints_coefficients"])

    with open(data_dir, 'wb') as file:
        file.write(packed_data)

    # Read struct array from the file
    with open(data_dir, 'rb') as file:
        data = file.read()
        unpacked_data = struct.unpack(format_string, data)
    
    print("Unpacked data:", unpacked_data)


if __name__ == "__main__":
    pack(input_data)