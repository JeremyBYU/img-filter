import argparse
from PIL import Image, ImageOps
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--file')
    args = parser.parse_args()
    if args.file is None:
        raise ValueError("Missing File Arguments")
    return args

def scale_depth(image, max_depth=3.0, depth_scale=10000):
    """Scale Z16 image to Float between 0-1
    
    Arguments:
        image {PIL.Image} -- Pillow Image
    
    Keyword Arguments:
        max_depth {float} -- Maximum Depth (default: {4.0})
    """
    data = np.asarray(image)
    scale_factor = (max_depth / 10.0) * 10000
    data1 = ((data / scale_factor) * 255).astype(np.uint8)
    scaled_image = Image.fromarray(data1,mode='L')
    color_image = ImageOps.colorize(scaled_image, 'blue', 'red')
    return color_image


def show_image(fpath):
    with open(fpath, 'rb') as f:
        image_bytes = f.read()
    image = Image.frombytes("I;16", (848,480), image_bytes, 'raw')
    color_image = scale_depth(image)
    color_image.show()

def main():
    args = parse_args()

    print(args)
    show_image(args.file)



if __name__ == "__main__":
    main()