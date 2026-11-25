"""CS663 Lecture 9 (PCA): visualising the generative model X = A W + mu.

Convention: every sample is a COLUMN vector, exactly as in the lecture.
A data matrix of N samples in D dimensions therefore has shape (D, N).
"""
import numpy as np

def standardNormal(shape: tuple = None):
    return np.random.standard_normal(shape)

if __name__ == "__main__":
    # generate a 2D dataset of 100 samples
    N = 100
    D = 2
    X = standardNormal((D, N))
    print("X.shape = ", X.shape)
    print("X = ", X)