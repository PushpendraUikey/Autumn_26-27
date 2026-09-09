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


def resize_image(img: np.ndarray, factor: float = None, size: tuple = None) -> np.ndarray:
    """
    Resizes a floating-point image array.
    Provide either a scaling 'factor' (e.g., 0.2 for 1/5th) or an exact (width, height) 'size'.
    """
    # Convert back to PIL Image temporarily for high-quality resampling
    pil_img = Image.fromarray(np.uint8(img * 255))
    
    if factor is not None:
        new_size = (int(pil_img.width * factor), int(pil_img.height * factor))
    elif size is not None:
        new_size = size
    else:
        return img
        
    resized = pil_img.resize(new_size, Image.Resampling.LANCZOS)
    return np.asarray(resized, dtype=np.float64) / 255.0

def save_ncc_grid(ncc_r: np.ndarray, ncc_g: np.ndarray, ncc_b: np.ndarray, 
                  template_size: str, save_path: str):
    """
    Saves a 1x3 grid displaying the NCC heatmaps for the Red, Green, and Blue channels.
    """
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    channels = [ncc_r, ncc_g, ncc_b]
    titles = [f"Red Channel ({template_size})", 
              f"Green Channel ({template_size})", 
              f"Blue Channel ({template_size})"]
    
    for ax, channel_data, title in zip(axes, channels, titles):
        im = ax.imshow(channel_data, cmap='viridis')
        ax.set_title(title)
        ax.axis("off")
        cbar = fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
        cbar.ax.tick_params(labelsize=8)
        
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight')
    plt.close()

def save_bokeh_comparison(orig: np.ndarray, bokeh_50: np.ndarray, bokeh_100:np.ndarray, save_path:str):
    """ 
    Saves a 1x3 grid comparing the original image with the two Bokeh-filtered version
    """
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    images = [orig, bokeh_50, bokeh_100]
    titles = ["Original", "Bokeh (d=50)", "Bokeh (d=100)"]

    for ax, img, title in zip(axes, images, titles):
        ax.imshow(np.clip(img, 0.0, 1.0))
        ax.set_title(title)
        ax.axis("off")

    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()