import scipy.io as sio
import numpy as np

# Load the MATLAB file
mat_contents = sio.loadmat('./data/ct.mat')

# The assignment specifies two images: "original" and "subsampled"
orig_img = mat_contents['original']
sub_img = mat_contents['subsampled']

# Export them to CSV so C++ can read them
np.savetxt('temp/1_f_ct_original.csv', orig_img, delimiter=',')
np.savetxt('temp/1_f_ct_subsampled.csv', sub_img, delimiter=',')
print("MAT file extracted successfully!")