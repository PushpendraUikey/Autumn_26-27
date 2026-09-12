import numpy as np
from scipy.ndimage import convolve
from scipy.ndimage import maximum_filter, minimum_filter
import utils
import os

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


def extract_features_nms(score_map: np.ndarray, threshold: float, mode: str = 'corner', window_size: int = 5):
    """
    Applies Non-Maximum Suppression (NMS) using a local window and a threshold.
    Returns the coordinates, the continuous NMS map, and the binary mask map.
    """
    if mode == 'corner':
        local_extreme = maximum_filter(score_map, size=window_size)
        mask = (score_map == local_extreme) & (score_map > threshold)
    elif mode == 'edge':
        local_extreme = minimum_filter(score_map, size=window_size)
        mask = (score_map == local_extreme) & (score_map < -threshold)
    else:
        raise ValueError("Mode must be 'corner' or 'edge'.")
        
    y, x = np.where(mask)
    
    # Generate continuous NMS map (preserves score at peaks, 0 elsewhere)
    nms_map = np.where(local_extreme == score_map, score_map, 0)
    
    # Generate binary map (1.0 at feature locations, 0.0 elsewhere)
    bin_map = mask.astype(np.float64)
    
    return y, x, nms_map, bin_map

def extract_all_features(shi_tomasi: np.ndarray, harris: np.ndarray, 
                         t_st: float, t_harris_c: float, t_harris_e: float, 
                         nms_window: int = 5):
    """
    Runs thresholding and NMS for Shi-Tomasi corners, Harris corners, and Harris edges.
    """
    st_data = extract_features_nms(shi_tomasi, t_st, mode='corner', window_size=nms_window)
    h_corner_data = extract_features_nms(harris, t_harris_c, mode='corner', window_size=nms_window)
    h_edge_data = extract_features_nms(harris, t_harris_e, mode='edge', window_size=nms_window)
    
    return st_data, h_corner_data, h_edge_data

def process_and_visualize_features(img_name: str, base_path: str, output_path: str, params: dict):
    img_path = os.path.join(base_path, img_name)
    
    # 1. Load BOTH RGB (canvas) and Grayscale (math) versions
    rgb_image = utils.load_image(img_path, as_gray=False)
    gray_image = utils.load_image(img_path, as_gray=True)
    
    pre_sig = params.get("pre_sigma", 1.0)
    win_sig = params.get("window_sigma", 1.5)
    k = params.get("k", 0.04)
    t_st = params["t_st"]
    t_hc = params["t_harris_corner"]
    t_he = params["t_harris_edge"]
    nms_win = params.get("nms_window", 5)
    
    # Stages 1-6: Compute Maps (on Grayscale)
    l1, l2, shi_tomasi, harris = detect_corners_and_edges(gray_image, pre_sig, win_sig, k)
    
    # Stages 7-9: Extract Features
    st_data, hc_data, he_data = extract_all_features(
        shi_tomasi, harris, t_st, t_hc, t_he, nms_window=nms_win
    )
    
    st_y, st_x, st_nms, st_bin = st_data
    h_y, h_x, hc_nms, hc_bin = hc_data
    he_y, he_x, he_nms, he_bin = he_data
    
    # Stage 10: Save all 7 specific figures per image
    base_save_name = os.path.join(output_path, f"Q4_{img_name.split('.')[0]}")
    utils.save_q4_figures(rgb_image, gray_image, l1, l2, harris, 
                          st_nms, hc_nms, st_bin, hc_bin, 
                          he_nms, he_bin, 
                          st_y, st_x, h_y, h_x, he_y, he_x, base_save_name)
    print(f"  Finished extracting and plotting features for: {img_name}")

if __name__ == "__main__":
    base_path = "../data/corner/"
    output_path = "./output/"
    os.makedirs(output_path, exist_ok=True)
    
    # Structure Tensor values scale significantly with image contrast and texture.
    feature_params = {
        "nandadevi.png": {
            "pre_sigma": 1.5,          
            "window_sigma": 2.0,       
            "k": 0.04, 
            "nms_window": 5,
            "t_st": 0.05,              
            "t_harris_corner": 0.01,  
            "t_harris_edge": 0.02     
        },
        "paithaniCorner.png": {
            "pre_sigma": 1.0,          
            "window_sigma": 1.5,       
            "k": 0.04, 
            "nms_window": 5,
            "t_st": 0.02,              
            "t_harris_corner": 0.005,  
            "t_harris_edge": 0.01     
        },
        "warli.png": {
            "pre_sigma": 0.5,          
            "window_sigma": 1.0,       
            "k": 0.04, 
            "nms_window": 3,           
            "t_st": 0.1,               
            "t_harris_corner": 0.05,  
            "t_harris_edge": 0.05     
        }
    }
    
    for img_name, params in feature_params.items():
        print(f"Processing Q4 Structure Tensor pipeline for: {img_name}")
        process_and_visualize_features(img_name, base_path, output_path, params)