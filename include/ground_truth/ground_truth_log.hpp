#ifndef ground_truth_log_h
#define ground_truth_log_h

#include <vector>
#include <fstream>

namespace OT {
    namespace GroundTruth {
        struct Annotation {
            long frame;
            int x;
            int y;
        };
        
        class Log {
        private:
            // The array of log entries.
            std::vector<Annotation> log;
        public:
            Log();
            
            // Add an annotation to this log.
            void addAnnotation(Annotation annotation);
            
            // Write the annotations to a stream.
            void writeToStream(std::ofstream& outputStream);
        };
    }
}


#endif /* ground_truth_log_h */
