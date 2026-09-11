"""
Edge Detection Comparison: Sobel vs. Marr-Hildreth (LoG) vs. Canny
====================================================================

Common definition of "edge" used by ALL THREE methods below
(as given in CS-663 DIP slides "Detecting Edges and Corners"):

    An EDGE is a location of SHARP CHANGE in image intensity
    (or color) over space. Such changes are typically caused by
    discontinuities in depth, discontinuities in surface
    orientation, changes in material properties, or variation in
    scene illumination.

The three methods below differ only in HOW they operationalize
"sharp change":
  * Sobel      -> thresholding the (smoothed) 1st-derivative
                  (gradient) magnitude.
  * Marr-Hildreth -> finding zero-crossings of the 2nd derivative
                  (Laplacian-of-Gaussian), i.e., locations where
                  the 1st derivative (slope of intensity) is
                  locally extremal.
  * Canny      -> thinning the 1st-derivative gradient-magnitude
                  ridge (non-maximum suppression) and keeping only
                  those ridges that are "strong enough" using a
                  two-threshold hysteresis criterion.

Each function below is heavily commented to explain the
higher-level operation being performed at each step, mirroring
the algorithm descriptions from the lecture slides.
"""

import os
import glob

import numpy as np
from scipy import ndimage
import matplotlib.pyplot as plt
from skimage import io, color, img_as_float


# ---------------------------------------------------------------
# 1) SOBEL EDGE DETECTOR
# ---------------------------------------------------------------
def sobel_edge_detection(image, threshold_ratio=0.2):
    """
    Sobel edge detector.

    Edge definition used here: a pixel is "on an edge" if the
    (smoothed) gradient magnitude at that pixel exceeds a
    threshold (set as a fraction of the max gradient magnitude
    in the image).

    Algorithm
    ---------
    1) Convolve the image with the Sobel Gx and Gy kernels. These
       kernels simultaneously (a) differentiate in one direction
       and (b) smooth in the orthogonal direction, giving
       "smoothed gradients" (as opposed to a raw finite
       difference).
    2) Combine Gx and Gy into a single gradient-magnitude image.
    3) Threshold the gradient-magnitude image to obtain a binary
       edge map.

    Parameters
    ----------
    image : 2D float ndarray (grayscale image)
    threshold_ratio : float
        Threshold on gradient magnitude, expressed as a fraction
        of the image's maximum gradient magnitude.

    Returns
    -------
    edges : 2D bool ndarray  (binary edge map)
    G     : 2D float ndarray (gradient-magnitude image, useful
            for visualizing the "raw" Sobel response before
            thresholding)
    """
    # Sobel kernels, exactly as given in the slides:
    #   Gx = [[-1,0,1],[-2,0,2],[-1,0,1]] * A
    #   Gy = [[-1,-2,-1],[0,0,0],[1,2,1]] * A
    Gx_kernel = np.array([[-1, 0, 1],
                           [-2, 0, 2],
                           [-1, 0, 1]], dtype=float)
    Gy_kernel = np.array([[-1, -2, -1],
                           [0, 0, 0],
                           [1, 2, 1]], dtype=float)

    # --- Step 1: compute smoothed horizontal & vertical gradients ---
    Gx = ndimage.convolve(image, Gx_kernel, mode='reflect')
    Gy = ndimage.convolve(image, Gy_kernel, mode='reflect')

    # --- Step 2: gradient magnitude at every pixel ---
    G = np.sqrt(Gx ** 2 + Gy ** 2)

    # --- Step 3: threshold gradient magnitude to detect edges ---
    threshold = threshold_ratio * G.max()
    edges = G > threshold

    return edges, G


