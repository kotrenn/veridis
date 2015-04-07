#include "scene.h"
#include "datamanager.h"
#include "resourcemanager.h"

Scene::Scene()
    : m_backgrounds(),
      m_x(0), m_y(0),
      m_w(0), m_h(0),
      m_view_w(0), m_view_h(0)
{
    DataManager *dm = DataManager::get_instance();
    
    vector2d size = dm->get_vec("size");
    m_w = size[0];
    m_h = size[1];

    vector2d view = dm->get_vec("viewport");
    m_w = view[0];
    m_h = view[1];
    
    ResourceManager *rm = ResourceManager::get_instance();
    int num_bgs = dm->get_int("num_bgs");
    char filename[80];
    char var[10];
    for (int i = 0; i < num_bgs; ++i)
    {
	snprintf(var, 10, "bg%d", i + 1);
	dm->get_str(var, filename);
	m_backgrounds.push_back(rm->get_image(filename));
    }

    Surface *bg = m_backgrounds.front();
    m_w = max(bg->get_w(), m_w);
    m_h = max(bg->get_h(), m_h);
    m_view_w = max(bg->get_w(), m_view_w);
    m_view_h = max(bg->get_h(), m_view_h);
}

Scene::~Scene()
{
}

void Scene::update()
{
    if (m_x < 0)
	m_x = 0;
    if (m_y < 0)
	m_y = 0;
    if (m_x + m_view_w > m_w)
	m_x = m_w - m_view_w;
    if (m_y + m_view_h > m_h)
	m_y = m_h - m_view_h;
}

void Scene::draw(Surface *dst) const
{
    list<Surface *>::const_iterator it;
    for (it = m_backgrounds.begin(); it != m_backgrounds.end(); it++)
    {
	Surface *bg = *it;
	int base_x = m_x % bg->get_w();
	int base_y = m_y % bg->get_h();
	int max_x = 1 + dst->get_w() / bg->get_w();
	int max_y = 1 + dst->get_h() / bg->get_h();
	for (int x = 0; x <= max_x; ++x)
	    for (int y = 0; y <= max_y; ++y)
		dst->blit(bg,
			  -base_x + x * bg->get_w(),
			  -base_y + y * bg->get_h());
    }
}

const vector2d Scene::get_pos() const
{
    vector2d ret(m_x, m_y);
    return ret;
}

const vector2d Scene::get_size() const
{
    vector2d ret(m_w, m_h);
    return ret;
}

void Scene::set_pos(const vector2d &pos)
{
    m_x = pos[0];
    m_y = pos[1];
}
