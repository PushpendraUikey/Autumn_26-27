"""
Moravec Corner Detection
=========================

Definition of "corner" used here (from CS-663 DIP slides,
"Corner Detection: Moravec"):

    A pixel location "p" is a CORNER when there is LOW SIMILARITY
    (i.e., HIGH DISSIMILARITY) between:
        (1) the image patch centered at "p", and
        (2) the image patch around EVERY neighboring location "q"
            of "p" (8 principal shift directions at 45-degree
            increments: left, right, up, down, NW, NE, SW, SE).

    Intuitively: a FLAT region looks the same after any shift
    (highly similar in every direction -> not a corner). An EDGE
    looks the same when shifted *along* the edge (similar in that
    one direction -> not a corner). A CORNER looks different when
    shifted in *any* direction (dissimilar in every direction ->
    corner).

Dissimilarity is measured via Sum of Squared Differences (SSD)
between corresponding patches, exactly as in the slides. Because
a genuine corner must be dissimilar in EVERY direction, the
corner-strength ("cornerness") at a pixel is taken as the SMALLEST
SSD across the 8 directions -- i.e., even the best-matching /
least-changing direction still shows a large difference. (This is
equivalent to the slides' phrasing of picking the *highest
similarity* among the 8 neighbors and assigning that value to the
pixel -- lowest SSD == highest similarity.)

Algorithm (3 steps, exactly following the slides)
--------------------------------------------------
1) At each pixel "p", compute the SSD between the patch at "p" and
   the patch around each of its 8 neighbors "q". The pixel's raw
   "cornerness" value is the MINIMUM SSD across the 8 directions
   (the direction of least change).
2) Non-maximum suppression: a pixel is discarded (its cornerness
   is set to 0) if ANY of its 8 image-neighbors has a HIGHER
   cornerness value, since that neighbor is then a better corner
   candidate.
3) Threshold the (non-max-suppressed) cornerness map to obtain the
   final set of detected corners.

Known limitation (also noted in the slides): Moravec only checks
8 shift directions at 45-degree increments using RECTANGULAR
(non-isotropic) patches, so its response is not rotation
invariant -- this is exactly the limitation that later detectors
such as Harris-Stephens were designed to fix.
"""

import os
import glob

import numpy as np
from scipy import ndimage
import matplotlib.pyplot as plt
from skimage import io, color, img_as_float


# ---------------------------------------------------------------
# MORAVEC CORNER DETECTOR
# ---------------------------------------------------------------
def moravec_cornerness(image, patch_radius=1, shift=1):
    """
    Step 1: compute the raw Moravec cornerness map.

    For every pixel "p", compute the SSD between the patch
    centered at "p" and the patch centered at "p" shifted by each
    of the 8 principal directions (at 45-degree increments):
    left, right, up, down, NW, NE, SW, SE. The cornerness at "p"
    is the MINIMUM of these 8 SSD values (the least amount of
    change, over the best-matching direction).

    Parameters
    ----------
    image : 2D float ndarray (grayscale image)
    patch_radius : int
        Patches are (2*patch_radius + 1) x (2*patch_radius + 1).
        The slides use a 3x3 neighborhood, i.e. patch_radius=1.
    shift : int
        Number of pixels to shift the patch in each of the 8
        directions when computing dissimilarity.

    Returns
    -------
    cornerness : 2D float ndarray
        Higher values indicate stronger corner response.
    """
    # The 8 principal shift directions at 45-degree increments,
    # exactly as listed in the slides.
    directions = [
        (0,  shift),    # right
        (0, -shift),    # left
        (shift,  0),    # down
        (-shift, 0),    # up
        (shift,  shift),    # SE
        (-shift, -shift),   # NW
        (shift,  -shift),   # SW
        (-shift, shift),    # NE
    ]

    patch_size = 2 * patch_radius + 1
    box_kernel = np.ones((patch_size, patch_size), dtype=float)

    cornerness = None
    for dy, dx in directions:
        # Shift the image by (dy, dx) -- this simulates comparing
        # the patch at "p" to the patch at neighbor "q" = p+(dy,dx).
        shifted = ndimage.shift(image, shift=(dy, dx), mode='reflect', order=0)

        # Per-pixel squared intensity difference at this shift.
        squared_diff = (image - shifted) ** 2

        # Sum the squared differences over a patch_size x patch_size
        # window around every pixel (this is the "SSD over a patch"
        # from the slides) -- implemented efficiently as a box-filter
        # convolution.
        ssd = ndimage.convolve(squared_diff, box_kernel, mode='reflect')

        # Keep a running minimum across the 8 directions: the
        # cornerness of a pixel is governed by its LEAST dissimilar
        # (best-matching) direction.
        cornerness = ssd if cornerness is None else np.minimum(cornerness, ssd)

    return cornerness


