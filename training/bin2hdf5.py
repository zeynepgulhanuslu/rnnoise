#!/usr/bin/python

from __future__ import print_function

import numpy as np
import h5py
import sys

# Read the binary file into a NumPy array
data = np.fromfile(sys.argv[1], dtype='float32')

# Calculate the number of rows for the new shape (some_length x 87)
some_length = int(data.shape[0] / 87)

# Reshape the data array to the desired shape
reshaped_data = data.reshape(some_length, 87)

# Create an HDF5 file for writing
with h5py.File(sys.argv[4], 'w') as h5f:
    # Create an HDF5 dataset named 'data' and populate it with reshaped data
    h5f.create_dataset('data', data=reshaped_data)

print("Reshaped data saved to", sys.argv[4])

'''
data = np.fromfile(sys.argv[1], dtype='float32')
print(data.shape)
data = np.reshape(data, (int(sys.argv[2]), int(sys.argv[3])))
h5f = h5py.File(sys.argv[4], 'wb')
h5f.create_dataset('data', data=data)
h5f.close()

'''
