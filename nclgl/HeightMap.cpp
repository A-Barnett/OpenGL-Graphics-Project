#include "HeightMap.h"
#include <iostream>
#include <fstream>

HeightMap::HeightMap(const std::string& name) {
    int iWidth, iHeight, iChans;
    data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return;
    }
    numVertices = iWidth * iHeight;
    numIndices = (iWidth - 1) * (iHeight - 1) * 6;
    vertices = new Vector3[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];
    Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
    Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

    for (int z = 0; z < iHeight; ++z) {
        for (int x = 0; x < iWidth; ++x) {
            int offset = (z * iWidth) + x;
            vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
            textureCoords[offset] = Vector2(x, z) * textureScale;
        }
    }
    int i = 0;

    for (int z = 0; z < iHeight - 1; ++z) {
        for (int x = 0; x < iWidth - 1; ++x) {
            int a = (z * iWidth) + x;
            int b = (z * iWidth) + (x + 1);
            int c = ((z + 1) * iWidth) + (x + 1);
            int d = ((z + 1) * iWidth) + x;

            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;

            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;
        }
    }
    GenerateNormals();
    GenerateTangents();
    BufferData();

    heightmapSize.x = vertexScale.x * (iWidth - 1);
    heightmapSize.y = vertexScale.y * 255.0f; // each height is a byte!
    heightmapSize.z = vertexScale.z * (iHeight - 1);

}


void HeightMap::UpdateHeights(float time, float scrollSpeed, const std::string& name) {
    if (!data) {
        std::cout << "Failed to load heightmap data!\n";
        return;
    }

    Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
    int jWidth = heightmapSize.x / vertexScale.x;
    int jHeight = heightmapSize.z / vertexScale.z;

    float offset = fmod(time * scrollSpeed, jWidth); // Get the scrolling offset
    int intOffset = (int)offset;                     // Integer part for base index
    float fracOffset = offset - intOffset;           // Fractional part for interpolation

    for (int z = 0; z < jHeight; ++z) {
        for (int x = 0; x < jWidth; ++x) {
            // Calculate the current index and the next index (for interpolation)
            int baseIndex = z * jWidth + ((x + intOffset) % jWidth);
            int nextIndex = z * jWidth + ((x + intOffset + 1) % jWidth);

            // Linearly interpolate between base and next height values
            float currentHeight = vertexScale.y * data[baseIndex];
            float nextHeight = vertexScale.y * data[nextIndex];
            vertices[z * jWidth + x].y = currentHeight * (1.0f - fracOffset) + nextHeight * fracOffset;
        }
    }
    BufferData();                // Re-upload the updated vertex data to the GPU
}