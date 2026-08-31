import numpy as np
import matplotlib.pyplot as plt
import os

os.makedirs('output_img', exist_ok=True)

# ==========================================
# GENERALIZED PLOTTING & UTILITY FUNCTIONS
# ==========================================

def save_formatted_plot(images, titles, filepath, cmap=None, colorbar_mode=None, vmin=None, vmax=None):
    """
    General helper to plot 1 or more images side-by-side, satisfying formatting rules.
    colorbar_mode can be 'individual', 'shared', or None.
    """
    n = len(images)
    # Adjust figure size dynamically based on number of subplots
    fig, axes = plt.subplots(1, n, figsize=(5 * n, 5))
    if n == 1: 
        axes = [axes]
    
    ims = []
    for ax, img, title in zip(axes, images, titles):
        # aspect='equal' enforces the 1:1 pixel aspect ratio requirement
        im = ax.imshow(img, cmap=cmap, aspect='equal', vmin=vmin, vmax=vmax)
        ims.append(im)
        ax.set_title(title)
        
        # Explicitly display pixel units along each axis
        ax.set_xlabel("Pixel Units (X)")
        ax.set_ylabel("Pixel Units (Y)")
        
        if colorbar_mode == 'individual':
            fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
            
    if colorbar_mode == 'shared':
        fig.colorbar(ims[-1], ax=list(axes), fraction=0.02, pad=0.04)
    else:
        # Only call tight_layout if we aren't spanning a shared colorbar
        plt.tight_layout()
        
    # bbox_inches='tight' trims the excess white space perfectly during the save
    fig.savefig(filepath, dpi=300, bbox_inches='tight')
    plt.close(fig) # Prevent memory leaks and suppress interactive display

def load_and_reshape(filepath, orig_h, orig_w, d, channels=3):
    new_h = orig_h // d
    new_w = orig_w // d
    flat_data = np.loadtxt(filepath, delimiter=',')
    img_3d = flat_data.reshape((new_h, new_w, channels))
    
    return np.clip(img_3d, 0, 255).astype(np.uint8)

def compute_rmse(img1, img2):
    return np.sqrt(np.mean((img1 - img2)**2))

# ==========================================
# ASSIGNMENT QUESTION-SPECIFIC PLOTTING FUNCTIONS
# ==========================================

def run_part_1a():
    orig_img = plt.imread('./data/suit.png')
    orig_h, orig_w = orig_img.shape[:2]

    img_d2 = load_and_reshape('temp/1_a_shrunken_d2.csv', orig_h, orig_w, 2)
    img_d3 = load_and_reshape('temp/1_a_shrunken_d3.csv', orig_h, orig_w, 3)

    # Comparisons
    save_formatted_plot([orig_img, img_d2], ["Original Image", "Subsampled (d=2)"], 'output_img/1_a_op_comparison_d2.png')
    save_formatted_plot([orig_img, img_d3], ["Original Image", "Subsampled (d=3)"], 'output_img/1_a_op_comparison_d3.png')

    # Standalone (Using the helper to ensure pixel axes are drawn)
    save_formatted_plot([img_d2], ["Subsampled (d=2)"], 'output_img/1_a_op_suit_d2_standalone.png')
    save_formatted_plot([img_d3], ["Subsampled (d=3)"], 'output_img/1_a_op_suit_d3_standalone.png')


