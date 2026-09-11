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

def compute_image_gradients(image: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """
    Computes horizontal (Ix) and vertical (Iy) intensity gradients using Sobel operators.
    """
    sobel_x = np.array([[-1,  0,  1],
                        [-2,  0,  2],
                        [-1,  0,  1]], dtype=np.float64)
    
    sobel_y = np.array([[-1, -2, -1],
                        [ 0,  0,  0],
                        [ 1,  2,  1]], dtype=np.float64)
    
    Ix = convolve(image, sobel_x, mode='reflect')
    Iy = convolve(image, sobel_y, mode='reflect')
    
    return Ix, Iy

def detect_corners_and_edges(image: np.ndarray, pre_smoothing_sigma: float = 1.0):
    if image.ndim > 2:
        raise ValueError("Feature detection requires a 2D grayscale image.")
        
    # --- Stage 1: Preprocessing ---
    smoothed_image = apply_gaussian_smoothing(image, sigma=pre_smoothing_sigma)
    
    # --- Stage 2: Gradients ---
    Ix, Iy = compute_image_gradients(smoothed_image)
    
    return Ix, Iy