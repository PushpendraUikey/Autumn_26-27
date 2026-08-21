import numpy as np
import matplotlib.pyplot as plt
import math
import os

# Ensure output directory exists
os.makedirs('output_img', exist_ok=True)

# ==========================================
# GENERALIZED PLOTTING & UTILITY FUNCTIONS
# ==========================================

def save_formatted_plot(images, titles, filepath, cmap='gray', colorbar_mode='individual', vmin=None, vmax=None, max_cols=2):
    """
    General helper to plot 1 or more images in a grid, satisfying formatting rules.
    """
    n = len(images)
    cols = min(n, max_cols)
    rows = math.ceil(n / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(6 * cols, 6 * rows))
    
    if isinstance(axes, np.ndarray):
        axes_flat = axes.flatten()
    else:
        axes_flat = [axes]
        
    ims = []
    for i in range(len(axes_flat)):
        ax = axes_flat[i]
        
        if i < n:
            img, title = images[i], titles[i]
            # using cmap='gray' satisfies the 200+ intensity grayscale requirement[cite: 3]
            im = ax.imshow(img, cmap=cmap, aspect='equal', vmin=vmin, vmax=vmax)
            ims.append(im)
            ax.set_title(title)
            
            ax.set_xlabel("Pixel Units (X)")
            ax.set_ylabel("Pixel Units (Y)")
            
            if colorbar_mode == 'individual':
                # The assignment explicitly requires a colorbar alongside grayscale images[cite: 3]
                fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
        else:
            ax.axis('off')
            
    if colorbar_mode == 'shared':
        fig.colorbar(ims[-1], ax=list(axes_flat[:n]), fraction=0.02, pad=0.04)
    else:
        plt.tight_layout()
        
    fig.savefig(filepath, dpi=300, bbox_inches='tight')
    plt.close(fig) 

# ==========================================
# ASSIGNMENT EXECUTIONS
# ==========================================

def plot_thresholding_results(image_filename):
    """Loads CSVs and generates Part 2 plots for a specific image."""
    base_name = image_filename.split('.')[0]
    
    # 1. Load Original Image
    orig_img = plt.imread(f'./data/{image_filename}')
    
    if orig_img.dtype == np.float32 or orig_img.dtype == np.float64:
        orig_img = (orig_img * 255).astype(np.uint8)
        
    if orig_img.ndim == 3: 
        orig_img = orig_img[:, :, 0] # Convert RGB to grayscale
        
    H, W = orig_img.shape

    # 2. Load C++ Generated CSVs
    try:
        man = np.loadtxt(f'temp/{base_name}_manual.csv', delimiter=',').reshape((H, W))
        otsu = np.loadtxt(f'temp/{base_name}_otsu.csv', delimiter=',').reshape((H, W))
        adapt_bin = np.loadtxt(f'temp/{base_name}_adapt_bin.csv', delimiter=',').reshape((H, W))
        adapt_map = np.loadtxt(f'temp/{base_name}_adapt_map.csv', delimiter=',').reshape((H, W))
    except OSError:
        print(f"Warning: CSV files for {base_name} not found. Skipping plot generation.")
        return

    # 3. Generate Part 2(a) Plots (Manual)
    save_formatted_plot([orig_img, man], 
                        ["Original", "Manual Thresholding"], 
                        f'output_img/2_a_op_{base_name}_manual.png')

    # 4. Generate Part 2(b) Plots (Otsu)
    save_formatted_plot([orig_img, otsu], 
                        ["Original", "Otsu Thresholding"], 
                        f'output_img/2_b_op_{base_name}_otsu.png')

    # 5. Generate Part 2(c) Plots (Adaptive + Threshold Map)[cite: 3]
    # We set max_cols=3 so the Original, Thresholded, and Map appear in a single row
    save_formatted_plot([orig_img, adapt_bin, adapt_map], 
                        ["Original", "Adaptive Thresholding", "Per-Pixel Threshold Map"], 
                        f'output_img/2_c_op_{base_name}_adaptive.png', 
                        max_cols=3)

if __name__ == "__main__":
    print("Starting Q2 Python Visualization Pipeline...")
    
    images_to_process = ["receipt.png", "blackboard.png", "lilavati.png", "qr.png"]
    
    for img_file in images_to_process:
        plot_thresholding_results(img_file)
        print(f"Generated plots for {img_file}")
        
    print("All Q2 plots saved to 'output_img/'.")