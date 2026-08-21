import numpy as np
import matplotlib.pyplot as plt


# ==========================================
# PART 1(a): Image Shrinking - Subsampling
# ==========================================
# 1. Load the Original Image Directly
orig_filepath = './data/suit.png'
orig_img = plt.imread(orig_filepath)

# Get original dimensions (ignoring the channel count in shape)
orig_height, orig_width = orig_img.shape[:2]

# Helper function to load CSV and reshape dynamically
def load_and_reshape(filepath, orig_h, orig_w, d, channels=3):
    new_h = orig_h // d
    new_w = orig_w // d
    flat_data = np.loadtxt(filepath, delimiter=',')
    img_3d = flat_data.reshape((new_h, new_w, channels))
    # Cast to uint8 as requested for RGB visualization[cite: 1]
    return np.clip(img_3d, 0, 255).astype(np.uint8)

# 2. Load the C++ Outputs
img_d2 = load_and_reshape('temp/shrunken_d2.csv', orig_height, orig_width, 2)
img_d3 = load_and_reshape('temp/shrunken_d3.csv', orig_height, orig_width, 3)

# 3. Figure 1: Original vs d=2
fig1, axes1 = plt.subplots(1, 2, figsize=(10, 5))
axes1[0].imshow(orig_img, aspect='equal') # aspect='equal' handles the 1:1 requirement[cite: 1]
axes1[0].set_title("Original Image")
axes1[0].set_xlabel("Pixel Units")
axes1[0].set_ylabel("Pixel Units")

axes1[1].imshow(img_d2, aspect='equal')
axes1[1].set_title("Subsampled (d=2)")
axes1[1].set_xlabel("Pixel Units")
axes1[1].set_ylabel("Pixel Units")

plt.tight_layout()
fig1.savefig('output_img/comparison_d2.png', dpi=300)

# 4. Figure 2: Original vs d=3
fig2, axes2 = plt.subplots(1, 2, figsize=(10, 5))
axes2[0].imshow(orig_img, aspect='equal')
axes2[0].set_title("Original Image")
axes2[0].set_xlabel("Pixel Units")
axes2[0].set_ylabel("Pixel Units")

axes2[1].imshow(img_d3, aspect='equal')
axes2[1].set_title("Subsampled (d=3)")
axes2[1].set_xlabel("Pixel Units")
axes2[1].set_ylabel("Pixel Units")

plt.tight_layout()
fig2.savefig('output_img/comparison_d3.png', dpi=300)

# 5. Save the standalone subsampled images
plt.imsave('output_img/suit_d2.png', img_d2)
plt.imsave('output_img/suit_d3.png', img_d3)

# Show both windows
plt.show()



# ==========================================
# PART 1(b): Nearest-Neighbor Interpolation
# ==========================================

# Load original random.png
# We load it using Matplotlib and grab the first channel in case it loads as RGB
orig_random = plt.imread('./data/random.png')
if orig_random.ndim == 3:
    orig_random = orig_random[:, :, 0] 

M, N = orig_random.shape

# Calculate expected dimensions based on the formula[cite: 1]
outM = 300 * (M - 1) + 1
outN = 300 * (N - 1) + 1

# Load the resized C++ output CSV (it is single channel, so no need to reshape for RGB)
flat_nn_data = np.loadtxt('temp/resized_nn.csv', delimiter=',')
img_nn = flat_nn_data.reshape((outM, outN))

# Create the plot side-by-side
fig_nn, axes_nn = plt.subplots(1, 2, figsize=(12, 5))

# Plot Original with "jet" colormap[cite: 1]
im0 = axes_nn[0].imshow(orig_random, cmap='jet', aspect='equal')
axes_nn[0].set_title("Original Image (random.png)")
axes_nn[0].set_xlabel("Pixel Units (X)")
axes_nn[0].set_ylabel("Pixel Units (Y)")
fig_nn.colorbar(im0, ax=axes_nn[0], fraction=0.046, pad=0.04)

# Plot Resized with "jet" colormap[cite: 1]
im1 = axes_nn[1].imshow(img_nn, cmap='jet', aspect='equal')
axes_nn[1].set_title("Nearest-Neighbor Enlargement")
axes_nn[1].set_xlabel("Pixel Units (X)")
axes_nn[1].set_ylabel("Pixel Units (Y)")
fig_nn.colorbar(im1, ax=axes_nn[1], fraction=0.046, pad=0.04)

plt.tight_layout()
fig_nn.savefig('output_img/comparison_nn.png', dpi=300)

# 5. Save the standalone Nearest-Neighbor image (fully formatted)
fig_nn_alone, ax_nn_alone = plt.subplots(figsize=(7, 6))

im_nn_alone = ax_nn_alone.imshow(img_nn, cmap='jet', aspect='equal')
ax_nn_alone.set_title("Nearest-Neighbor Enlargement")
ax_nn_alone.set_xlabel("Pixel Units (X)")
ax_nn_alone.set_ylabel("Pixel Units (Y)")

