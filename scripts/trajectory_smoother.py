import json
import sys
import numpy as np
from scipy.ndimage.filters import gaussian_filter

# The first positional argument is the path to the JSON file.
data = json.load(open(sys.argv[1]))


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

print "frame, x, y, xVel, yVel, xAcc, yAcc, trackerId, trackerIndex"
for i in xrange(0, data["numFrames"]):
    if i not in pointsForFrame:
        continue
    for (x, y, xVel, yVel, xAcc, yAcc, frame, trackerId) in pointsForFrame[i]:
        print ",".join((str(frame), str(x), str(y), str(xVel), str(yVel), str(
            xAcc), str(yAcc), str(trackerId), str(indexForTrackerId[trackerId])
                        ))
