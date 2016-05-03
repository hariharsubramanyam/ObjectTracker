"""
Given a predicted position CSV file, which has (timestamp, x, y, frame) tuples
and a ground truth position CSV file, which has (frame, x, y) tuples, evaluate
the accuracies.

Execute this program as:

python evaluator.py <ground_truth_file> <prediction_file>
"""

import sys
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
from scipy.stats import describe

# Read the ground truth.
groundTruth = open(sys.argv[1]).read().replace("\r", "\n").split("\n")[1:]
groundTruth = filter(lambda line: len(line) > 3, groundTruth)


def parseLine(line):
    line = line.split(",")
    line = [s.strip() for s in line]
    return tuple([int(float(s)) for s in line])


groundTruth = map(parseLine, groundTruth)

# Read the prediction file.
predictions = open(sys.argv[2]).read().replace("\r", "\n").split("\n")[1:]
predictions = filter(lambda line: len(line) > 3, predictions)
predictions = map(parseLine, predictions)


# Get the predicted position for the given frame.
def predictionForFrame(frame):
    global predictions
    predX = 0
    predY = 0
    predFrame = 0
    for prediction in predictions:
        (framePred, x, y) = prediction
        if framePred <= frame:
            (predFrame, predX, predY) = (framePred, x, y)
    return (predFrame, predX, predY)


# Measure the error between two points.
def error(pair1, pair2):
    (x1, y1) = pair1
    (x2, y2) = pair2
    return ((x2 - x1)**2 + (y2 - y1)**2)**0.5

# Find the error for each frame of the ground truth.
frameErrorPairs = []
for actual in groundTruth:
    (aTimestamp, ax, ay, aFrame) = actual
    (pFrame, px, py) = predictionForFrame(aFrame)
    frameErrorPairs.append((aFrame, error((ax, ay), (px, py))))

errors = np.array([x[1] for x in frameErrorPairs])

print describe(errors)

plt.hist(errors, 50, normed=1, facecolor='green', alpha=0.75)
plt.show()
