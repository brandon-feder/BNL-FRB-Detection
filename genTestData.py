import json
import sys
import numpy as np
from matplotlib import pyplot as plt
import os
from math import sin, cos, pi

# Parses command-line arguments
def parse_args():
    arg_dict = {
        "display":False,
        "width":500,
        "height":500,
        "n":1
    }

    arg_inx = 1
    try:
        while arg_inx < len(sys.argv):
            if sys.argv[arg_inx] in {"-d", "-display"}:
                arg_dict["display"] = True
            elif sys.argv[arg_inx] in {"-w", "-width"}:
                arg_dict["width"] = int(sys.argv[arg_inx+1])
                arg_inx += 1
            elif sys.argv[arg_inx] in {"-h", "-height"}:
                arg_dict["height"] = int(sys.argv[arg_inx+1])
                arg_inx += 1
            elif sys.argv[arg_inx] in {"-n"}:
                arg_dict["n"] = int(sys.argv[arg_inx+1])
                arg_inx += 1
            else:
                raise ValueError("Invalid Command Line Arguments")

            arg_inx += 1
    except:
        raise ValueError("Invalid Command Line Arguments")

    return arg_dict

# Draw line on 2D list/array
def draw_line(img, start, end):
    # Calculate change in x and y
    dx = end[0] - start[0]
    dy = end[1] - start[1]
    m = max(abs(dx), abs(dy))
    dx /= m
    dy /= m

    # Draw line
    x, y = start
    for _ in range(m):
        img[round(x), round(y)] = 100
        x += dx
        y += dy

def main():
    # Parse arguments
    args = parse_args()

    for _ in range(args["n"]):
        # Create image
        img = np.zeros((args["width"], args["height"]))

        # Draw the line on the data
        theta = np.random.random()*pi
        l = min(args["width"], args["height"])//4
        x, y = l*cos(theta), l*sin(theta)
        draw_line(img,
            (int(y+args["height"]//2), int(x+args["width"]//2)),
            (int(-y+args["height"]//2), int(-x+args["width"]//2))
        )

        # Display if requested
        if args["display"]:
            plt.imshow(img)
            plt.show()

        # Get next file to output to
        i = 1
        while os.path.exists("./test-data/data{}.json".format(i)):
            i += 1

        # Output to file
        with open("./test-data/data{}.json".format(i), "w") as f:
            json.dump({
                "width":args["width"],
                "height":args["height"],
                "data":img.tolist(),
                "angle":pi - theta
            }, f)


if __name__ == "__main__":
    main()