def moravec_non_max_suppression(cornerness, neighborhood_size=3):
    """
    Step 2: non-maximum suppression.

    A pixel's cornerness is discarded (set to 0) if any of its
    neighbors (within a `neighborhood_size` x `neighborhood_size`
    window) has a strictly higher cornerness value -- that
    neighbor is then the better corner candidate at this location.
    """
    local_max = ndimage.maximum_filter(cornerness, size=neighborhood_size,
                                        mode='reflect')
    is_local_max = cornerness >= local_max
    suppressed = np.where(is_local_max, cornerness, 0.0)
    return suppressed


def moravec_corner_detection(image, patch_radius=1, shift=1,
                              nms_neighborhood=3, threshold_ratio=0.1):
    """
    Full Moravec corner detector = Step 1 + Step 2 + Step 3.

    Parameters
    ----------
    image : 2D float ndarray (grayscale image)
    patch_radius : int
        Half-width of the square patch used for SSD (slides: 3x3
        patch -> patch_radius=1).
    shift : int
        Pixel shift used for the 8 principal directions.
    nms_neighborhood : int
        Size of the non-maximum-suppression neighborhood.
    threshold_ratio : float
        Step 3: final threshold on the (non-max-suppressed)
        cornerness map, expressed as a fraction of its maximum
        value.

    Returns
    -------
    corner_mask : 2D bool ndarray
        True at detected corner locations.
    cornerness  : 2D float ndarray
        The raw (pre-NMS) cornerness map, useful for visualization.
    """
    # --- Step 1: raw cornerness (min SSD over 8 directions) ---
    cornerness = moravec_cornerness(image, patch_radius=patch_radius, shift=shift)

    # --- Step 2: non-maximum suppression ---
    suppressed = moravec_non_max_suppression(cornerness,
                                              neighborhood_size=nms_neighborhood)

    # --- Step 3: threshold to obtain final corner locations ---
    threshold = threshold_ratio * suppressed.max()
    corner_mask = suppressed > threshold

    return corner_mask, cornerness


# ---------------------------------------------------------------
# MAIN: load every image in ./data/, run Moravec corner detection,
# plot original (with corners marked) + cornerness map side by
# side, and save the comparison figure to ./output/
# ---------------------------------------------------------------
def process_and_visualize(image_path, output_dir):
    """Run the Moravec detector on one image and save a
    side-by-side visualization (original+corners, cornerness map)."""

    # Load and prepare a single-channel float image in [0, 1]
    img = io.imread(image_path)
    if img.ndim == 3:
        img = color.rgb2gray(img)  # drop alpha/color -> grayscale
    img = img_as_float(img)

    corner_mask, cornerness = moravec_corner_detection(
        img, patch_radius=1, shift=1,
        nms_neighborhood=3, threshold_ratio=0.1)

    ys, xs = np.nonzero(corner_mask)

    # Plot: original image with corners overlaid, and the raw
    # cornerness heatmap, side by side.
    fig, axes = plt.subplots(1, 2, figsize=(12, 6))

    axes[0].imshow(img, cmap='gray')
    axes[0].scatter(xs, ys, s=12, facecolors='none', edgecolors='red', linewidths=1)
    axes[0].set_title(f'Moravec corners (n={len(xs)})')
    axes[0].axis('off')

    axes[1].imshow(cornerness, cmap='gray')
    axes[1].set_title('Cornerness map (min SSD over 8 directions)')
    axes[1].axis('off')

    plt.tight_layout()

    basename = os.path.splitext(os.path.basename(image_path))[0]
    out_path = os.path.join(output_dir, f"{basename}_moravec_corners.png")
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