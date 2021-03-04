#ifndef TREASURE_H
#define TREASURE_H
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <unistd.h>

class sparse {
public:
    int x;
    int y;
    bool operator<(const sparse &other) const {
        if (x < other.x) return true;
        if (other.x < x) return false;
        return y < other.y;
    }
};
    std::map<sparse,int> treasureEmpty;
    sparse treasureCoord;
#endif
