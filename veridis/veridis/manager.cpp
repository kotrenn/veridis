#include "manager.h"
#include "actorfactory.h"
#include "datamanager.h"
#include "resourcemanager.h"

AbstractManager::AbstractManager()
{
}

AbstractManager::~AbstractManager()
{
}



DecoratorManager::DecoratorManager(AbstractManager *inner)
    :AbstractManager(),
     m_inner(inner)
{
}

DecoratorManager::~DecoratorManager()
{
    delete m_inner;
}



Manager::Manager()
    :AbstractManager(),
     m_level(NULL),
     m_camera(NULL),
     m_paused(false),
     m_player(NULL),
     m_prev_move(SDL_GetTicks()),
     m_pause_ticks(0)
{
    DataManager *dm = DataManager::get_instance();
    int start_x = dm->get_int("start_x");
    int start_y = dm->get_int("start_y");
    m_level = new Level(start_x, start_y);
    vector2d size = dm->get_vec("viewport");
    m_camera = new Camera(size[0], size[1],
			  m_level->get_w(), m_level->get_h());

    m_player = new PlayerActor(m_level);
    m_level->add_actor(m_player);
    m_camera->set_target(m_player);

    ResourceManager *rm = ResourceManager::get_instance();
    rm->play_music("music1.mp3");
}

Manager::~Manager()
{
    delete m_level;
    delete m_camera;
    delete m_player;
}

void Manager::key_pressed(SDLKey key, SDLMod)
{
    if (key == SDLK_SPACE)
    {
	new TripleShot(m_level);
	new RadialShot(m_level);
    }
    else if (key == SDLK_p)
    {
	if (m_paused)
	    unpause();
	else
	    pause();
    }
}

void Manager::mouse_event(SDL_MouseButtonEvent)
{
}

void Manager::pause()
{
    m_paused = true;
    m_level->pause();
    m_pause_ticks = SDL_GetTicks() - m_prev_move;
}

void Manager::unpause()
{
    m_paused = false;
    m_level->unpause();
    m_prev_move = SDL_GetTicks() - m_pause_ticks;
    m_pause_ticks = 0;
}

void Manager::update()
{
    if (m_paused)
	return;

    // update objects
    m_level->update();

    // move objects
    Uint32 cur_time = SDL_GetTicks();
    m_level->move((cur_time - m_prev_move) / 1000.0);
    m_prev_move = cur_time;

    if (m_player != NULL)
    {
	vector2d pos = m_player->get_center();
	int x = pos[0];
	int y = pos[1];
	int dx = 0;
	int dy = 0;
	if (x < 0)                dx = -1;
	if (x > m_level->get_w()) dx =  1;
	if (y < 0)                dy = -1;
	if (y > m_level->get_h()) dy =  1;
	if (abs(dx) > 0 || abs(dy) > 0)
	{
	    int map_x = m_level->get_x() + dx;
	    int map_y = m_level->get_y() + dy;
	    char filename[80];
	    snprintf(filename, 80, "map_%d_%d.dat", map_x, map_y);
	    DATFile in;
	    int ret = in.open(filename, "r");
	    if (ret >= 0)
	    {
		in.close();
		set_level(new Level(map_x, map_y));
		m_player->set_center(
		    vector2d(x - dx * m_level->get_w(),
			     y - dy * m_level->get_h()));
	    }
	}
	
	if (m_player->get_health() <= 0)
	{
	    delete m_player;
	    m_player = NULL;
	    m_camera->set_target(NULL);
	}
    }
    
    m_camera->move();
}

void Manager::draw(Surface *dst) const
{
    m_camera->clear();
    m_level->draw(m_camera);
    m_camera->draw(dst, 0, 0);

    if (m_paused)
    {
	Uint32 white = mapRGB(255, 255, 255);
	int pause_x = 200;
	int pause_y = 200;
	int text_h = 20;
	int cur_i = 0;
	dst->draw_string("--Paused--", white,
			 pause_x, pause_y + text_h * cur_i++);
	dst->draw_string("", white,
			 pause_x, pause_y + text_h * cur_i++);
	dst->draw_string("Options:", white,
			 pause_x, pause_y + text_h * cur_i++);
	dst->draw_string("[ESC] Quit Game", white,
			 pause_x, pause_y + text_h * cur_i++);
    }
}

void Manager::set_level(Level *level)
{
    delete m_level;
    m_level = level;
    m_player->set_level(level);
}
