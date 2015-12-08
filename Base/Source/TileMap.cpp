#include "TileMap.h"

TileMap::TileMap()
: m_tileSize(0)
, m_mapHeight(0)
, m_mapWidth(0)
, m_numOfTileHeight(0)
, m_numOfTileWidth(0)
{
}

TileMap::~TileMap()
{
}

void TileMap::Init(const float& tileSize, const unsigned& mapWidth, const unsigned& mapHeight, const unsigned& numOfTileWidth, const unsigned& numOfTileHeight)
{
	m_tileSize = tileSize;
	m_mapWidth = mapWidth;
	m_mapHeight = mapHeight;
	m_numOfTileWidth = numOfTileWidth;
	m_numOfTileHeight = numOfTileHeight;

	m_tileMap = new int*[m_mapWidth];
	for (int count = 0; count < m_mapWidth; ++count)
		m_tileMap[count] = new int[m_mapHeight];
}

bool TileMap::LoadMap(const std::string& mapName)
{
	std::ifstream file(mapName.c_str());
	if (file.is_open())
	{
		int lineCount = 0;
		while (file.good())
		{
			if (lineCount >= m_mapHeight) break;

			std::string line = "";
			std::getline(file, line);

			std::string token = "";
			std::istringstream iss(line);

			int columnCount = 0;
			while (std::getline(iss, token, ','))
			{
				m_tileMap[columnCount++][m_mapHeight - lineCount - 1] = atoi(token.c_str());
				if (columnCount > m_mapWidth) break;
			}
			++lineCount;

			if (columnCount > m_mapWidth) return false;
		}
	}
	else return false;
	return true;
}

int TileMap::getTile(unsigned col, unsigned row) const
{
	if (col < 0) col = 0;
	if (row < 0) row = 0;
	if (col >= m_mapWidth) col = m_mapWidth - 1;
	if (row >= m_mapHeight) row = m_mapHeight - 1;
	return m_tileMap[col][row];
}

unsigned TileMap::getMapWidth() const
{
	return m_mapWidth;
}

unsigned TileMap::getMapHeight() const
{
	return m_mapHeight;
}

unsigned TileMap::getNumOfTilesWidth() const
{
	return m_numOfTileWidth;
}

unsigned TileMap::getNumOfTilesHeight() const
{
	return m_numOfTileHeight;
}