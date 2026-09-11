import numpy as np
from scipy.ndimage import convolve

def apply_gaussian_smoothing(image: np.ndarray, sigma: float = 1.0) -> np.ndarray:
    """
    Suppresses noise using a 2D Gaussian filter to prevent false corner detections.
    """
    size = int(2 * np.ceil(3 * sigma) + 1)
    ax = np.arange(-size // 2 + 1., size // 2 + 1.)
    xx, yy = np.meshgrid(ax, ax)
    
    kernel = np.exp(-(xx**2 + yy**2) / (2. * sigma**2))
    kernel = kernel / np.sum(kernel)
    
    return convolve(image, kernel, mode='reflect')

def detect_corners_and_edges(image: np.ndarray, pre_smoothing_sigma: float = 1.0):
    """
    Stage 1: Preprocessing for Structure Tensor feature detection.
    """
    if image.ndim > 2:
        raise ValueError("Feature detection requires a 2D grayscale image.")
        
    # Apply light Gaussian blur to mitigate rapid intensity spikes
    smoothed_image = apply_gaussian_smoothing(image, sigma=pre_smoothing_sigma)
    
    return smoothed_image