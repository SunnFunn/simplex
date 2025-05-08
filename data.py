import os, sys
import struct
from collections import namedtuple


current_dir = os.path.dirname(os.path.realpath(__file__))
data_dir = current_dir + '/data/data.bin'


# vars_number = 20
# constraints_number = 9
# inequalities_number = 9
# inequalities_type = {"neg", "neg", "neg", "neg", "pos", "pos", "pos", "pos", "pos"}
# demand = (5,30,40,100,40,60,30,5,20)
# costs = (90,70,85,50,30,20,35,30,35,70,25,40,35,40,80,30,20,25,35,55)
# constraints_coefficients = (1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
#                             0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,
#                             0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,
#                             0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
#                             1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,
#                             0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,
#                             0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,
#                             0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1)
vars_number = 9
constraints_number = 6
inequalities_number = 6
inequalities_type = {"neg", "neg", "neg", "pos", "pos", "pos"}
demand = (5,30,13,8,14,1000)
costs = (90,70,85,1000, 20,35,30,1000)
constraints_coefficients = (1,1,1,0,0,0,0,0,0,
                            0,0,0,1,1,1,0,0,0,
                            0,0,0,0,0,0,1,1,1,
                            1,0,0,1,0,0,1,0,0,
                            0,1,0,0,1,0,0,1,0,
                            0,0,1,0,0,1,0,0,1)


Data = namedtuple('Data', ['vars_number', 'constraints_number',
                           'demand', 'costs', 'constraints_coefficients'])
data = Data(vars_number, constraints_number,
            demand, costs, constraints_coefficients)

def main(data):
    # Define a structured format string with padding and alignment
    format_string = f'ii{len(data.demand)}f{len(data.costs)}f{len(data.constraints_coefficients)}f'

    # Pack the data into binary format
    packed_data = struct.pack(format_string, data.vars_number, data.constraints_number,
                            *data.demand, *data.costs, *data.constraints_coefficients)

    with open(data_dir, 'wb') as file:
        file.write(packed_data)

    # Read struct array from the file
    with open(data_dir, 'rb') as file:
        data = file.read()
        unpacked_data = struct.unpack(format_string, data)
    
    print("Unpacked data:", unpacked_data)


if __name__ == "__main__":
    main(data)