# ---------------------------------------------------------------
# 2) MARR-HILDRETH (LoG) EDGE DETECTOR
# ---------------------------------------------------------------
def marr_hildreth_edge_detection(image, sigma=2.0, zero_cross_threshold=0.0):
    """
    Marr-Hildreth edge detector.

    Edge definition used here: a pixel is "on an edge" if the
    LoG-filtered image has a ZERO CROSSING at that pixel, i.e.,
    two opposite neighbors (left/right, up/down, or along either
    diagonal) have DIFFERENT SIGNS and the MAGNITUDE of their
    difference exceeds a threshold. This corresponds to a local
    extremum in the 1st derivative of intensity (the point of
    steepest slope on the intensity ramp of an edge).

    Algorithm
    ---------
    1) Convolve the image with a Laplacian-of-Gaussian (LoG)
       filter -- i.e. Gaussian-smooth the image (to suppress
       noise) and then apply the Laplacian (2nd derivative)
       operator.
    2) Detect zero crossings: for every pixel, compare the signs
       of its opposite neighbor pairs (L-R, U-D, NW-SE, NE-SW).
       If any pair has opposite signs AND the magnitude of their
       difference exceeds a threshold, mark the pixel as an edge.

    Parameters
    ----------
    image : 2D float ndarray (grayscale image)
    sigma : float
        Standard deviation of the Gaussian used inside the LoG
        filter. Larger sigma -> coarser / smoother edges (as
        shown in the slides' progression g1 < g2 < g3).
    zero_cross_threshold : float
        Minimum magnitude of the sign-changing difference for a
        zero crossing to be accepted as an edge (slide's
        "threshold" parameter; 0 reproduces the noisy result
        shown in the slides).

    Returns
    -------
    edges  : 2D bool ndarray  (binary edge map)
    log_img: 2D float ndarray (the LoG-filtered image, useful for
             visualization)
    """
    # --- Step 1: Laplacian-of-Gaussian = Gaussian smoothing + Laplacian ---
    log_img = ndimage.gaussian_laplace(image, sigma=sigma)

    # --- Step 2: zero-crossing detection ---
    # Pad by 1 pixel so we can look at all 8-neighbors of every
    # pixel (including border pixels) without special-casing.
    padded = np.pad(log_img, pad_width=1, mode='reflect')

    left = padded[1:-1, 0:-2]
    right = padded[1:-1, 2:]
    up = padded[0:-2, 1:-1]
    down = padded[2:, 1:-1]
    nw = padded[0:-2, 0:-2]
    se = padded[2:, 2:]
    ne = padded[0:-2, 2:]
    sw = padded[2:, 0:-2]

    def opposite_pair_zero_crossing(a, b):
        # A zero crossing is declared between opposite neighbors
        # a and b when they have different signs AND the
        # magnitude of their difference exceeds the threshold.
        different_signs = np.sign(a) != np.sign(b)
        big_enough = np.abs(a - b) > zero_cross_threshold
        return different_signs & big_enough

    edges = (
        opposite_pair_zero_crossing(left, right) |   # left vs right
        opposite_pair_zero_crossing(up, down) |       # up vs down
        opposite_pair_zero_crossing(nw, se) |          # NW vs SE
        opposite_pair_zero_crossing(ne, sw)            # NE vs SW
    )

    return edges, log_img


# ---------------------------------------------------------------
# 3) CANNY EDGE DETECTOR
# ---------------------------------------------------------------
def canny_edge_detection(image, sigma=1.0,
                          low_threshold_ratio=0.1,
                          high_threshold_ratio=0.2):
    """
    Canny edge detector (does not need 2nd derivatives).

    Edge definition used here: an edge pixel is a LOCAL MAXIMUM
    of gradient magnitude ALONG THE GRADIENT DIRECTION (a "thin
    ridge") that is either strong on its own (magnitude above
    thresholdHigh) or connected, via a chain of other
    above-thresholdLow ridge pixels, to such a strong pixel
    (hysteresis).

    Algorithm
    ---------
    1) Gaussian smoothing to reduce noise.
    2) Compute gradient magnitude G and orientation Theta at
       every pixel (using Sobel-like derivatives).
    3) Edge thinning by non-maximum suppression: keep a pixel
       only if its gradient magnitude is >= the magnitude of its
       two neighbors along the (interpolated/quantized) gradient
       direction; otherwise suppress it.
    4) Edge tracing via hysteresis thresholding: pixels above
       thresholdHigh are surely edges; pixels above thresholdLow
       are edges only if they are connected (via a path of
       above-thresholdLow pixels) to a pixel above thresholdHigh.

    Parameters
    ----------
    image : 2D float ndarray (grayscale image)
    sigma : float
        Standard deviation of the Gaussian used for step 1.
    low_threshold_ratio, high_threshold_ratio : float
        thresholdLow / thresholdHigh, expressed as fractions of
        the max non-max-suppressed gradient magnitude.

    Returns
    -------
    edges : 2D bool ndarray  (binary edge map)
    nms   : 2D float ndarray (gradient magnitude after
            non-maximum suppression, before hysteresis)
    """
    # --- Step 1: Gaussian smoothing to reduce noise ---
    smoothed = ndimage.gaussian_filter(image, sigma=sigma)

    # --- Step 2: gradient magnitude G and orientation Theta ---
    Gx = ndimage.sobel(smoothed, axis=1, mode='reflect')  # d/dx
    Gy = ndimage.sobel(smoothed, axis=0, mode='reflect')  # d/dy
    G = np.hypot(Gx, Gy)
    theta_deg = np.degrees(np.arctan2(Gy, Gx))
    theta_deg[theta_deg < 0] += 180.0  # fold to [0, 180): +/- gradient equivalent

    # --- Step 3: non-maximum suppression (edge thinning) ---
    # For each pixel, compare its gradient magnitude to the two
    # neighbors lying along its gradient direction, quantized to
    # the nearest of 4 directions (0, 45, 90, 135 degrees).
    nms = np.zeros_like(G)
    rows, cols = G.shape

    # Direction masks (vectorized instead of a per-pixel python loop)
    angle = theta_deg
    dir0 = ((angle >= 0) & (angle < 22.5)) | ((angle >= 157.5) & (angle <= 180))
    dir45 = (angle >= 22.5) & (angle < 67.5)
    dir90 = (angle >= 67.5) & (angle < 112.5)
    dir135 = (angle >= 112.5) & (angle < 157.5)

    # Shifted magnitude images for each of the 4 candidate directions
    G_pad = np.pad(G, 1, mode='constant', constant_values=0)
    n_left = G_pad[1:-1, 0:-2]
    n_right = G_pad[1:-1, 2:]
    n_up = G_pad[0:-2, 1:-1]
    n_down = G_pad[2:, 1:-1]
    n_nw = G_pad[0:-2, 0:-2]
    n_se = G_pad[2:, 2:]
    n_ne = G_pad[0:-2, 2:]
    n_sw = G_pad[2:, 0:-2]

    is_local_max = np.zeros_like(G, dtype=bool)
    is_local_max |= dir0 & (G >= n_left) & (G >= n_right)
    is_local_max |= dir45 & (G >= n_ne) & (G >= n_sw)
    is_local_max |= dir90 & (G >= n_up) & (G >= n_down)
    is_local_max |= dir135 & (G >= n_nw) & (G >= n_se)

    nms[is_local_max] = G[is_local_max]

    # --- Step 4: hysteresis thresholding (edge tracing) ---
    high_threshold = high_threshold_ratio * nms.max()
    low_threshold = low_threshold_ratio * nms.max()

    strong = nms >= high_threshold
    weak = (nms >= low_threshold) & (nms < high_threshold)

    # Find connected components among (strong OR weak) pixels.
    # Any component that contains at least one "strong" pixel has
    # ALL of its pixels (strong and weak) promoted to edges -- this
    # implements "trace connected paths from a strong pixel through
    # weak pixels".
    candidate_mask = strong | weak
    labeled, _ = ndimage.label(candidate_mask, structure=np.ones((3, 3)))
    strong_component_labels = set(labeled[strong].tolist()) - {0}
    edges = np.isin(labeled, list(strong_component_labels))

    return edges, nms


