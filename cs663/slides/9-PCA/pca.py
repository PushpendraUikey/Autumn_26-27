"""CS663 Lecture 9 (PCA): visualising the generative model X = A W + mu.

Convention: every sample is a COLUMN vector, exactly as in the lecture.
A data matrix of N samples in D dimensions therefore has shape (D, N).
"""
import numpy as np
import matplotlib.pyplot as plt

def standardNormal(shape: tuple = None):
    return np.random.standard_normal(shape)

# A is a DxD square matrix, mean is a Dx1 vector, N is the number of samples
def gaussianRandomVar(mean: np.ndarray, A: np.ndarray, N: int):
    assert mean.ndim == 1
    assert A.shape[0] == mean.shape[0]
    shape = (mean.shape[0], N)
    W = standardNormal(shape)
    X = A @ W + mean[:, np.newaxis]
    return X

def plot2D(X: np.ndarray):
    assert X.ndim == 2
    assert X.shape[0] == 2
    plt.scatter(X[0, :], X[1, :])
    plt.axis('equal')
    plt.show()
    plt.close()

if __name__ == "__main__":
    # generate a 2D dataset of 100 samples
    N = 100
    D = 2
    X = standardNormal((D, N))
    plot2D(X)

    A = np.identity(D)
    mean = np.array([3,4])
    X = gaussianRandomVar(mean, A, N)
    print(X.shape)
    plot2D(X)
