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

def save_canny_stages(orig: np.ndarray, mag: np.ndarray, nms: np.ndarray, final: np.ndarray,
                      title: str, save_path: str):
 
    fig, axes = plt.subplots(1, 4, figsize=(20, 5))
    images = [orig, mag, nms, final]
    titles = ["Original Image", "Gradient Magnitude", "Non-Maximum Suppression", "Final Edges"]

    for ax, img, ttl in zip(axes, images, titles):
        ax.imshow(img, cmap='gray')
        ax.set_title(ttl)
        ax.axis("off")

    plt.suptitle(title, fontsize=14)
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()


def save_input_binary_pair(orig: np.ndarray, final: np.ndarray, title: str, save_path: str):

    fig, axes = plt.subplots(1, 2, figsize=(12, 6))
    axes[0].imshow(orig, cmap='gray')
    axes[0].set_title("Input Image")
    axes[0].axis("off")

    axes[1].imshow(final, cmap='gray', vmin=0, vmax=1)
    axes[1].set_title("Binary Edge Map")
    axes[1].axis("off")

    plt.suptitle(title, fontsize=13)
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()


def save_edge_overlay(orig: np.ndarray, final: np.ndarray, title: str, save_path: str):

    if orig.ndim == 2:
        # Grayscale -> stack to RGB so "black" reads as true black (0,0,0)
        # instead of being tinted by a colormap when displayed.
        composite = np.stack([orig, orig, orig], axis=-1).copy()
    else:
        composite = orig.copy()

    edge_mask = final > 0
    composite[edge_mask] = 0.0  # paint edge pixels black

    plt.figure(figsize=(8, 8))
    plt.imshow(composite)
    plt.title(title)
    plt.axis("off")
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()