# ---------------------------------------------------------------
# MAIN: load every image in ./data/, run all 3 detectors, plot
# side by side, and save the comparison figure to ./output/
# ---------------------------------------------------------------
def process_and_visualize(image_path, output_dir):
    """Run all three edge detectors on one image and save a
    side-by-side comparison figure."""

    # Load and prepare a single-channel float image in [0, 1]
    img = io.imread(image_path)
    if img.ndim == 3:
        img = color.rgb2gray(img)  # drop alpha/color -> grayscale
    img = img_as_float(img)

    # Run the three edge detectors
    sobel_edges, _ = sobel_edge_detection(img, threshold_ratio=0.15)
    log_edges, _ = marr_hildreth_edge_detection(img, sigma=2.0,
                                                 zero_cross_threshold=0.0)
    canny_edges, _ = canny_edge_detection(img, sigma=1.0,
                                           low_threshold_ratio=0.1,
                                           high_threshold_ratio=0.2)

    # Plot original + 3 edge maps side by side
    fig, axes = plt.subplots(1, 4, figsize=(20, 5))
    panels = [
        (img, 'Original', 'gray'),
        (sobel_edges, 'Sobel', 'gray'),
        (log_edges, 'Marr-Hildreth (LoG)', 'gray'),
        (canny_edges, 'Canny', 'gray'),
    ]
    for ax, (data, title, cmap) in zip(axes, panels):
        ax.imshow(data, cmap=cmap)
        ax.set_title(title)
        ax.axis('off')

    plt.tight_layout()

    basename = os.path.splitext(os.path.basename(image_path))[0]
    out_path = os.path.join(output_dir, f"{basename}_edges_comparison.png")
    plt.savefig(out_path, dpi=150)
    plt.close(fig)
    print(f"Saved: {out_path}")


def main():
    data_dir = "./data"
    output_dir = "./output"
    os.makedirs(output_dir, exist_ok=True)

    image_extensions = ("*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tif", "*.tiff")
    image_paths = []
    for ext in image_extensions:
        image_paths.extend(glob.glob(os.path.join(data_dir, ext)))

    if not image_paths:
        print(f"No images found in {data_dir}. "
              f"Place some images there and re-run.")
        return

    for path in sorted(image_paths):
        process_and_visualize(path, output_dir)


if __name__ == "__main__":
    main()