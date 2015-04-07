#include "tile.h"

#include "datamanager.h"
#include "resourcemanager.h"

TileFactory *TileFactory::m_instance = NULL;

TileFactory *TileFactory::get_instance()
{
	if (m_instance == NULL)
		m_instance = new TileFactory();
	return m_instance;
}

TileFactory::TileFactory()
	:m_tile_w(0),
	 m_tile_h(0),
	 m_walls(NULL)
{
	ResourceManager *rm = ResourceManager::get_instance();
	Surface *sheet = rm->get_image("tiles.png");
    
	DataManager *dm = DataManager::get_instance();
	m_tile_w = sheet->get_w() / dm->get_int("tile_w");
	m_tile_h = sheet->get_h() / dm->get_int("tile_h");
	m_walls = (bool *)malloc(sizeof(bool) * m_tile_w * m_tile_h);

	DATFile walls;
	walls.open("walls.dat", "r");
	for (int r = 0; r < m_tile_h; ++r)
	{
		string row;
		walls.read_str(&row);
		for (int c = 0; c < m_tile_w; ++c)
			m_walls[r * m_tile_w + c] = row[c] == '1';
	}
	walls.close();
}

TileFactory::~TileFactory()
{
	free(m_walls);
}

Tile TileFactory::build_tile(int x, int y) const
{
	return Tile(x, y, is_wall(x, y));
}



Surface *Tile::m_sheet = NULL;

Tile::Tile(int x, int y, bool wall)
	:m_x(x),
	 m_y(y),
	 m_wall(wall)
{
	if (m_sheet == NULL)
	{
		ResourceManager *rm = ResourceManager::get_instance();
		m_sheet = rm->get_image("tiles.png");
	}
}

Tile::Tile(DATFile *in)
	:m_x(0),
	 m_y(0),
	 m_wall(false)
{
	if (m_sheet == NULL)
	{
		ResourceManager *rm = ResourceManager::get_instance();
		m_sheet = rm->get_image("tiles.png");
	}
    
	in->read_int(&m_x);
	in->read_int(&m_y);
	in->read_bool(&m_wall);
}

void Tile::save(DATFile *out) const
{
	out->write_int(m_x);
	out->write_int(m_y);
	out->write_bool(m_wall);
}

void Tile::fix(Actor *actor, int x, int y,
               int tile_w, int tile_h) const
{
	if (!m_wall)
		return;
	vector2d v= actor->get_v();
	Circle circle = actor->get_circle();
	Line line[] = { Line(x, y, x + tile_w - 1, y),
	                Line(x + tile_w - 1, y,
	                     x + tile_w - 1, y + tile_h - 1),
	                Line(x + tile_w - 1, y + tile_h - 1,
	                     x, y + tile_h - 1),
	                Line(x, y + tile_h - 1, x, y) };
	int axis[] = { 1, 0, 1, 0 };
	vector2d dir[] = { vector2d(0, -1),
	                   vector2d(1, 0),
	                   vector2d(0, 1),
	                   vector2d(-1, 0) };
	bool adjusted = false;
	for (int i = 0; i < 4; ++i)
	{
		if (line[i].collides(&circle))
		{
			adjusted = true;
			v[axis[i]] = 0;
			circle[axis[i]] = line[i][axis[i]] +
				dir[i][axis[i]] * circle[2];
		}
	}
	if (adjusted)
	{
		actor->set_v(v);
		actor->set_center(vector2d(circle[0], circle[1]));
	}
}

void Tile::draw(Camera *camera, int x, int y,
                int tile_w, int tile_h) const
{
	Surface *dst = camera->get_surface();
	int draw_x = x * tile_w - camera->get_x();
	int draw_y = y * tile_h - camera->get_y();
	dst->blit(m_sheet, draw_x, draw_y,
	          Rect(m_x * tile_w, m_y * tile_h,
	               tile_w, tile_h));

	/*
	  if (m_wall)
	  {
	  Rect bounds(draw_x, draw_y, tile_w, tile_h);
	  dst->draw_rect(mapRGBA(255, 0, 255, 127), &bounds);
	  }
	*/
}

void Tile::set_x(int x)
{
	if (m_x == x)
		return;
    
	m_x = x;
	m_wall = TileFactory::get_instance()->is_wall(m_x, m_y);
}

void Tile::set_y(int y)
{
	if (m_y == y)
		return;

	m_y = y;
	m_wall = TileFactory::get_instance()->is_wall(m_x, m_y);
}
