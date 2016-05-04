#include "ground_truth_log.hpp"

#include <vector>

namespace OT {
    namespace GroundTruth {
        Log::Log() {
            this->log = std::vector<OT::GroundTruth::Annotation>();
        }
        
        void Log::addAnnotation(OT::GroundTruth::Annotation annotation) {
            this->log.push_back(annotation);
        }
        
        void Log::writeToStream(std::ofstream& outputStream) {
            for (auto annotation : this->log) {
                outputStream << annotation.frame << "," << annotation.x << "," << annotation.y << std::endl;
            }
        }
    }
}