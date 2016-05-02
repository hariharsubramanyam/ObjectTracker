import json
import sys
import numpy as np
import argparse
from scipy.ndimage.filters import gaussian_filter

parser = argparse.ArgumentParser(
    description=
    'Create smoothed trajectories, velocities, and accelerations from JSON.')
parser.add_argument('input',
                    metavar='input',
                    type=str,
                    help="Path to the input JSON file")
parser.add_argument('timestamp',
                    metavar='timestamp',
                    type=float,
                    help="Starting timestamp of the video")
parser.add_argument('duration',
                    metavar='duration',
                    type=float,
                    help='Duration of the video in seconds')
args = parser.parse_args()

# The first positional argument is the path to the JSON file.
data = json.load(open(args.input))


def smoothTrajectory(track, sigma=51):
    trackNp = np.array(track, dtype=float).transpose()
    xvals = gaussian_filter(trackNp[0], sigma)
    yvals = gaussian_filter(300 - trackNp[1], sigma)
    return zip(xvals, yvals, trackNp[2])


def velocity(i, track):
    (x, y, frame) = track[i]
    if len(track) <= 1:
        return (x, y)

    if i == 0:
        return velocity(i + 1, track)
    else:
        (xp, yp, framep) = track[i - 1]
        return ((x - xp) / (frame - framep), (y - yp) / (frame - framep))


def get_timestamp(currFrame, totalFrames, startingTs, duration):
    proportionOfVideo = float(currFrame) / float(totalFrames)
    return startingTs + proportionOfVideo * duration


def acceleration(i, track):
    (x, y, frame) = track[i]
    if len(track) <= 2:
        return (x, y)

    if i == 0:
        return acceleration(i + 1, track)
    elif i == len(track) - 1:
        return acceleration(i - 1, track)
    else:
        (xp, yp, framep) = track[i - 1]
        (xn, yn, framen) = track[i + 1]
        h1 = frame - framep
        h2 = framen - frame
        return ((xn - 2 * x + xp) / (h1 * h2), (yn - 2 * y + yp) / (h1 * h2))


def curvature(xVel, yVel, xAcc, yAcc):
    if xVel == 0 and yVel == 0:
        return 0
    return (xVel * yAcc - yVel * xAcc) / (xVel**2 + yVel**2)**(1.5)


def mag(a, b):
    return (a**2 + b**2)**(0.5)


numFrames = float(data["numFrames"])

trackForId = {}
indexForTrackerId = {}
for i in xrange(len(data["trackers"])):
    tracker = data["trackers"][i]
    indexForTrackerId[tracker["trackerId"]] = i
    trackForId[tracker["trackerId"]] = smoothTrajectory(tracker["track"])

pointsForFrame = {}

for trackerId in trackForId:
    track = trackForId[trackerId]
    for i in xrange(len(track)):
        (x, y, frame) = track[i]
        frame = int(frame)
        if frame not in pointsForFrame:
            pointsForFrame[frame] = []

        (xVel, yVel) = velocity(i, track)
        (xAcc, yAcc) = velocity(i, track)
        pointsForFrame[frame].append((x, y, xVel, yVel, xAcc, yAcc, frame,
                                      trackerId))

print "frame, timestamp, x, y, xVel, yVel, velMag, xAcc, yAcc, accMag, curvature, trackerId, trackerIndex"
for i in xrange(0, data["numFrames"]):
    if i not in pointsForFrame:
        continue
    for (x, y, xVel, yVel, xAcc, yAcc, frame, trackerId) in pointsForFrame[i]:
        output = []
        output.append(frame)
        output.append(get_timestamp(frame, numFrames, args.timestamp,
                                    args.duration))
        output.append(x)
        output.append(y)
        output.append(xVel)
        output.append(yVel)
        output.append(mag(xVel, yVel))
        output.append(xAcc)
        output.append(yAcc)
        output.append(mag(xAcc, yAcc))
        output.append(curvature(xVel, yVel, xAcc, yAcc))
        output.append(trackerId)
        output.append(indexForTrackerId[trackerId])
        output = [str(o) for o in output]
        print ",".join(output)
