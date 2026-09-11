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

def compute_tensor_components(Ix: np.ndarray, Iy: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Computes the raw per-pixel components of the Structure Tensor.
    """
    Ixx = Ix ** 2
    Iyy = Iy ** 2
    Ixy = Ix * Iy
    
    return Ixx, Iyy, Ixy

def aggregate_tensor_components(Ixx: np.ndarray, Iyy: np.ndarray, Ixy: np.ndarray, window_sigma: float = 1.5) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Aggregates the raw tensor components over a Gaussian-weighted local window 
    to form a full-rank Structure Tensor at each pixel.
    """
    # Reuse the Gaussian smoothing function to apply the weighted window
    Sxx = apply_gaussian_smoothing(Ixx, sigma=window_sigma)
    Syy = apply_gaussian_smoothing(Iyy, sigma=window_sigma)
    Sxy = apply_gaussian_smoothing(Ixy, sigma=window_sigma)
    
    return Sxx, Syy, Sxy

def compute_eigenvalues(Sxx: np.ndarray, Syy: np.ndarray, Sxy: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """
    Computes the largest (lambda_1) and smallest (lambda_2) eigenvalues 
    for the 2x2 Structure Tensor at each pixel using the closed-form solution.
    """
    mean = (Sxx + Syy) / 2.0
    diff = (Sxx - Syy) / 2.0
    
    discriminant = np.sqrt(diff**2 + Sxy**2)
    
    # lambda_1 is guaranteed to be >= lambda_2 because discriminant is always >= 0
    lambda_1 = mean + discriminant
    lambda_2 = mean - discriminant
    
    return lambda_1, lambda_2

def calculate_corner_scores(lambda_1: np.ndarray, lambda_2: np.ndarray, 
                            Sxx: np.ndarray, Syy: np.ndarray, Sxy: np.ndarray, 
                            k: float = 0.04) -> tuple[np.ndarray, np.ndarray]:
    """
    Calculates both Shi-Tomasi and Harris-Stephens scoring measures.
    """
    # Shi-Tomasi Measure: The minimum eigenvalue
    shi_tomasi_score = lambda_2
    
    # Harris-Stephens Measure: det(M) - k * (trace(M))^2
    det_M = (Sxx * Syy) - (Sxy**2)
    trace_M = Sxx + Syy
    harris_score = det_M - k * (trace_M**2)
    
    return shi_tomasi_score, harris_score

def detect_corners_and_edges(image: np.ndarray, pre_smoothing_sigma: float = 1.0, 
                             window_sigma: float = 1.5, k: float = 0.04):
    if image.ndim > 2:
        raise ValueError("Feature detection requires a 2D grayscale image.")
        
    # --- Stage 1 & 2: Preprocessing and Gradients ---
    smoothed = apply_gaussian_smoothing(image, sigma=pre_smoothing_sigma)
    Ix, Iy = compute_image_gradients(smoothed)
    
    # --- Stage 3 & 4: Tensor Components and Aggregation ---
    Ixx, Iyy, Ixy = compute_tensor_components(Ix, Iy)
    Sxx, Syy, Sxy = aggregate_tensor_components(Ixx, Iyy, Ixy, window_sigma)
    
    # --- Stage 5: Eigen-decomposition and Scoring ---
    lambda_1, lambda_2 = compute_eigenvalues(Sxx, Syy, Sxy)
    shi_tomasi, harris = calculate_corner_scores(lambda_1, lambda_2, Sxx, Syy, Sxy, k)
    
    return lambda_1, lambda_2, shi_tomasi, harris