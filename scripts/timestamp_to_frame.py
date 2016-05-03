"""
Convert a CSV file of (timestamp, x, y) tuples into a CSV file of
(timestamp, x, y, frame) tuples given the number of frames in the video, the
starting timestamp of the video (in seconds), and the duration of the video
(in seconds).

Run this program as:

python timestamp_to_frame.py <input_file_path> <num_frames> <start_ts> <duration>
"""

import sys

# Read the inputs
inputLines = open(sys.argv[1]).read().replace("\r", "\n").split("\n")
inputLines = filter(lambda line: len(line) > 1, inputLines)
numFrames = int(sys.argv[2])
startTimestamp = float(sys.argv[3])
duration = float(sys.argv[4])


# Parse the input lines into (timestamp, x, y) tuples.
def parseLine(line):
    line = line.split(",")
    line = [float(l.strip()) for l in line]
    return line
parsedLines = map(parseLine, inputLines[1:])

# Given a timestamp, compute the frame.
def frameForTimestamp(timestamp):
    global numFrames, startTimestamp, duration
    return int((timestamp - startTimestamp) / duration * numFrames)

# Create the output file.
print "timestamp, x, y, frame"
for line in parsedLines:
    (timestamp, x, y) = line
    outputTuple = (timestamp, int(x), int(y), frameForTimestamp(timestamp))
    outputTuple = [str(t) for t in outputTuple]
    print ",".join(outputTuple)
