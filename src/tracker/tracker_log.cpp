#include "tracker/tracker_log.hpp"

#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>

#include "lib/json.hpp"

namespace OT {
    TrackerLog::TrackerLog(bool compress) {
        this->tracksForTrackerId = std::unordered_map<int, std::vector<OT::Track>>();
        this->numFrames = 0;
        this->birthFrameForTrackerId = std::unordered_map<int, long>();
        this->compress = compress;
    }
    
    void TrackerLog::addTrack(int trackerId, int x, int y, long frameNumber) {
        // Add the track.
        if (this->tracksForTrackerId.find(trackerId) == this->tracksForTrackerId.end()) {
            this->tracksForTrackerId[trackerId] = std::vector<OT::Track>();
        }
        this->tracksForTrackerId[trackerId].push_back(OT::Track{trackerId, x, y, frameNumber});
        
        // Update the birth frame.
        if (this->birthFrameForTrackerId.find(trackerId) == this->birthFrameForTrackerId.end()) {
            this->birthFrameForTrackerId[trackerId] = frameNumber;
        }
        this->birthFrameForTrackerId[trackerId] = std::min(this->birthFrameForTrackerId[trackerId], frameNumber);
        
        // Update the number of frames.
        this->numFrames = std::max(this->numFrames, frameNumber);
    }
    
    void TrackerLog::logToFile(std::ofstream& outputStream) {
        // Sort the trackers by birth frame number.
        nlohmann::json json;
        json["numFrames"] = this->numFrames;
        json["width"] = this->width;
        json["height"] = this->height;
        
        
        // Create a vector of pairs.
        std::vector<std::pair<int, long>> idBirthPairs;
        
        for (auto pair : this->birthFrameForTrackerId) {
            idBirthPairs.push_back(pair);
        }
        
        // Sort the vector.
        auto cmp = [](std::pair<int,long> const & a, std::pair<int,long> const & b)
        {
            return a.second != b.second?  a.second < b.second : a.first < b.first;
        };
        std::sort(idBirthPairs.begin(), idBirthPairs.end(), cmp);
        
        // Now iterate through each tracker, in order of birth.
        for (size_t i = 0; i < idBirthPairs.size(); i++) {
            json["trackers"][i]["birth"] = idBirthPairs[i].second;
            json["trackers"][i]["trackerId"] = idBirthPairs[i].first;
            
            // Iterate through each track for the tracker.
            for (auto track : this->tracksForTrackerId[idBirthPairs[i].first]) {
                if (this->compress) {
                    json["trackers"][i]["track"].push_back({track.x, track.y, track.frameNumber});
                } else {
                    json["trackers"][i]["track"].push_back({{"x", track.x}, {"y", track.y}, {"frame", track.frameNumber}});
                }
            }
        }
        
        auto output = this->compress ? json.dump() : json.dump(2);
        outputStream << output << std::endl;
    }
    
    void TrackerLog::setDimensions(int width, int height) {
        this->width = width;
        this->height = height;
    }
}
