#pragma once
#include <string>
#include "mesh.h"

class HeightMapRaw : public Mesh {
public:
    HeightMapRaw(const std::string& filename, int width, int height);
    ~HeightMapRaw() { };
    Vector3 GetHeightmapSize() const { return heightmapSize; }
protected:
    Vector3 heightmapSize;
    unsigned char* data;
};
