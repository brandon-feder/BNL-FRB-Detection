import numpy as np
from matplotlib import pyplot as plt
from math import *

ANGLE_ACCURACY = 200
SIGMA_DETECTION_VALUE = 3

class Data:
    def __init__(self):
        self.Nt, self.Nf, self.slope, self.i = 1000, 128, 0.3, -1
        self.signal = np.random.normal(5,1.0, (self.Nf, self.Nt))
        for f in range(self.Nf//8, 7*self.Nf//8): self.signal[f, int(np.round(self.Nt//2+(f-self.Nf//2)*self.slope))]+=10
        self.signal /= np.amax(self.signal)

    def nextFrame(self):
        self.i += 1
        return self.signal[:, self.i:self.Nf+self.i] if self.Nf+self.i <= self.Nt else None


class FRBDetector:
    def __pixelIntersections(self, start, end):
        dx, dy = end[0] - start[0], end[1] - start[1]
        r = max(abs(dx), abs(dy))
        dx /= r
        dy /= r

        pixels = []

        x, y = start
        for _ in range(int(r)):
            pixels.append((round(x), round(y)))
            x += dx
            y += dy

        return pixels

    def __init__(self, w, h):
        self.intersects = []
        self.weights = []
        self.angles = []
        for theta in np.linspace(0, np.pi, ANGLE_ACCURACY):
            t = np.tan(theta)

            if t == 0: f = w//2-1
            else: f = min( abs((h//2-1)/t), w//2-1 )

            self.intersects.append(((pi/2 - theta)%pi, self.__pixelIntersections((t*f, f), (-t*f, -f))))
            self.weights.append([])

        self.w, self.h = w, h

    def detect(self, frame):
        for line, weights in zip(self.intersects, self.weights):
            S = 0
            for pixel in line[1]:
                S += frame[self.w//2 + pixel[0]][self.h//2 + pixel[1]]
            
            S /= len(self.intersects[1])
            if len(weights) < 32: weights.append(S)
            else: weights = weights[1:len(weights)] + [S]

            if S >= sum(weights)/len(weights) + SIGMA_DETECTION_VALUE*sqrt(S):
                return ( line[0], S )
        
        return None

def main():
    test_data = Data()

    # Note that Nf was used twice to get a square viewport
    detect = FRBDetector(test_data.Nf, test_data.Nf)

    i = test_data.Nf//2
    while not (sig_frame := test_data.nextFrame()) is None:
        res = detect.detect( sig_frame )
        if not res == None:
            print( "i: {}; angle: {}; avg: {};".format( i, res[0], res[1] ) )
        
        i += 1

if __name__ == "__main__":
    main()