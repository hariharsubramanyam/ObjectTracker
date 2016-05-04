#ifndef tracker_log_h
#define tracker_log_h

#include <unordered_map>
#include <vector>
#include <fstream>

namespace OT {
    struct Track {
        int trackerId;
        int x;
        int y;
        long frameNumber;
    };
    
    class TrackerLog {
    private:
        // Get the tracks associated with a tracker given its tracker id.
        std::unordered_map<int, std::vector<Track>> tracksForTrackerId;
        
        // Given a tracker ID, return the first frame that the tracker appears in.
        std::unordered_map<int, long> birthFrameForTrackerId;
        
        // The maximum frame seen in any track so far.
        long numFrames;
        
        // Whether the JSON output should be compressed (i.e. no spaces, no keys for tracks).
        bool compress;
    public:
        TrackerLog(bool compress = false);
        
        // Update the tracker log with the latest info for some tracker.
        void addTrack(int trackerId, int x, int y, long frameNumber);
        
        // Output the log to the given file as JSON.
        void logToFile(std::ofstream& outputStream);
    };
}


#endif /* tracker_log_h */
