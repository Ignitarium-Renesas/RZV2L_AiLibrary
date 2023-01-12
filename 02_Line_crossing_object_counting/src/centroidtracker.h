#ifndef C___CENTROIDTRACKER_H
#define C___CENTROIDTRACKER_H

#endif /* C___CENTROIDTRACKER_H */

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <set>
#include <algorithm>

class CentroidTracker {
    public:
        std::vector<std::pair<int, std::pair<int, int>>> update(std::vector<std::vector<int>> boxes);
        std::vector<std::pair<int, std::pair<int, int>>> objects;
        std::map<int, std::vector<std::pair<int, int>>> path_keeper;

        explicit CentroidTracker(int maxDisappear, int maxDist);
        void register_Object(int cX, int cY);

    private:
        int maxDisappear;
        int maxDist;
        int nextObjID;
        std::map<int, int> disappeared;

        static double calcDistance(double x1, double y1, double x2, double y2);  
};
