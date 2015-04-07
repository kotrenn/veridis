#include "editormanager.h"

#include "datamanager.h"
#include "resourcemanager.h"

EditorManager::EditorManager(AbstractManager *inner)
    :DecoratorManager(inner),
     m_editing(false),
     m_state(EM_TILE),
     m_sheet(NULL),
     m_camera_w(0), m_camera_h(0),
     m_tile_x(0), m_tile_y(0),
     m_selector(NULL),
     m_font(NULL),
     m_font_size(12)
{
    ResourceManager *rm = ResourceManager::get_instance();
    m_sheet = rm->get_image("tiles.png");

    Camera *camera = m_inner->get_camera();
    vector2d size(m_sheet->get_w(),
		  camera->get_h() - m_sheet->get_h());
    vector2d pos(camera->get_w() - size[0],
		 camera->get_h() - size[1]);
    m_selector = new ActorSelector(12, pos, size);

    m_font = rm->get_font("freesansbold.ttf", m_font_size);
}

EditorManager::~EditorManager()
{
    delete m_selector;
}

void EditorManager::key_pressed(SDLKey key, SDLMod mod)
{
    DecoratorManager::key_pressed(key, mod);

    if (key == SDLK_BACKSPACE)
    {
	m_editing = m_editing == false;
	Camera *camera = m_inner->get_camera();
	if (m_editing)
	{
	    m_camera_w = camera->get_w();
	    m_camera_h = camera->get_h();
	    camera->set_w(m_camera_w - m_sheet->get_w());
	}
	else
	{
	    camera->set_w(m_camera_w);
	    camera->set_h(m_camera_h);
	}
    }
    else if (key == SDLK_1)
	m_state = EM_ACTOR;
    else if (key == SDLK_2)
	m_state = EM_TILE;
    else if (key == SDLK_w || key == SDLK_s ||
   	     key == SDLK_a || key == SDLK_d)
    {
	int dx = 0;
	int dy = 0;
	if (key == SDLK_w) dy = -1;
	if (key == SDLK_s) dy =  1;
	if (key == SDLK_a) dx = -1;
	if (key == SDLK_d) dx =  1;
	int x = get_level()->get_x() + dx;
	int y = get_level()->get_y() + dy;
	char filename[80];
	snprintf(filename, 80, "map_%d_%d.dat", x, y);
	DATFile in;
	int ret = in.open(filename, "r");
	if (ret < 0)
	{
	    Level *level = new Level();
	    level->set_x(x);
	    level->set_y(y);
	    m_inner->set_level(level);
	}
	else
	{
	    in.close();
	    m_inner->set_level(new Level(x, y));
	}
    }
    else if (key == SDLK_f)
    {
	Level *level = m_inner->get_level();
	for (int x = 0; x < level->get_tile_w(); ++x)
	    for (int y = 0; y < level->get_tile_h(); ++y)
		level->set_tile(x, y, m_tile_x, m_tile_y);
    }
    
    if (!m_editing)
	return;
}

void EditorManager::mouse_event(SDL_MouseButtonEvent event)
{
    DecoratorManager::mouse_event(event);

    m_selector->mouse_event(event);

    Camera *camera = m_inner->get_camera();
    Level *level = m_inner->get_level();
    
    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    int tile_x = (event.x + camera->get_x()) / tile_w;
    int tile_y = (event.y + camera->get_y()) / tile_h;

    if (event.type == SDL_MOUSEBUTTONUP)
    {
	if (event.button == SDL_BUTTON_LEFT)
	{
	    if (m_state == EM_ACTOR)
	    {
		string actor = m_selector->get_selection();
		level->set_spawn(tile_x, tile_y, actor);
	    }
	}
	else if (event.button == SDL_BUTTON_RIGHT)
	    if (m_state == EM_ACTOR)
		level->remove_spawn(tile_x, tile_y);
    }
}

void EditorManager::update()
{
    DecoratorManager::update();

    if (!m_editing)
	return;

    Camera *camera = m_inner->get_camera();
    
    int mouse_x;
    int mouse_y;
    Uint8 mouse = SDL_GetMouseState(&mouse_x, &mouse_y);
    
    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    int tile_x = (mouse_x + camera->get_x()) / tile_w;
    int tile_y = (mouse_y + camera->get_y()) / tile_h;

    if (mouse & SDL_BUTTON(1))
    {
	if (mouse_x > camera->get_w() &&
	    mouse_y < m_sheet->get_h())
	{
	    m_tile_x = (mouse_x - camera->get_w()) / tile_w;
	    m_tile_y = mouse_y / tile_h;
	}
	else if (m_state == EM_TILE)
	{
	    Level *level = m_inner->get_level();
	    level->set_tile(tile_x, tile_y, m_tile_x, m_tile_y);
	}
    }
}

void EditorManager::draw(Surface *dst) const
{
    DecoratorManager::draw(dst);

    if (!m_editing)
	return;

    Camera *camera = m_inner->get_camera();
    Level *level = m_inner->get_level();

    level->draw_spawn(dst, camera);

    int mouse_x;
    int mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    
    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    int tile_x = (mouse_x + camera->get_x()) / tile_w;
    int tile_y = (mouse_y + camera->get_y()) / tile_h;

    string actor = level->get_spawn(tile_x, tile_y);
    if (actor.size() > 0)
    {
	int actor_x = mouse_x;
	int actor_y = mouse_y - m_font_size;
	dst->draw_string(m_font, actor.c_str(),
			 mapRGB(255, 255, 255),
			 actor_x, actor_y);
    }

    char state[80];
    snprintf(state, 80, "Editing %s",
	     (m_state == EM_TILE)? "tiles" : "actors");
    dst->draw_string(state, mapRGB(255, 255, 255),
		     5, camera->get_h() - 45);

    char pos[80];
    snprintf(pos, 80, "Map [%d, %d]", level->get_x(), level->get_y());
    dst->draw_string(pos, mapRGB(255, 255, 255),
		     5, camera->get_h() - 65);

    int sheet_x = dst->get_w() - m_sheet->get_w();
    int sheet_y = 0;
    Rect bounds(sheet_x, sheet_y, m_sheet->get_w(), dst->get_h());
    dst->draw_rect(mapRGB(0, 0, 255), bounds);
    dst->blit(m_sheet, sheet_x, 0);

    int border = 2;
    Rect tile_bounds(sheet_x + tile_w * m_tile_x - border,
		     sheet_y + tile_h * m_tile_y - border,
		     tile_w + 2 * border, tile_h + 2 * border);
    dst->draw_rect(mapRGB(255, 255, 255), tile_bounds, border);

    m_selector->draw(dst);
}
