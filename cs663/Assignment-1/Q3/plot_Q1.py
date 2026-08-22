import numpy as np
import matplotlib.pyplot as plt
import os

# Ensure output directory exists
os.makedirs('output_img', exist_ok=True)

# ==========================================
# GENERALIZED PLOTTING & UTILITY FUNCTIONS
# ==========================================

def save_plot_with_histograms(images, titles, filepath):
    """
    Plots images on the top row and their corresponding histograms on the bottom row.
    Handles both Grayscale (2D) and RGB (3D) images.
    """
    n = len(images)
    fig, axes = plt.subplots(2, n, figsize=(6 * n, 10))
    
    # Handle single image edge case
    if n == 1:
        axes = np.array([[axes[0]], [axes[1]]])

    for i in range(n):
        img = images[i]
        
        # 1. Plot Image (Top Row)
        ax_img = axes[0, i]
        if img.ndim == 2:
            ax_img.imshow(img, cmap='gray', aspect='equal', vmin=0, vmax=255)
        else:
            ax_img.imshow(img, aspect='equal')
            
        ax_img.set_title(titles[i])
        ax_img.set_xlabel("Pixel Units (X)")
        ax_img.set_ylabel("Pixel Units (Y)")

        # 2. Plot Histogram (Bottom Row)
        ax_hist = axes[1, i]
        if img.ndim == 2:
            # Grayscale histogram
            ax_hist.hist(img.ravel(), bins=256, range=(0, 255), color='black', alpha=0.7)
        else:
            # RGB histogram (plot luminance or individual channels)
            # Standard luminance formula: 0.299*R + 0.587*G + 0.114*B
            luminance = 0.299 * img[:,:,0] + 0.587 * img[:,:,1] + 0.114 * img[:,:,2]
            ax_hist.hist(luminance.ravel(), bins=256, range=(0, 255), color='gray', alpha=0.7)
            
        ax_hist.set_title(f"Histogram: {titles[i]}")
        ax_hist.set_xlabel("Intensity Value")
        ax_hist.set_ylabel("Pixel Count")
        ax_hist.set_xlim([0, 255])

    plt.tight_layout()
    fig.savefig(filepath, dpi=300, bbox_inches='tight')
    plt.close(fig)

def load_and_reshape(filepath, h, w, channels):
    """Loads a CSV into a correctly shaped numpy array."""
    flat_data = np.loadtxt(filepath, delimiter=',')
    if channels == 1:
        img = flat_data.reshape((h, w))
    else:
        img = flat_data.reshape((h, w, channels))
    return np.clip(img, 0, 255).astype(np.uint8)

# ==========================================
# ASSIGNMENT EXECUTIONS
# ==========================================

def run_part_3a():
    """Generates the side-by-side plot and histograms for Part 3(a)."""
    orig_leh = plt.imread('./data/leh.png')
    
    # Scale float representations to 0-255 if needed
    if orig_leh.dtype in [np.float32, np.float64]:
        orig_leh = (orig_leh * 255).astype(np.uint8)
        
    H, W = orig_leh.shape[:2]
    
    try:
        # Load C++ Output
        stretched = load_and_reshape('temp/3_a_leh_linear.csv', H, W, 3)
        
        # The helper function automatically computes and plots the histograms[cite: 3]
        save_plot_with_histograms(
            [orig_leh, stretched], 
            ["Original (leh.png)", "Linear Contrast Stretch"], 
            'output_img/3_a_op_leh_linear.png'
        )
        print("Part 3(a) plots saved.")
    except OSError:
        print("Warning: CSV file for 3(a) not found.")

def run_part_3b():
    """Generates the side-by-side plot and histograms for Part 3(b)."""
    orig_leh = plt.imread('./data/leh.png')
    
    # Scale float representations to 0-255 if needed
    if orig_leh.dtype in [np.float32, np.float64]:
        orig_leh = (orig_leh * 255).astype(np.uint8)
        
    H, W = orig_leh.shape[:2]
    
    try:
        # Load C++ Output
        he_img = load_and_reshape('temp/3_b_leh_he.csv', H, W, 3)
        
        # The helper function automatically computes and plots the histograms[cite: 3]
        save_plot_with_histograms(
            [orig_leh, he_img], 
            ["Original (leh.png)", "Histogram Equalization"], 
            'output_img/3_b_op_leh_he.png'
        )
        print("Part 3(b) plots saved.")
    except OSError:
        print("Warning: CSV file for 3(b) not found.")

def run_part_3c():
    """Generates separate Original vs. Variation plots for Part 3(c) CLAHE."""
    images_to_process = ["canyon", "retina"]
    
    for img_name in images_to_process:
        orig_img = plt.imread(f'./data/{img_name}.png')
        if orig_img.dtype in [np.float32, np.float64]:
            orig_img = (orig_img * 255).astype(np.uint8)
            
        H, W = orig_img.shape[:2]
        
        # Define the variations and their corresponding display titles
        variations = [
            ("tuned", "Tuned CLAHE"),
            ("large_win", "Larger Window"),
            ("small_win", "Smaller Window"),
            ("half_thresh", "Half Threshold")
        ]
        
        for var_suffix, var_title in variations:
            csv_path = f'temp/3_c_{img_name}_clahe_{var_suffix}.csv'
            try:
                # Load the specific variation
                var_img = load_and_reshape(csv_path, H, W, 3)
                
                # Plot just the Original vs. This Specific Variation
                images = [orig_img, var_img]
                titles = [f"Original ({img_name})", var_title]
                output_filename = f'output_img/3_c_op_{img_name}_clahe_{var_suffix}.png'
                
                save_plot_with_histograms(images, titles, output_filename)
                print(f"Saved: {output_filename}")
                
            except OSError:
                print(f"Warning: CSV file {csv_path} not found. Skipping plot.")



def run_part_3d():
    """Generates plots for Part 3(d) Histogram Matching."""
    source_img = plt.imread('./data/retina.png')
    ref_img = plt.imread('./data/retinaRef.png')
    
    for img in [source_img, ref_img]:
        if img.dtype in [np.float32, np.float64]:
            img = (img * 255).astype(np.uint8)
            
    H, W = source_img.shape[:2]
    
    try:
        matched_256 = load_and_reshape('temp/3_d_retina_matched_256.csv', H, W, 3)
        save_plot_with_histograms(
            [source_img, ref_img, matched_256], 
            ["Source (retina.png)", "Reference", "Matched (256 Bins)"], 
            'output_img/3_d_op_retina_matched.png'
        )
        print("Part 3(d) plots saved.")
    except OSError:
        print("Warning: CSV files for 3(d) not found.")


if __name__ == "__main__":
    print("Starting Q3 Python Visualization Pipeline...")
    
    # run_part_3a()
    # run_part_3b()
    # run_part_3c()
    run_part_3d()

    print("All Q3 plots saved to 'output_img/'.")