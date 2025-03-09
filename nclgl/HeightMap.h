#pragma once
#include <string>
#include "mesh.h"

class HeightMap : public Mesh {
public:
    HeightMap(const std::string& name);
    ~HeightMap() { SOIL_free_image_data(data); };
    void UpdateHeights(float time, float scrollSpeed, const std::string& name);
    Vector3 GetHeightmapSize() const { return heightmapSize; }
protected:
    Vector3 heightmapSize;
    unsigned char* data;
};
