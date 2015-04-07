#include "actorselector.h"

#include "actorfactory.h"
#include "resourcemanager.h"

ActorSelector::ActorSelector(int font_size, vector2d pos,
                             vector2d size)
	:m_font(NULL),
	 m_font_size(font_size),
	 m_pos(pos),
	 m_size(size),
	 m_actors(),
	 m_cur_actor(0),
	 m_first_actor(0)
{
	ResourceManager *rm = ResourceManager::get_instance();
	m_font = rm->get_font("freesansbold.ttf", m_font_size);
    
	ActorFactory *af = ActorFactory::get_instance();
	af->get_actors(&m_actors);
}

ActorSelector::~ActorSelector()
{
}

void ActorSelector::mouse_event(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT)
	{
		Rect dims(m_pos[0], m_pos[1], m_size[0], m_size[1]);
		if (dims.contains(event.x, event.y))
		{
			int y = event.y - m_pos[1];
			m_cur_actor = m_first_actor + y / m_font_size;
		}
	}
	else if (event.button == SDL_BUTTON_WHEELUP &&
	         event.type == SDL_MOUSEBUTTONUP)
		m_cur_actor--;
	else if (event.button == SDL_BUTTON_WHEELDOWN &&
	         event.type == SDL_MOUSEBUTTONUP)
		m_cur_actor++;
    
	m_cur_actor = max(m_cur_actor, 0);
	m_cur_actor = min(m_cur_actor, (int)(m_actors.size() - 1));
}

void ActorSelector::draw(Surface *dst) const
{
	Rect bounds(m_pos[0], m_pos[1], m_size[0], m_size[1]);
	dst->draw_rect(mapRGB(0, 0, 0), bounds);
	int num_rows = min((int)m_actors.size(),
	                   (int)(m_size[1] / m_font_size));
	for (int i = 0; i < num_rows; ++i)
	{
		int actor_i = m_first_actor + i;
		int actor_x = m_pos[0];
		int actor_y = m_pos[1] + m_font_size * actor_i;
		string actor = m_actors[actor_i];
		if (actor_i == m_cur_actor)
		{
			Rect text_bounds(actor_x, actor_y, m_size[0], m_font_size);
			dst->draw_rect(mapRGB(255, 255, 127), text_bounds);
		}
		dst->draw_string(m_font, actor.c_str(),
		                 mapRGB(255, 255, 255),
		                 actor_x, actor_y);
	}
	if (num_rows > m_size[1] / m_font_size)
	{
		Rect bar_dims(m_pos[0] + m_size[0] - m_font_size,
		              m_pos[1], m_font_size,
		              m_size[1] * ((1.0 * num_rows) / m_actors.size()));
		dst->draw_rect(mapRGB(255, 255, 255), bar_dims);
	}
}

string ActorSelector::get_selection() const
{
	return m_actors[m_cur_actor];
}
