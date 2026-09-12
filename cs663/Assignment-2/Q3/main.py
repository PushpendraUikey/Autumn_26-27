import numpy as np
from scipy import ndimage
import os
import utils

# ------------------ Q3 Canny Edge Detection ------------------ #
# PHASE1 : Gaussian noise suppression function
def apply_guassian_smoothing(image: np.ndarray, sigma: float) -> np.ndarray:
    """
    Applies Gaussian smoothing to the input image using a specified sigma value.
    """
    size = int(2 * np.ceil(3 * sigma) + 1)
    ax = np.arange(-size // 2 + 1., size // 2 + 1.)
    xx, yy = np.meshgrid(ax, ax)
    kernel = np.exp(-(xx**2 + yy**2) / (2. * sigma**2))
    kernel = kernel / np.sum(kernel)

    # convolution with boundary reflection
    smoothed_image = ndimage.convolve(image, kernel, mode='reflect')
    return smoothed_image


# PHASE2 : Gradient Computation using Sobel Operators on smoothed image
def compute_gradients(smoothed_img: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """
    Computes the gradient magnitude and direction of an image using Sobel Operators.
    """
    sobel_x = np.array([[-1, 0, +1],
                         [-2, 0, +2],
                         [-1, 0, +1]], dtype=np.float64)
    sobel_y = np.array([[-1, -2, -1],
                         [0,   0,  0],
                         [+1, +2, +1]], dtype=np.float64)

    Gx = ndimage.convolve(smoothed_img, sobel_x, mode='reflect')
    Gy = ndimage.convolve(smoothed_img, sobel_y, mode='reflect')

    magnitude = np.sqrt(Gx**2 + Gy**2)
    direction = np.arctan2(Gy, Gx)

    direction = np.rad2deg(direction)
    direction[direction < 0] += 180

    if magnitude.max() > 0:
        magnitude = (magnitude / magnitude.max())

    return magnitude, direction


# PHASE3 : Non-Maximum Suppression
def non_maximum_suppression(magnitude: np.ndarray, direction: np.ndarray) -> np.ndarray:
    """
    Non-Maximum Suppression -> Thins edges by suppressing pixels that are not
    local maxima in the direction of the gradient.
    """
    h, w = magnitude.shape
    suppressed = np.zeros((h, w), dtype=np.float64)

    for i in range(1, h - 1):
        for j in range(1, w - 1):
            angle = direction[i, j]
            mag = magnitude[i, j]

            # edge is vertical (0 degrees). Check left and right pixels
            if (0 <= angle < 22.5) or (157.5 <= angle <= 180):
                q = magnitude[i, j + 1]
                r = magnitude[i, j - 1]
            # edge is diagonal (-45 degrees). Check top-right and bottom-left
            elif 22.5 <= angle < 67.5:
                q = magnitude[i + 1, j - 1]
                r = magnitude[i - 1, j + 1]
            # edge is horizontal (90 degrees). Check top and bottom pixels
            elif 67.5 <= angle < 112.5:
                q = magnitude[i + 1, j]
                r = magnitude[i - 1, j]
            # edge is diagonal (45 degrees). Check top-left and bottom-right
            else:  # 112.5 <= angle < 157.5
                q = magnitude[i - 1, j - 1]
                r = magnitude[i + 1, j + 1]

            if (mag >= q) and (mag >= r):
                suppressed[i, j] = mag
            else:
                suppressed[i, j] = 0

    return suppressed


# PHASE4 : Double Thresholding & Edge Tracking by Hysteresis
def double_threshold_and_hysterisis(suppressed: np.ndarray, t_low: float, t_high: float) -> np.ndarray:
    """
    Applies double thresholding and edge tracking by hysteresis to the suppressed image.
    """
    h, w = suppressed.shape
    final_edges = np.zeros((h, w), dtype=np.float64)

    strong_y, strong_x = np.where(suppressed >= t_high)

    is_weak = np.zeros((h, w), dtype=bool)
    is_weak[(suppressed >= t_low) & (suppressed < t_high)] = True

    final_edges[strong_y, strong_x] = 1.0

    stack = list(zip(strong_y, strong_x))  # DFS seeds

    neighbors = [(-1, -1), (-1, 0), (-1, 1),
                 (0, -1),           (0, 1),
                 (1, -1),  (1, 0),  (1, 1)]
    while stack:
        y, x = stack.pop()

        for dy, dx in neighbors:
            ny, nx = y + dy, x + dx

            if 0 <= ny < h and 0 <= nx < w:
                if is_weak[ny, nx] and final_edges[ny, nx] == 0.0:
                    final_edges[ny, nx] = 1.0
                    stack.append((ny, nx))

    return final_edges


def canny_edge_detector(image: np.ndarray, sigma: float, t_low: float, t_high: float) -> tuple:
    """
    Canny Edge Detection pipeline execution.
    """
    smoothed = apply_guassian_smoothing(image, sigma)
    mag, direction = compute_gradients(smoothed)
    nms = non_maximum_suppression(mag, direction)
    final_edges = double_threshold_and_hysterisis(nms, t_low, t_high)

    return mag, nms, final_edges


if __name__ == "__main__":
    base_path = "../data/edge/"
    output_path = "./output/"
    os.makedirs(output_path, exist_ok=True)

    canny_params = {
        "butterfly.png": {"sigma": 1.0, "t_low": 0.05, "t_high": 0.15},
        "paithaniEdge.png": {"sigma": 2.0, "t_low": 0.08, "t_high": 0.25},
        "rangoli.png": {"sigma": 1.0, "t_low": 0.05, "t_high": 0.15},
    }

    for img_name, params in canny_params.items():
        print(f"Processing Canny Edge Detection for : {img_name}")
        img_path = os.path.join(base_path, img_name)
        image = utils.load_image(img_path, as_gray=True)

        sigma = params["sigma"]
        t_low = params["t_low"]
        t_high = params["t_high"]

        mag, nms, final_edges = canny_edge_detector(image, sigma, t_low, t_high)

        title = f"{img_name} | sigma={sigma}, t_low={t_low}, t_high={t_high}"
        stem = os.path.splitext(img_name)[0]

        # Part (b), deliverables 1+2: input + binary edge map, side by side.
        utils.save_input_binary_pair(
            image, final_edges, title,
            os.path.join(output_path, f"Q3_pair_{stem}.png")
        )

        # Part (b), deliverable 3: original with edges painted black, as its
        # own separate composite image.
        utils.save_edge_overlay(
            image, final_edges, title,
            os.path.join(output_path, f"Q3_overlay_{stem}.png")
        )