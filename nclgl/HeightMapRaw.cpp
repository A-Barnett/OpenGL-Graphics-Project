#include "HeightMapRaw.h"


HeightMapRaw::HeightMapRaw(const std::string& filename, int width, int height) {
    // Open the .raw file in binary mode
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "HeightMap can't load file!\n";
        return;
    }

    // Read the raw height data as 16-bit values
    unsigned short* data = new unsigned short[width * height];
    file.read(reinterpret_cast<char*>(data), width * height * sizeof(unsigned short));
    file.close();

    numVertices = width * height;
    numIndices = (width - 1) * (height - 1) * 6;
    vertices = new Vector3[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];

    Vector3 vertexScale = Vector3(16.0f, 5000.0f, 16.0f); // Adjust height scale as needed
    Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

    // Generate vertices and texture coordinates
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            int offset = (z * width) + x;
            float heightValue = static_cast<float>(data[offset]) / 65535.0f; // Normalize 16-bit value
            if (heightValue < 0.005f) {
                heightValue = -1;
            }
            vertices[offset] = Vector3(x, heightValue, z) * vertexScale;
            textureCoords[offset] = Vector2(x, z) * textureScale;
        }
    }

    // Generate indices for triangle strips
    int i = 0;
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int a = (z * width) + x;
            int b = (z * width) + (x + 1);
            int c = ((z + 1) * width) + (x + 1);
            int d = ((z + 1) * width) + x;

            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;

            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;
        }
    }

    // Clean up and generate additional data
    delete[] data;
    GenerateNormals();
    GenerateTangents();
    BufferData();

    heightmapSize.x = vertexScale.x * (width - 1);
    heightmapSize.y = vertexScale.y * 65535.0f; // Max height based on 16-bit value
    heightmapSize.z = vertexScale.z * (height - 1);
}
