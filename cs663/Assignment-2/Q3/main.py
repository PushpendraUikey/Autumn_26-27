import numpy as np
from scipy import ndimage

# ------------------ Q3 Canny Edge Detection ------------------ #
# PHASE1 : Guassian noise suppression function
def apply_guassian_smoothing(image: np.ndarray, sigma: float) -> np.ndarray:
    """
    Applies Gaussian smoothing to the input image using a specified sigma value.
    """
    # smoothed_image = ndimage.gaussian_filter(image, sigma=sigma)
    size = int(2 * np.ceil(3*sigma) + 1)
    ax = np.arange(-size//2 + 1. , size//2 + 1.)
    xx, yy = np.meshgrid(ax, ax)
    kernel = np.exp(-(xx**2 + yy**2) / (2. * sigma**2))
    kernel = kernel / np.sum(kernel)

    # convolution with boundary reflection
    smoothed_image = ndimage.convolve(image, kernel, mode='reflect') 
    return smoothed_image

# PHASE2 : Gradient Computation using Sobel Operators on smoothed image
def compute_gradients(smoothed_img: np.ndarray) -> tuple [np.ndarray, np.ndarray] :
    """
    Computes the gradient magnitude and direction of an image using Soble Operators.
    """
    sobel_x = np.array([ [-1, 0, +1],
                         [-2, 0, +2],
                         [-1, 0, +1]], dtype=np.float64)
    sobel_y = np.array([ [-1, -2, -1],
                         [0,  0,  0], 
                         [+1, +2, +1],], dtype=np.float64)

    Gx = ndimage.convolve(smoothed_img, sobel_x, mode='reflect')
    Gy = ndimage.convolve(smoothed_img, sobel_y, mode='reflect')

    magnitude = np.sqrt(Gx**2 + Gy**2)
    direction = np.arctan2(Gy, Gx)

    direction = np.rad2deg(direction)
    direction[direction < 0] += 180

    if magnitude.max() > 0:
        magnitude = (magnitude / magnitude.max())

    return magnitude, direction