# Add the required colorbar[cite: 1]
fig_nn_alone.colorbar(im_nn_alone, ax=ax_nn_alone, fraction=0.046, pad=0.04)

plt.tight_layout()
fig_nn_alone.savefig('output_img/random_nn_standalone.png', dpi=300)

# ==========================================
# PART 1(c) & 1(d): Bilinear and Bicubic
# ==========================================

# Load the C++ CSV outputs
flat_bilinear = np.loadtxt('temp/resized_bilinear.csv', delimiter=',')
img_bilinear = flat_bilinear.reshape((outM, outN))

flat_bicubic = np.loadtxt('temp/resized_bicubic.csv', delimiter=',')
img_bicubic = flat_bicubic.reshape((outM, outN))

# ------------------------------------------
# 1(c): Bilinear Interpolation Plots
# ------------------------------------------

# Comparison Plot
fig_bi_comp, axes_bi = plt.subplots(1, 2, figsize=(12, 5))
im_bi0 = axes_bi[0].imshow(orig_random, cmap='jet', aspect='equal')
axes_bi[0].set_title("Original Image (random.png)")
axes_bi[0].set_xlabel("Pixel Units (X)")
axes_bi[0].set_ylabel("Pixel Units (Y)")
fig_bi_comp.colorbar(im_bi0, ax=axes_bi[0], fraction=0.046, pad=0.04)

im_bi1 = axes_bi[1].imshow(img_bilinear, cmap='jet', aspect='equal')
axes_bi[1].set_title("Bilinear Interpolation")
axes_bi[1].set_xlabel("Pixel Units (X)")
axes_bi[1].set_ylabel("Pixel Units (Y)")
fig_bi_comp.colorbar(im_bi1, ax=axes_bi[1], fraction=0.046, pad=0.04)

plt.tight_layout()
fig_bi_comp.savefig('output_img/comparison_bilinear.png', dpi=300)

# Standalone Plot
fig_bi, ax_bi = plt.subplots(figsize=(7, 6))
im_bi = ax_bi.imshow(img_bilinear, cmap='jet', aspect='equal')
ax_bi.set_title("Bilinear Interpolation (Standalone)")
ax_bi.set_xlabel("Pixel Units (X)")
ax_bi.set_ylabel("Pixel Units (Y)")
fig_bi.colorbar(im_bi, ax=ax_bi, fraction=0.046, pad=0.04)

plt.tight_layout()
fig_bi.savefig('output_img/random_bilinear_standalone.png', dpi=300)

# ------------------------------------------
# 1(d): Bicubic Interpolation Plots
# ------------------------------------------

# Comparison Plot
fig_bic_comp, axes_bic = plt.subplots(1, 2, figsize=(12, 5))
im_bic0 = axes_bic[0].imshow(orig_random, cmap='jet', aspect='equal')
axes_bic[0].set_title("Original Image (random.png)")
axes_bic[0].set_xlabel("Pixel Units (X)")
axes_bic[0].set_ylabel("Pixel Units (Y)")
fig_bic_comp.colorbar(im_bic0, ax=axes_bic[0], fraction=0.046, pad=0.04)

im_bic1 = axes_bic[1].imshow(img_bicubic, cmap='jet', aspect='equal')
axes_bic[1].set_title("Bicubic Interpolation")
axes_bic[1].set_xlabel("Pixel Units (X)")
axes_bic[1].set_ylabel("Pixel Units (Y)")
fig_bic_comp.colorbar(im_bic1, ax=axes_bic[1], fraction=0.046, pad=0.04)

plt.tight_layout()
fig_bic_comp.savefig('output_img/comparison_bicubic.png', dpi=300)

# Standalone Plot
fig_bic, ax_bic = plt.subplots(figsize=(7, 6))
im_bic = ax_bic.imshow(img_bicubic, cmap='jet', aspect='equal')
ax_bic.set_title("Bicubic Interpolation (Standalone)")
ax_bic.set_xlabel("Pixel Units (X)")
ax_bic.set_ylabel("Pixel Units (Y)")
fig_bic.colorbar(im_bic, ax=ax_bic, fraction=0.046, pad=0.04)

plt.tight_layout()
fig_bic.savefig('output_img/random_bicubic_standalone.png', dpi=300)

# Render all the interactive windows at the very end
plt.show()


# ==========================================
# PART 1(e): Image Rotation
# ==========================================

# 1. Load original main image directly
orig_main = plt.imread('./data/main.png')

# Note: Matplotlib sometimes loads .png files as normalized floats [0, 1]. 
# We scale it to [0, 255] for visual consistency with our C++ RGB outputs.
if orig_main.dtype == np.float32 or orig_main.dtype == np.float64:
    orig_main_display = (orig_main * 255).astype(np.uint8)
else:
    orig_main_display = orig_main

H_m, W_m = orig_main_display.shape[:2]

