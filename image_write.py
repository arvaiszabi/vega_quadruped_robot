import cv2 as cv
import numpy as np


def image_writer(path):
    image = cv.imread(path, cv.IMREAD_UNCHANGED)
    image = cv.cvtColor(image, cv.COLOR_BGRA2RGBA)
    image = np.asfarray(image, dtype='f')
    raw_image = np.true_divide(image, 255.0)
    return raw_image
