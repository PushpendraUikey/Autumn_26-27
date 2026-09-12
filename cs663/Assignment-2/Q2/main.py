import numpy as np
from scipy.signal import correlate
from PIL import Image
import os
import utils

# --- Q2(a): Unmasked NCC ---
def template_Match_unmasked(img: np.ndarray, template: np.ndarray) -> np.ndarray:
    """Computes NCC using perfectly aligned 'valid' FFT convolutions."""
    if img.ndim == 3 and template.ndim == 3:
        ncc_r = template_Match_unmasked(img[:, :, 0], template[:, :, 0])
        ncc_g = template_Match_unmasked(img[:, :, 1], template[:, :, 1])
        ncc_b = template_Match_unmasked(img[:, :, 2], template[:, :, 2])
        return np.stack([ncc_r, ncc_g, ncc_b], axis=-1)

    img, template = img.astype(np.float64), template.astype(np.float64)
    t_h, t_w = template.shape
    N = t_h * t_w

    t_mean = np.mean(template)
    t_std = np.std(template)
    if t_std == 0:
        return np.zeros((img.shape[0] - t_h + 1, img.shape[1] - t_w + 1))

    t_zero_mean = template - t_mean

    # Numerator
    numerator = correlate(img, t_zero_mean, mode='valid', method='fft')

    # Local Patch Statistics (Replaces uniform_filter to fix boundaries)
    kernel_ones = np.ones((t_h, t_w), dtype=np.float64)
    local_sum = correlate(img, kernel_ones, mode='valid', method='fft')
    local_sq_sum = correlate(img**2, kernel_ones, mode='valid', method='fft')

    local_mean = local_sum / N
    local_var = np.maximum((local_sq_sum / N) - local_mean**2, 0)
    local_std = np.sqrt(local_var)

    ncc_map = np.zeros_like(numerator)
    valid_mask = local_std > 1e-8
    ncc_map[valid_mask] = numerator[valid_mask] / (local_std[valid_mask] * t_std * N)

    return np.clip(ncc_map, -1.0, 1.0)


# --- Q2(c): Masked NCC ---
def template_Match_masked(img: np.ndarray, template: np.ndarray, mask: np.ndarray) -> np.ndarray:
    """Computes NCC evaluating exclusively the pixels isolated by the binary mask."""
    if img.ndim == 3 and template.ndim == 3:
        ncc_r = template_Match_masked(img[:, :, 0], template[:, :, 0], mask)
        ncc_g = template_Match_masked(img[:, :, 1], template[:, :, 1], mask)
        ncc_b = template_Match_masked(img[:, :, 2], template[:, :, 2], mask)
        return np.stack([ncc_r, ncc_g, ncc_b], axis=-1)

    img, template, mask = img.astype(np.float64), template.astype(np.float64), mask.astype(np.float64)
    N_mask = np.sum(mask)
    if N_mask == 0:
        return np.zeros((img.shape[0] - template.shape[0] + 1, img.shape[1] - template.shape[1] + 1))

    # Masked Template Normalization
    t_mean = np.sum(template * mask) / N_mask
    t_zero_mean = (template - t_mean) * mask
    t_std = np.sqrt(np.sum(t_zero_mean**2) / N_mask)
    if t_std == 0:
        return np.zeros_like(img)

    # Numerator
    numerator = correlate(img, t_zero_mean, mode='valid', method='fft')

    # Masked Local Patch Statistics
    local_sum = correlate(img, mask, mode='valid', method='fft')
    local_sq_sum = correlate(img**2, mask, mode='valid', method='fft')

    local_mean = local_sum / N_mask
    local_var = np.maximum((local_sq_sum / N_mask) - local_mean**2, 0)
    local_std = np.sqrt(local_var)

    ncc_map = np.zeros_like(numerator)
    valid_mask = local_std > 1e-8
    ncc_map[valid_mask] = numerator[valid_mask] / (local_std[valid_mask] * t_std * N_mask)

    return np.clip(ncc_map, -1.0, 1.0)


if __name__ == "__main__":
    raw_img = utils.load_image("../data/templateMatch/parking.png")
    raw_template = utils.load_image("../data/templateMatch/templateNoPark.png")
    
    os.makedirs("./output", exist_ok=True)

    # --- Q2(b): Unmasked Evaluation ---
    # Scene IS reduced by factor of 5
    img_q2b = utils.resize_image(raw_img, factor=0.2)
    sizes_q2b = [(41, 41), (51, 51), (61, 61)]
    
    for size in sizes_q2b:
        print(f"Q2(b) - Processing Unmasked: {size[0]}x{size[1]}")
        temp_scaled = utils.resize_image(raw_template, size=size)
        ncc_result = template_Match_unmasked(img_q2b, temp_scaled)
        
        utils.save_ncc_grid(
            ncc_result[:, :, 0], ncc_result[:, :, 1], ncc_result[:, :, 2], 
            template_size=f"{size[0]}x{size[1]}", 
            save_path=f"./output/Q2b_unmasked_ncc_{size[0]}x{size[1]}.png"
        )

    # --- Q2(d): Masked Evaluation ---
    # Scene IS NOT reduced. It is tested against the large templates.
    img_q2d = raw_img 
    sizes_q2d = [(201, 201), (251, 251), (301, 301)]
    
    # Generate original binary mask: True if not pure white
    # Assumes white background approaches 1.0 in RGB
    raw_mask = (raw_template < 0.95).any(axis=-1).astype(np.float64)

    for size in sizes_q2d:
        print(f"Q2(d) - Processing Masked: {size[0]}x{size[1]}")
        temp_scaled = utils.resize_image(raw_template, size=size)
        
        # Resize mask exclusively using Nearest Neighbor to maintain 0 or 1 binary constraint
        pil_mask = Image.fromarray(np.uint8(raw_mask * 255))
        mask_scaled = np.asarray(pil_mask.resize(size, Image.Resampling.NEAREST)) > 128
        mask_scaled = mask_scaled.astype(np.float64)

        ncc_result = template_Match_masked(img_q2d, temp_scaled, mask_scaled)
        
        utils.save_ncc_grid(
            ncc_result[:, :, 0], ncc_result[:, :, 1], ncc_result[:, :, 2], 
            template_size=f"{size[0]}x{size[1]} (Masked)", 
            save_path=f"./output/Q2d_masked_ncc_{size[0]}x{size[1]}.png"
        )