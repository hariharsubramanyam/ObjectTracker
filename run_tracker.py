#!/usr/bin/env python

import sys
import os

data = {
    "lobby71":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/Lobby7_1.mov",
     1462273804.346465, 180,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/tracker.json",
     "166 167 181 399 717 260 583 110",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/input_for_eric.csv"
     ),
    "lobby72":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/Lobby7_2.mov",
     1462274331.162998, 284,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/tracker.json",
     "194 399 174 162 533 95 691 268",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/input_for_eric.csv"
     ),
    "walker1":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/Walker1.mov",
     1462275499.029912,
     312,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/tracker.json",
     "2 401 259 16 485 22 636 455",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/input_for_eric.csv",
     ),
    "walker2":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/Walker2.mov",
     1462275918.126208, 326,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/tracker.json",
     "3 391 278 17 488 20 641 455",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/input_for_eric.csv"
     )
}

(videoPath, timestamp, duration, trackerFile, perspective,
 smoothedPathUrl) = data[sys.argv[1]]

maxDimension = 300

# First run the object tracker.
argumentTuple = [videoPath, trackerFile, maxDimension]
formatString = "./start.sh -m tracker -i %s -s %s -d %s"

if len(sys.argv) == 3:
    argumentTuple.append(perspective)
    formatString += " -p %s"

command = (formatString % tuple(argumentTuple))
os.system(command)

# Smooth the trajectories.
argumentTuple = [trackerFile, timestamp, duration, smoothedPathUrl]
formatString = "python scripts/trajectory_smoother.py %s %s %s > %s"
command = (formatString % tuple(argumentTuple))

os.system(command)
