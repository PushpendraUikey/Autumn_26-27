import os
from PIL import Image

def convert_tif_to_png(input_path, output_path):
    """
    Converts a TIFF image to PNG format to ensure compatibility 
    with the stb_image C++ library.
    """
    if not os.path.exists(input_path):
        print(f"Error: Could not find {input_path}. Please check your data folder structure.")
        return
    
    try:
        # Open the TIFF and save it as PNG
        img = Image.open(input_path)
        img.save(output_path, 'PNG')
        print(f"Success: Converted '{input_path}' to '{output_path}'")
    except Exception as e:
        print(f"An error occurred during conversion: {e}")

if __name__ == "__main__":
    print("Starting data conversion...")
    
    # Define the target paths
    input_file = './data/lilavati.tif'
    output_file = './data/lilavati.png'
    
    convert_tif_to_png(input_file, output_file)