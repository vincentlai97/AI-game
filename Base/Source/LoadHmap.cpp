#include <iostream>
#include <fstream>
#include "LoadHmap.h"

bool LoadHeightMap(const char *file_path, std::vector<unsigned char> &heightMap)
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if(!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}

    fileStream.seekg(0, std::ios::end);
    std::streampos fsize = (unsigned)fileStream.tellg();
	
	fileStream.seekg(0, std::ios::beg);
	heightMap.resize((unsigned)fsize);
	fileStream.read((char *)&heightMap[0], fsize);
	
	fileStream.close();
	return true;
}

float ReadHeightMap(std::vector<unsigned char> &heightMap, float x, float z)
{
	if (x < -0.5f || x > 0.5f || z < -0.5f || z > 0.5f)
		return 0;
	if (heightMap.size() == 0)
		return 0;

	int mapSize = sqrt((float)heightMap.size());

	unsigned _x = (x + 0.5f) * mapSize;
	unsigned _z = (z + 0.5f) * mapSize;

	return heightMap[_x + _z*mapSize] / 256.f;
}