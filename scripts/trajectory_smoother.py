import json
import sys
import numpy as np
from scipy.signal import medfilt
from scipy.ndimage.filters import gaussian_filter

data = json.load(open(sys.argv[1]))


def smoothTrajectory(track, sigma=51):
    trackNp = np.array(track, dtype=float).transpose()
    xvals = gaussian_filter(trackNp[0], sigma)
    yvals = gaussian_filter(300 - trackNp[1], sigma)
    return zip(xvals, yvals, trackNp[2])


def transform(i, track):
    (x, y, frame) = track[i]
    return (x, y)


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

        (tx, ty) = transform(i, track)
        pointsForFrame[frame].append((x, y, tx, ty, frame, trackerId))

print "frame, x, y, tx, ty, trackerId, trackerIndex"
for i in xrange(0, data["numFrames"]):
    if i not in pointsForFrame:
        continue
    for (x, y, tx, ty, frame, trackerId) in pointsForFrame[i]:
        print ",".join((str(frame), str(x), str(y), str(tx), str(ty), str(
            trackerId), str(indexForTrackerId[trackerId])))
