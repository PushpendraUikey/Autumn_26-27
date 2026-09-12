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

def resize_image(img: np.ndarray, factor: float = None, size: tuple = None) -> np.ndarray:
    """
    Resizes a floating-point image array.
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
        im = ax.imshow(channel_data, cmap='viridis', vmin=-1.0, vmax=1.0)
        ax.set_title(title)
        ax.axis("off")
        cbar = fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
        cbar.ax.tick_params(labelsize=8)
        idx = np.unravel_index(np.argmax(channel_data), channel_data.shape)
        peak_val = channel_data[idx]
        ax.plot(idx[1], idx[0], marker='+', color='red', markersize=18, markeredgewidth=2)
        
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight')
    plt.close()