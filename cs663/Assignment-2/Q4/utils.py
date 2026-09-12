import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import matplotlib.colors as mcolors

def load_image(image_path: str, as_gray: bool = False) -> np.ndarray:
    img = Image.open(image_path)
    if as_gray:
        img = img.convert('L')
    
    # Normalize to [0, 1] for floating point processing
    arr = np.asarray(img, dtype=np.float64) / 255.0
    return arr


def save_q4_figures(rgb_img, gray_img, l1, l2, harris_raw, 
                    st_nms, hc_nms, st_bin, hc_bin, 
                    he_nms, he_bin, 
                    st_y, st_x, h_y, h_x, he_y, he_x, base_save_name):
    
    # Fig 1: Eigenvalues
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    axes[0].imshow(rgb_img); axes[0].set_title("Input Image")
    axes[1].imshow(l1, cmap='viridis'); axes[1].set_title("Lambda 1 (Largest)")
    axes[2].imshow(l2, cmap='viridis'); axes[2].set_title("Lambda 2 (Smallest / Shi-Tomasi)")
    for ax in axes: ax.axis('off')
    plt.savefig(f"{base_save_name}_1_Eigenvalues.png", bbox_inches='tight', dpi=200); plt.close()

    # Fig 2: Harris Invariant (Symmetrical Log Normalization)
    fig, axes = plt.subplots(1, 2, figsize=(10, 5))
    axes[0].imshow(rgb_img); axes[0].set_title("Input Image")
    v_max = np.max(np.abs(harris_raw))
    norm = mcolors.SymLogNorm(linthresh=1e-5, vmin=-v_max, vmax=v_max, base=10)
    axes[1].imshow(harris_raw, cmap='seismic', norm=norm)
    axes[1].set_title("Raw Harris C Map")
    
    for ax in axes: ax.axis('off')
    plt.savefig(f"{base_save_name}_2_HarrisRaw.png", bbox_inches='tight', dpi=200); plt.close()

    # Fig 3: NMS Corner Scores
    # Use SymLogNorm to make sparse extreme values visible against the zero-background
    st_norm = mcolors.SymLogNorm(linthresh=1e-4, vmin=0, base=10)
    hc_norm = mcolors.SymLogNorm(linthresh=1e-4, vmin=0, base=10)
    
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    axes[0].imshow(rgb_img); axes[0].set_title("Input Image")
    axes[1].imshow(st_nms, cmap='viridis', norm=st_norm); axes[1].set_title("NMS Shi-Tomasi")
    axes[2].imshow(hc_nms, cmap='viridis', norm=hc_norm); axes[2].set_title("NMS Harris Corner")
    for ax in axes: ax.axis('off')
    plt.savefig(f"{base_save_name}_3_NMS_Corners.png", bbox_inches='tight', dpi=200); plt.close()

    # Fig 4: Binary Corner Masks
    # Enforce vmin=0, vmax=1 to guarantee pure black and pure white
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    axes[0].imshow(rgb_img); axes[0].set_title("Input Image")
    axes[1].imshow(st_bin, cmap='gray', vmin=0, vmax=1); axes[1].set_title("Binary Shi-Tomasi")
    axes[2].imshow(hc_bin, cmap='gray', vmin=0, vmax=1); axes[2].set_title("Binary Harris Corner")
    for ax in axes: ax.axis('off')
    plt.savefig(f"{base_save_name}_4_Binary_Corners.png", bbox_inches='tight', dpi=200); plt.close()

    # Fig 5: Final Corner Overlays
    fig, axes = plt.subplots(1, 2, figsize=(16, 8))
    
    axes[0].imshow(rgb_img)
    axes[0].scatter(st_x, st_y, c='black', marker='+', s=30, label='Shi-Tomasi')
    axes[0].set_title("Shi-Tomasi Corners Overlaid")
    axes[0].legend()
    
    axes[1].imshow(rgb_img)
    axes[1].scatter(h_x, h_y, c='black', marker='x', s=30, label='Harris')
    axes[1].set_title("Harris Corners Overlaid")
    axes[1].legend()
    
    for ax in axes: ax.axis('off')
    plt.savefig(f"{base_save_name}_5_Corner_Overlay.png", bbox_inches='tight', dpi=300); plt.close()

    # Fig 6: Harris Edges Processing
    # Harris NMS edges are negative. Take absolute value so they pop out from 0 in viridis
    he_norm = mcolors.SymLogNorm(linthresh=1e-4, vmin=0, base=10)
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    axes[0].imshow(rgb_img); axes[0].set_title("Input Image")
    axes[1].imshow(np.abs(he_nms), cmap='viridis', norm=he_norm); axes[1].set_title("NMS Harris Edge (Abs Val)")
    axes[2].imshow(he_bin, cmap='gray', vmin=0, vmax=1); axes[2].set_title("Binary Harris Edge")
    for ax in axes: ax.axis('off')
    plt.savefig(f"{base_save_name}_6_Harris_Edges.png", bbox_inches='tight', dpi=200); plt.close()

    # Fig 7: Final Edge Overlay (High Contrast Color)
    fig, ax = plt.subplots(1, 1, figsize=(8, 8))
    ax.imshow(rgb_img)
    ax.scatter(he_x, he_y, c='white', s=5, marker='s') # Magenta squares jump out on screen
    ax.set_title("Harris Edges Overlaid")
    ax.axis('off')
    plt.savefig(f"{base_save_name}_7_Edge_Overlay.png", bbox_inches='tight', dpi=300); plt.close()