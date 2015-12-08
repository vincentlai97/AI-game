#ifndef TILEMAP_H
#define TILEMAP_H

#include <string>
#include <fstream>
#include <sstream>

class TileMap
{
private:
	float m_tileSize;
	unsigned m_mapWidth;
	unsigned m_mapHeight;
	unsigned m_numOfTileWidth;
	unsigned m_numOfTileHeight;

	int **m_tileMap;
public:
	TileMap();
	~TileMap();

	void Init(const float& tileSize, const unsigned& mapWidth, const unsigned& mapHeight, const unsigned& numOfTileWidth, const unsigned& numOfTileHeight);
	bool LoadMap(const std::string& mapName);

	int getTile(unsigned col, unsigned row) const;

	unsigned getMapWidth() const;
	unsigned getMapHeight() const;

	unsigned getNumOfTilesWidth() const;
	unsigned getNumOfTilesHeight() const;

};

#endif