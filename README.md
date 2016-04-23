# ObjectTracker

## Note
This pipeline is still very much a prototype, so please keep that in mind if you'd like to use it. I am still learning computer vision, so I'd appreciate any suggestions on how to improve this pipeline's tracking ability or make it simpler (while maintaining similar performance). Thank you!

This README describes how the tracking pipeline works. I hope it can be helpful to you if you decide to build your own tracking pipeline.

## Acknowledgements
I want to give credit to:

This [paper](http://ceur-ws.org/Vol-1391/40-CR.pdf) by Gábor Szűcs, Dávid Papp, and Dániel Lovas, which outlines the Kalman + Hungarian tracking pipeline.

This [repo](https://github.com/Smorodov/Multitarget-tracker) by Andrey Smorodov, which implements the Kalman + Hungarian pipeline.
I use his code for the Hungarian algorithm and his technique for initializing the matrices for Kalman filters.

This [website](http://web.rememberingemil.org/Projects/DisjointSets.aspx.html) by Emil Stefanov, 
which has an implementation of the union-find (aka. disjoint set) data structure.

[OpenCV](http://opencv.org/), which does most of the heavy lifting for the computer vision tasks that I do.

## Intro
This is an object tracking pipeline that I built. It is designed for high mounted fixed cameras 
to track objects moving in the scene below them. So, I'm making two important assumptions here:

* The camera is not moving
* It has an overhead (or almost overhead) view of the moving objects

Here's an outline of its operation, with explanations of why these decisions were made.

## Resize Image
Computing on large images is expensive, so I first shrink each frame down to **(300, 300)** pixels.

## Background Subtraction
To separate the foreground (the moving objects) from the backgound, I use background subtraction
with a [Mixture of Gaussians](http://docs.opencv.org/2.4/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractormog2).
This basically models the probability that a given pixel will change intensity in the next frame.
So, pixels that are very likely to change intensity are the foreground.

I also use this model to **detect shadows** and threshold them out.

## Preprocessing
To **remove salt and pepper noise**, I use a **median blur**. This, along with the removal of shadows, can lead
to **holes in the image**, so I fill those in by **dilating** the image a few times.

## Contours

### First Pass
Now, I find the **contours** in the image. It's possible that **contours are drawn around noise**, so I address that
by removing all contours that have an **area that is less than 10% of the largest contour's area**. Then, I compute the **center of mass** and **bounding box** around each of the remaining contours.

### Merging
Sometimes, an object **may appear as two small pieces, rather than one whole**. So, I want to merge contours that
are near each other. To do this, I have to explain two things:

1. What does it mean for contours to be near each other?
2. How do you merge contours?

To figure out if two contours are near each other, I first compute the **distance between their mass centers**, call it `dist`.
Then, I look at the bounding boxes, call them `b1` and `b2`. Specifically, I decide to merge the two contours if:

```
dist <= 0.7 * max(max(b1.width, b1.height), max(b2.width, b2.height))
```

I keep track of contours that should be merged using the **union find** data structure.

Well, that's how I find the contours that should be merged. How do I merge them? I simply find the **convex hull** around
each set of contours that must be merged.

### Second Pass
After the merging process, I have the final set of contours. So, I **recompute the bounding boxes and mass centers**.

## Multiple Object Tracking
Now, I feed these mass centers and bounding boxes to the multiple object tracker.

At a high level, the multi-tracker basically **associates a Kalman filter to track each moving object**.
There's a challenge here though, which is that the **mass centers for a given frame don't tell me which object they
belong to**. That's something I need to address. But first...

### Base Cases
If there are **no mass centers**, I just **update each Kalman filter** (using their most recent observation).

If there are some mass centers, but **no Kalman filters**, I **create one for each mass center**.

### Hungarian Algorithm
Since the two base cases have been handled, now I can be sure that I have some Kalman filters and some mass centers.

So, the problem is: **how should mass centers be paired with Kalman filters?**

To make this happen, I use the **[Hungarian Algorithm](https://en.wikipedia.org/wiki/Hungarian_algorithm)**. This will
assign Kalman filters to mass centers. The **cost matrix uses the distance** between the Kalman filter and mass center.

### Distance Filter
The assignment found by the Hungarian Algorithm may **assign a Kalman filter with a mass center that is very far away**.
This happens because all the other mass centers are already paired up, so the Kalman filter is forced to pair with a
faraway mass center.

To address this problem, I iterate and **unpair a Kalman filter and mass center if they are too far away**.
I define "too far away" as 

```
distance(Kalman filter, mass center) > (frame.width + frame.height)/2
```

### Dead Trackers
When a **Kalman filter's object has left the screen**, I don't want that Kalman filter sticking around, so
I remove any Kalman filters that have **gone more than 10 frames without being paired with a mass center**.

### Unassigned Mass Centers
Some mass centers **may not have been paired** with a Kalman filter, so I create Kalman filters for them.

### Kalman Update
Now, I **update each Kalman filter** with its paired mass center or with the most recent mass center it has seen.

### Ignore Young Filters
Sometimes, if there's a **patch of noise that persists many frames**, a Kalman filter may be created for it. However,
such Kalman filters usually don't live long, so I only pay attention to Kalman filters that **have been alive for 20 frames**.
