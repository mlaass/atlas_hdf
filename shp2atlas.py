#!/bin/python3
import atlashdf as at
import sys
import io
import os
import sys
import random
import json
import h5py
from tqdm import tqdm
import numpy as np
import matplotlib.pyplot as plt
import sys
import inspect


def print_classes():
    for name, obj in inspect.getmembers(sys.modules[__name__]):
        print(obj)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("shp2atlas <shapefile> <atlasfile>")
    outer, coords = at.read_shapefile(sys.argv[1])
    print("saveing file", sys.argv[2], "...")
    atlas = h5py.File(sys.argv[2], "w")
    atlas.create_dataset("coords",  data=coords,
                         chunks=True, compression="gzip")
    atlas.create_dataset("polygons",  data=outer,
                         chunks=True, compression="gzip")