def run_part_1b_1c_1d():
    orig_random = plt.imread('./data/random.png')
    if orig_random.ndim == 3: 
        orig_random = orig_random[:, :, 0] 
        
    M, N = orig_random.shape
    outM, outN = 300 * (M - 1) + 1, 300 * (N - 1) + 1

    try:
        img_nn = np.loadtxt('temp/1_b_resized_nn.csv', delimiter=',').reshape((outM, outN))
        img_bi = np.loadtxt('temp/1_c_resized_bilinear.csv', delimiter=',').reshape((outM, outN))
        img_bic = np.loadtxt('temp/1_d_resized_bicubic.csv', delimiter=',').reshape((outM, outN))
    except OSError as e:
        print(f"Warning: CSV file not found. {e}")
        return

    # 1(b) Nearest-Neighbor Plots
    save_formatted_plot([orig_random, img_nn], ["Original (random.png)", "Nearest-Neighbor Enlargement"], 
                        'output_img/1_b_op_comparison_nn.png', cmap='jet', colorbar_mode='individual')
    save_formatted_plot([img_nn], ["Nearest-Neighbor Enlargement"], 
                        'output_img/1_b_op_random_nn_standalone.png', cmap='jet', colorbar_mode='individual')

    # 1(c) Bilinear Plots
    save_formatted_plot([orig_random, img_bi], ["Original", "Bilinear Interpolation"], 
                        'output_img/1_c_op_comparison_bilinear.png', cmap='jet', colorbar_mode='individual')
    save_formatted_plot([img_bi], ["Bilinear Interpolation"], 
                        'output_img/1_c_op_random_bilinear_standalone.png', cmap='jet', colorbar_mode='individual')

    # 1(d) Bicubic Plots
    save_formatted_plot([orig_random, img_bic], ["Original", "Bicubic Interpolation"], 
                        'output_img/1_d_op_comparison_bicubic.png', cmap='jet', colorbar_mode='individual')
    save_formatted_plot([img_bic], ["Bicubic Interpolation"], 
                        'output_img/1_d_op_random_bicubic_standalone.png', cmap='jet', colorbar_mode='individual')


def run_part_1e():
    orig_main = plt.imread('./data/main.png')
    if orig_main.dtype in [np.float32, np.float64]:
        orig_main = (orig_main * 255).astype(np.uint8)
    H_m, W_m = orig_main.shape[:2]

    img_rot_nn = np.clip(np.loadtxt('temp/1_e_rotated_nn.csv', delimiter=',')
                 .reshape((H_m, W_m, 3)), 0, 255).astype(np.uint8)
    img_rot_bi = np.clip(np.loadtxt('temp/1_e_rotated_bilinear.csv', delimiter=',')
                 .reshape((H_m, W_m, 3)), 0, 255).astype(np.uint8)

    save_formatted_plot([orig_main, img_rot_nn, img_rot_bi], 
                        ["Original (Slanted)", "Rotated (NN)", "Rotated (Bilinear)"], 
                        'output_img/1_e_op_main_comparison_rotation.png')
    
    save_formatted_plot([img_rot_nn], ["Rotated (NN)"], 
                        'output_img/1_e_op_rotated_nn_standalone.png')
    save_formatted_plot([img_rot_bi], ["Rotated (Bilinear)"], 
                        'output_img/1_e_op_rotated_bilinear_standalone.png')


def run_part_1f():
    orig = np.loadtxt('temp/1_f_ct_original.csv', delimiter=',')
    nn   = np.loadtxt('temp/1_f_enlarged_nn.csv', delimiter=',')
    bi   = np.loadtxt('temp/1_f_enlarged_bi.csv', delimiter=',')
    bic  = np.loadtxt('temp/1_f_enlarged_bic.csv', delimiter=',')

    # RMSE
    print("\n--- Part 1(f) RMSE Reports ---")
    print(f"Nearest-Neighbor: {compute_rmse(orig, nn):.4f}")
    print(f"Bilinear:         {compute_rmse(orig, bi):.4f}")
    print(f"Bicubic:          {compute_rmse(orig, bic):.4f}\n")

    # identical limits for visual inference
    v_min, v_max = orig.min(), orig.max()
    diff_nn, diff_bi, diff_bic = np.abs(orig - nn), np.abs(orig - bi), np.abs(orig - bic)
    d_min, d_max = 0, max(diff_nn.max(), diff_bi.max(), diff_bic.max())

    # Plot Enlarged
    save_formatted_plot([orig, nn, bi, bic], 
                        ["Original", "Nearest Neighbor", "Bilinear", "Bicubic"], 
                        'output_img/1_f_op_ct_enlarged_comparison.png', 
                        cmap='jet', colorbar_mode='shared', vmin=v_min, vmax=v_max)

    # Plot Differences
    save_formatted_plot([diff_nn, diff_bi, diff_bic], 
                        ["Diff: NN", "Diff: Bilinear", "Diff: Bicubic"], 
                        'output_img/1_f_op_ct_difference_comparison.png', 
                        cmap='jet', colorbar_mode='shared', vmin=d_min, vmax=d_max)


# ==========================================
# MAIN EXECUTION 
# ==========================================
if __name__ == "__main__":
    print("Starting Python Visualization Pipeline...")
    run_part_1a()
    run_part_1b_1c_1d()
    run_part_1e()
    run_part_1f()
    print("All plots generated and saved to 'output_img/'.")