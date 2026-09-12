import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def load_image(image_path: str, as_gray: bool = False) -> np.ndarray:
    img = Image.open(image_path)
    if as_gray:
        img = img.convert('L')
    
    # Normalize to [0, 1] for floating point processing
    arr = np.asarray(img, dtype=np.float64) / 255.0
    return arr

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