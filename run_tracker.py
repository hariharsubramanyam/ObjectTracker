#!/usr/bin/env python

import sys
import os
import argparse
data = {
    "stata1":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata1/stata1.mov",
     1461811541.050154, 244,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata1/tracker.json",
     "67 471 248 205 588 220 717 478",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata1/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata1/input_for_eric_perspective.csv"
     ),
    "stata2":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata2/stata2.mov",
     1461811906.243576, 275,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata2/tracker.json",
     "67 476 261 175 608 182 719 475",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata2/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata2/input_for_eric_perspective.csv"
     ),
    "stata3":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata3/stata3.mov",
     1461812418.383915, 217,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata3/tracker.json",
     "62 478 262 178 600 188 719 473",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata3/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Stata3/input_for_eric_perspective.csv"
     ),
    "chem1":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem1/chem1.mov",
     1461809912.643582, 209,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem1/tracker.json",
     "DON'T DO PERSPECTIVE",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem1/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem1/input_for_eric_perspective.csv"
     ),
    "chem2":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem2/chem2.mov",
     1461810598.068358, 237,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem2/tracker.json",
     "DON'T DO PERSPECTIVE",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem2/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection1/Chem2/input_for_eric_perspective.csv"
     ),
    "lobby71":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/Lobby7_1.mov",
     1462273804.346465, 180,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/tracker.json",
     "166 167 181 399 717 260 583 110",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_1/input_for_eric_perspective.csv"
     ),
    "lobby72":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/Lobby7_2.mov",
     1462274331.162998, 284,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/tracker.json",
     "194 399 174 162 533 95 691 268",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Lobby7_2/input_for_eric_perspective.csv"
     ),
    "walker1":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/Walker1.mov",
     1462275499.029912,
     312,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/tracker.json",
     "2 401 259 16 485 22 636 455",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker1/input_for_eric_perspective.csv",
     ),
    "walker2":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/Walker2.mov",
     1462275918.126208, 326,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/tracker.json",
     "3 391 278 17 488 20 641 455",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection2/Walker2/input_for_eric_perspective.csv"
     ),
    "court1":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard1/Courtyard1.mov",
     1462375212.026118, 172,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard1/tracker.json",
     "110 473 307 155 619 178 631 472",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard1/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard1/input_for_eric_perspective.csv"
     ),
    "court2":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard2/Courtyard2.mov",
     1462375480.266197, 151,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard2/tracker.json",
     "110 473 307 155 619 178 631 472",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard2/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard2/input_for_eric_perspective.csv"
     ),
    "court3":
    ("~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard3/Courtyard3.mov",
     1462375734.968232, 185,
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard3/tracker.json",
     "110 473 307 155 619 178 631 472",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard3/input_for_eric.csv",
     "~/Dropbox/6.S062\ Final\ Project\ Videos/MITDataCollection3/Courtyard3/input_for_eric_perspective.csv"
     )
}

parser = argparse.ArgumentParser(
    description='Run the tracker, plotter, or ground truth annotator.')
parser.add_argument(
    'input',
    metavar='input',
    type=str,
    help=
    "Name of input video (it should be one of the keys in the dictionary)")
parser.add_argument('mode',
                    metavar='mode',
                    type=str,
                    help="Should be tracker, plotter, or ground_truth")
parser.add_argument('--perspective',
                    metavar='perspective',
                    type=bool,
                    default=False,
                    help='Whether a perspective transform should be applied')
parser.add_argument('--out',
                    metavar='out',
                    type=bool,
                    default=False,
                    help='Whether an output file should be generated')
args = parser.parse_args()

(videoPath, timestamp, duration, trackerFile, perspective, smoothedPathUrl,
 smoothedPathPerspectiveUrl) = data[args.input]

maxDimension = 300

if args.mode == "tracker":
    # First run the object tracker.
    argumentTuple = [videoPath, maxDimension]
    formatString = "./start.sh -m tracker -i %s -d %s"

    if args.perspective:
        argumentTuple.append(perspective)
        formatString += " -p %s"

    if args.out:
        argumentTuple.append(trackerFile)
        formatString += " -s %s"

    command = (formatString % tuple(argumentTuple))
    os.system(command)

    # Smooth the trajectories.
    smoothOutputFile = smoothedPathUrl
    if args.perspective:
        smoothOutputFile = smoothedPathPerspectiveUrl
    if args.out:
        argumentTuple = [trackerFile, timestamp, duration, smoothOutputFile]
        formatString = "python scripts/trajectory_smoother.py %s %s %s > %s"
        command = (formatString % tuple(argumentTuple))
        os.system(command)
