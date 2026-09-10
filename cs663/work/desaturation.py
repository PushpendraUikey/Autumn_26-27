import numpy as np
from scipy import ndimage
from PIL import Image
import os
import matplotlib.pyplot as plt

def load_image(image_path: str, as_gray: bool = False) -> np.ndarray:
    """
    Loads an image from the specified path and converts it to a numpy array.
    If as_gray is True, the image is converted to grayscale.
    """
    from PIL import Image
    img = Image.open(image_path)
    if as_gray:
        img = img.convert('L')
    
    # Normalize to [0, 1] for floating point processing
    arr = np.asarray(img, dtype=np.float64) / 255.0
    return arr

def save_img_comparison(orig: np.ndarray, trns: np.ndarray, title: str, save_path: str):
    is_rgb = orig.ndim == 3
    vmin, vmax = (0.0, 1.0) if is_rgb else (orig.min(), orig.max())

    fig, axes = plt.subplots(1, 2, figsize=(10, 5))
    images = [orig, trns]

    titles = ["Original Image", title]
    for ax, img, title in zip(axes, images, titles):
        if is_rgb:
            im = ax.imshow(img)
        else:
            im = ax.imshow(img, cmap=plt.get_cmap('gray', 256), vmin=vmin, vmax=vmax)

        ax.set_title(title)
        ax.axis("off")
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches='tight', dpi=300)
    plt.close()


def desaturate_image(image: np.ndarray, s: float) -> np.ndarray:
    """
    Converts an RGB image to grayscale using the luminosity method.
    """
    if image.ndim == 3 and image.shape[2] == 3:
        # Luminosity method: 0.21 R + 0.72 G + 0.07 B
        gray_image = 0.21 * image[:, :, 0] + 0.72 * image[:, :, 1] + 0.07 * image[:, :, 2]
        final_image = s * image + (1 - s) * gray_image[:, :, np.newaxis]
        return final_image
    else:
        raise ValueError("Input image must be an RGB image with shape (H, W, 3).")

def adjust_saturation(img: np.ndarray, s: float) -> np.ndarray:
    if img.ndim == 3 and img.shape[2] == 3:
        A = np.mean(img, axis=2, keepdims=True)
        final_img = s * img + (1 - s) * A
        return np.clip(final_img, 0, 255).astype(img.dtype)

    else:
        raise ValueError("Input image must be an RGB image with shape (H, W, 3).")

if __name__ == "__main__":
    base_path = "./data/"
    output_path = "./output/"
    os.makedirs(output_path, exist_ok=True)


    for img in os.listdir(base_path):
        if img.endswith(('.png', '.jpg', '.jpeg')):
            print(f"Processing image: {img}")
            img_path = os.path.join(base_path, img)
            image = load_image(img_path, as_gray=False)

            s1 = 0.3  # Desaturation factor
            desaturated_image = desaturate_image(image, s1)
            s2 = 0.3  # Adjustment factor
            adjusted_image = adjust_saturation(image, s2)
            save_image_path = os.path.join(output_path, f"desaturated_{img}")
            save_img_comparison(image, desaturated_image, f"Desaturated (s={s1})", save_image_path)
            save_image_path = os.path.join(output_path, f"adjusted_{img}")
            save_img_comparison(image, adjusted_image, f"Adjusted (s={s2})", save_image_path)