#ifndef TILE__H
#define TILE__H

#include "actor.h"
#include "camera.h"

class Actor;
class Tile;

class TileFactory
{
public:
    static TileFactory *get_instance();
    ~TileFactory();

    Tile build_tile(int x, int y) const;
    bool is_wall(int x, int y) const
    { return m_walls[y * m_tile_w + x]; }
private:
    TileFactory();
    static TileFactory *m_instance;

    int m_tile_w;
    int m_tile_h;
    bool *m_walls;
};

class Tile
{
public:
    Tile(int x, int y, bool wall);
    Tile(DATFile *in);

    void save(DATFile *out) const;

    void fix(Actor *actor, int x, int y,
	     int tile_w, int tile_h) const;
    //void fix(Item *item) const;

    void draw(Camera *camera, int x, int y,
	      int tile_w, int tile_h) const;

    int get_x() const { return m_x; }
    int get_y() const { return m_y; }
    bool is_wall() const { return m_wall; }
    void set_x(int x);
    void set_y(int y);
private:
    static Surface *m_sheet;

    void update_wall();
    
    /* x and y position on tile sheet */
    int m_x;
    int m_y;
    bool m_wall;
};

#endif
