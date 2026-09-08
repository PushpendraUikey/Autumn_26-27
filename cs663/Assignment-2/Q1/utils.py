import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import os

def load_image(image_path: str, as_gray: bool = False) -> np.ndarray:
    img = Image.open(image_path)
    if as_gray:
        img = img.convert('L')
    
    # Normalize to [0, 1] for floating point processing
    arr = np.asarray(img, dtype=np.float64) / 255.0
    return arr

def save_sharpening_comparison(orig: np.ndarray, sharp1: np.ndarray, sharp2: np.ndarray, 
                               title1: str, title2: str, save_path: str):
    """
    Saves the original image and two sharpened versions on the exact same colorscale.
    """
    is_rgb = orig.ndim == 3
    vmin, vmax = (0.0, 1.0) if is_rgb else (orig.min(), orig.max())
    
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    images = [orig, sharp1, sharp2]
    titles = ["Original Image", title1, title2]
    
    for ax, img, title in zip(axes, images, titles):
        if is_rgb:
            im = ax.imshow(img)
        else:
            im = ax.imshow(img, cmap=plt.get_cmap('gray', 256), vmin=vmin, vmax=vmax)
            
        ax.set_title(title)
        ax.axis("off")
        
        # Colorbars are required for grayscale images
        if not is_rgb:
            cbar = fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
            cbar.ax.tick_params(labelsize=8)
        
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()

def save_difference_heatmap(diff: np.ndarray, title: str, mse: float, save_path: str):
    """
    Saves an absolute difference matrix using a high-contrast heatmap.
    """
    # If RGB difference, take the mean across channels for a 2D heatmap
    if diff.ndim == 3:
        diff = np.mean(diff, axis=-1)

    plt.figure(figsize=(6, 5))
    im = plt.imshow(diff, cmap='hot')
    plt.title(f"{title}\nMSE: {mse:.6f}")
    plt.axis("off")
    cbar = plt.colorbar(im, fraction=0.046, pad=0.04)
    cbar.ax.tick_params(labelsize=8)
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()