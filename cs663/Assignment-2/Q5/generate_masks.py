import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import PolygonSelector
from matplotlib.path import Path
from PIL import Image

def create_manual_mask(image_path: str, mask_path: str):
    """
    Opens an interactive window to draw a polygon mask over the foreground.
    """
    if not os.path.exists(image_path):
        print(f"Image not found {image_path}")
        return

    img = Image.open(image_path)
    img_array = np.asarray(img)

    fig, ax = plt.subplots(figsize=(10, 8))
    ax.imshow(img_array)
    ax.set_title(f"Click to outline the foreground. {os.path.basename(image_path)}\nPress 'Enter' to confirm and save, or 'Esc' to cancel.")

    polygon_pts = []

    def onselect(verts):
        nonlocal polygon_pts
        polygon_pts = verts

    def on_key(event):
        if event.key == 'enter':
            plt.close()

    fig.canvas.mpl_connect('key_press_event', on_key)
    selector = PolygonSelector(ax, onselect, useblit=True)
    plt.show()

    if not polygon_pts:
        print(f"No mask drawn for {os.path.basename(image_path)}. Skipping.")
        return

    h, w = img_array.shape[:2]
    x, y = np.meshgrid(np.arange(w), np.arange(h))
    points = np.vstack((x.flatten(), y.flatten())).T

    path = Path(polygon_pts)
    mask_flat = path.contains_points(points)
    mask_2d = mask_flat.reshape((h, w))

    mask_image = Image.fromarray((mask_2d * 255).astype(np.uint8))
    mask_image.save(mask_path)
    print(f"Saved binary mask to {mask_path}")


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

        create_manual_mask(img_path, mask_path)