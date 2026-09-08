import numpy as np
from scipy import ndimage
import os
import utils

def get_gaussian_kernel(sigma: float) -> np.ndarray:
    size = int(2 * np.ceil(3 * sigma) + 1)
    ax = np.arange(-size // 2 + 1., size // 2 + 1.)
    xx, yy = np.meshgrid(ax, ax)
    kernel = np.exp(-(xx**2 + yy**2) / (2. * sigma**2))
    return kernel / np.sum(kernel)

def get_mean_kernel(size: int) -> np.ndarray:
    if size % 2 == 0:
        size += 1
    kernel = np.ones((size, size), dtype=np.float64)
    return kernel / np.sum(kernel)

def get_laplacian_kernel() -> np.ndarray:
    return np.array([[ 0,  1,  0],
                     [ 1, -4,  1],
                     [ 0,  1,  0]], dtype=np.float64)

def unsharp_masking(image: np.ndarray, kernel: np.ndarray, alpha: float = 1.5) -> np.ndarray:
    """
    Applies unsharp masking. Applies filter separately to R, G, B channels if image is colored.
    """
    if image.ndim == 3:
        enhanced = np.zeros_like(image)
        for c in range(image.shape[2]):
            blurred = ndimage.convolve(image[:, :, c], kernel, mode='reflect')
            high_freq = image[:, :, c] - blurred
            enhanced[:, :, c] = image[:, :, c] + alpha * high_freq
    else:
        blurred = ndimage.convolve(image, kernel, mode='reflect')
        high_freq = image - blurred
        enhanced = image + alpha * high_freq
        
    return np.clip(enhanced, image.min(), image.max())

if __name__ == "__main__":
    image_names = ["moon.png", "peacock.png", "tiger.png"]
    base_path = "../data/sharpen/"
    output_path = "output/"
    
    os.makedirs(output_path, exist_ok=True)
    
    gauss_kernel_1 = get_gaussian_kernel(sigma=1.0)
    gauss_kernel_2 = get_gaussian_kernel(sigma=2.5) 
    mean_kernel = get_mean_kernel(size=7)
    laplacian_kernel = get_laplacian_kernel()
    
    for img_name in image_names:
        img_path = os.path.join(base_path, img_name)
        if not os.path.exists(img_path):
            print(f"File not found: {img_path}")
            continue
            
        print(f"Processing: {img_name}")
        # Only grayscale for the moon, allowing RGB for peacock and tiger
        force_gray = (img_name == "moon.png")
        image = utils.load_image(img_path, as_gray=force_gray)
        
        sharp_level_1 = unsharp_masking(image, gauss_kernel_1, alpha=1.5)
        sharp_level_2 = unsharp_masking(image, gauss_kernel_2, alpha=3.0)
        
        save_file = os.path.join(output_path, f"Q1_sharp_{img_name}")
        utils.save_sharpening_comparison(
            image, sharp_level_1, sharp_level_2, 
            title1="Gaussian (sigma=1.0, alpha=1.5)", 
            title2="Gaussian (sigma=2.5, alpha=3.0)",
            save_path=save_file
        )
        
        sharp_mean = unsharp_masking(image, mean_kernel, alpha=1.5)
        diff_matrix = np.abs(sharp_level_1 - sharp_mean)
        mse = np.mean((sharp_level_1 - sharp_mean)**2)
        
        diff_save_file = os.path.join(output_path, f"Q1_diff_GaussVsMean_{img_name}")
        utils.save_difference_heatmap(
            diff_matrix, 
            title=f"Abs Difference\n({img_name})", 
            mse=mse,
            save_path=diff_save_file
        )


        # --- Direct Laplacian Edge Detection ---
        if image.ndim == 3:
            laplacian_edges = np.zeros_like(image)
            for c in range(image.shape[2]):
                laplacian_edges[:, :, c] = ndimage.convolve(image[:, :, c], laplacian_kernel, mode='reflect')
        else:
            laplacian_edges = ndimage.convolve(image, laplacian_kernel, mode='reflect')

        utils.save_difference_heatmap(
            np.abs(laplacian_edges), 
            title=f"Direct Laplacian Edges\n({img_name})", 
            mse=0.0,
            save_path=os.path.join(output_path, f"Q1_LaplacianEdges_{img_name}")
        )