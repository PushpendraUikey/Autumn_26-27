import os
import numpy as np
from scipy import ndimage
from scipy.signal import fftconvolve
from PIL import Image
import utils

def get_disc_kernel(diameter: int) -> np.ndarray:
    """ Generates a normalized circular disc filter of the specified diameter. """
    radius = diameter // 2
    y, x = np.ogrid[-radius:radius+1, -radius:radius+1]
    mask = x**2 + y**2 <= radius**2

    kernel = np.zeros((2*radius + 1, 2*radius + 1), dtype=np.float64)
    kernel[mask] = 1.0
    return kernel / np.sum(kernel)


def apply_bokeh(image: np.ndarray, fg_mask: np.ndarray, diameter: int) -> np.ndarray:
    """
    Applies a Bokeh blur strictly to the background using a distance transform
    to handle boundary cropping efficiently.
    """
    radius = diameter // 2
    h, w = image.shape[:2]

    if fg_mask.shape != (h, w):
        print(f"[Warning] Mask Shape mismatched, resizing to {(h, w)}...")
        pil_mask = Image.fromarray(fg_mask.astype(np.uint8) * 255)
        fg_mask = np.asarray(pil_mask.resize((w,h), Image.Resampling.NEAREST)) > 128
    
    kernel = get_disc_kernel(diameter)
    output = np.copy(image)
    bg_mask = ~fg_mask

    blurred_global = np.zeros_like(image)
    for c in range(image.shape[2]):
        blurred_global[:, :, c] = fftconvolve(image[:, :, c], kernel, mode='same')

    # padded_bg = np.zeros((h+2, w+2), dtype=bool)
    # padded_bg[1:-1, 1:-1] = bg_mask
    padded_bg = np.pad(bg_mask, pad_width=1, mode='constant', constant_values=False)

    dist_map = ndimage.distance_transform_edt(padded_bg)[1:-1, 1:-1]

    safe_bg_mask = (dist_map > radius) & bg_mask
    unsafe_bg_mask = (dist_map <= radius) & bg_mask

    output[safe_bg_mask] = blurred_global[safe_bg_mask]

    unsafe_y, unsafe_x = np.where(unsafe_bg_mask)

    for y, x in zip(unsafe_y, unsafe_x):
        y_min = max(0, y-radius)
        y_max = min(h, y+radius+1)
        x_min = max(0, x-radius)
        x_max = min(w, x+radius+1)

        ky_min = radius - (y-y_min)
        ky_max = radius + (y_max-y)
        kx_min = radius - (x-x_min)
        kx_max = radius + (x_max-x)

        k_crop = kernel[ky_min:ky_max, kx_min:kx_max]
        bg_crop = bg_mask[y_min:y_max, x_min:x_max]

        valid_k = k_crop * bg_crop
        k_sum = np.sum(valid_k)

        if k_sum > 0:
            valid_k /= k_sum
            patch = image[y_min:y_max, x_min:x_max]
            for c in range(image.shape[2]):
                output[y,x,c] = np.sum(patch[:, :, c] * valid_k)

    return output

if __name__ == "__main__":
    base_path = "../data/bokeh/"
    output_path = "./output/"
    if not os.path.exists(output_path):
        os.makedirs(output_path)
    images = ["deep.png", "lotus.png", "marigold.png"]

    for img_name in images:
        print(f"Processing Bokeh effect for : {img_name}")
        img_path = os.path.join(base_path, img_name)

        mask_name = img_name.replace(".png", "_mask.png")
        mask_path = os.path.join(output_path, mask_name)

        if not os.path.exists(mask_path):
            print(f" Missing mask {mask_path}. Run generate_masks.py first!")
            continue
        image = utils.load_image(img_path)
        mask_img = Image.open(mask_path).convert('L')
        fg_mask = np.asarray(mask_img) > 128

        bokeh_50 = apply_bokeh(image, fg_mask, diameter=50)
        print(" Finished diameter = 50")

        bokeh_100 = apply_bokeh(image, fg_mask, diameter=100)
        print(" Finished diameter = 100")

        save_file = os.path.join(output_path, f"Q5_bokeh_{img_name}")
        utils.save_bokeh_comparison(image, bokeh_50, bokeh_100, save_file)