# 2. Load the rotated C++ outputs
flat_rot_nn = np.loadtxt('temp/rotated_nn.csv', delimiter=',')
img_rot_nn = np.clip(flat_rot_nn.reshape((H_m, W_m, 3)), 0, 255).astype(np.uint8)

flat_rot_bi = np.loadtxt('temp/rotated_bilinear.csv', delimiter=',')
img_rot_bi = np.clip(flat_rot_bi.reshape((H_m, W_m, 3)), 0, 255).astype(np.uint8)

# 3. Comparison Plot (Original, NN, Bilinear)[cite: 1]
fig_rot, axes_rot = plt.subplots(1, 3, figsize=(18, 6))

axes_rot[0].imshow(orig_main_display, aspect='equal')
axes_rot[0].set_title("Original (Slanted)")
axes_rot[0].set_xlabel("Pixel Units (X)")
axes_rot[0].set_ylabel("Pixel Units (Y)")

axes_rot[1].imshow(img_rot_nn, aspect='equal')
axes_rot[1].set_title("Rotated (Nearest-Neighbor)")
axes_rot[1].set_xlabel("Pixel Units (X)")
axes_rot[1].set_ylabel("Pixel Units (Y)")

axes_rot[2].imshow(img_rot_bi, aspect='equal')
axes_rot[2].set_title("Rotated (Bilinear)")
axes_rot[2].set_xlabel("Pixel Units (X)")
axes_rot[2].set_ylabel("Pixel Units (Y)")

plt.tight_layout()
fig_rot.savefig('output_img/main_comparison_rotation.png', dpi=300)

# 4. Standalone Plots
fig_rn, ax_rn = plt.subplots(figsize=(8, 6))
ax_rn.imshow(img_rot_nn, aspect='equal')
ax_rn.set_title("Rotated (Nearest-Neighbor)")
ax_rn.set_xlabel("Pixel Units (X)")
ax_rn.set_ylabel("Pixel Units (Y)")
fig_rn.savefig('output_img/rotated_nn_standalone.png', dpi=300)

fig_rb, ax_rb = plt.subplots(figsize=(8, 6))
ax_rb.imshow(img_rot_bi, aspect='equal')
ax_rb.set_title("Rotated (Bilinear)")
ax_rb.set_xlabel("Pixel Units (X)")
ax_rb.set_ylabel("Pixel Units (Y)")
fig_rb.savefig('output_img/rotated_bilinear_standalone.png', dpi=300)

# Make sure this remains the absolute last line in your script!
plt.show()


def compute_rmse(img1, img2):
    return np.sqrt(np.mean((img1 - img2)**2))

# Load data
orig = np.loadtxt('temp/ct_original.csv', delimiter=',')
nn   = np.loadtxt('temp/enlarged_nn.csv', delimiter=',')
bi   = np.loadtxt('temp/enlarged_bi.csv', delimiter=',')
bic  = np.loadtxt('temp/enlarged_bic.csv', delimiter=',')

# 1. Report RMSE[cite: 1]
print(f"RMSE Nearest-Neighbor: {compute_rmse(orig, nn):.4f}")
print(f"RMSE Bilinear:         {compute_rmse(orig, bi):.4f}")
print(f"RMSE Bicubic:          {compute_rmse(orig, bic):.4f}")

# Calculate shared colormap limits[cite: 1]
vmin_img, vmax_img = orig.min(), orig.max()

diff_nn = np.abs(orig - nn)
diff_bi = np.abs(orig - bi)
diff_bic = np.abs(orig - bic)
vmin_diff, vmax_diff = 0, max(diff_nn.max(), diff_bi.max(), diff_bic.max())

# 2. Plot Enlarged Images[cite: 1]
fig_enl, axes_enl = plt.subplots(1, 4, figsize=(16, 4))
titles = ["Original", "Nearest Neighbor", "Bilinear", "Bicubic"]
images = [orig, nn, bi, bic]

for ax, img, title in zip(axes_enl, images, titles):
    im = ax.imshow(img, cmap='jet', vmin=vmin_img, vmax=vmax_img)
    ax.set_title(title)
fig_enl.colorbar(im, ax=axes_enl.ravel().tolist(), fraction=0.02, pad=0.04)
fig_enl.savefig('output_img/ct_enlarged_comparison.png', dpi=300)

# 3. Plot Difference Images[cite: 1]
fig_diff, axes_diff = plt.subplots(1, 3, figsize=(12, 4))
diff_titles = ["Diff: NN", "Diff: Bilinear", "Diff: Bicubic"]
diffs = [diff_nn, diff_bi, diff_bic]

for ax, diff, title in zip(axes_diff, diffs, diff_titles):
    im = ax.imshow(diff, cmap='jet', vmin=vmin_diff, vmax=vmax_diff)
    ax.set_title(title)
fig_diff.colorbar(im, ax=axes_diff.ravel().tolist(), fraction=0.02, pad=0.04)
fig_diff.savefig('output_img/ct_difference_comparison.png', dpi=300)

plt.show()