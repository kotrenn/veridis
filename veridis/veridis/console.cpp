#include "console.h"

#include "resourcemanager.h"

Console::Console(int max_lines)
	:m_max_lines(max_lines),
	 m_lines(),
	 m_input(""),
	 m_visible(false),
	 m_font(NULL),
	 m_font_size(12)
{
	ResourceManager *rm = ResourceManager::get_instance();
	m_font = rm->get_font("freesansbold.ttf", m_font_size);
}

Console::~Console()
{
}

void Console::key_pressed(SDLKey key, SDLMod mod)
{
	if (key == SDLK_BACKSPACE)
	{
		if (m_input.size() > 0)
			m_input.erase(m_input.end() - 1);
	}
	else if (key == SDLK_SPACE)
	{
		m_input += ' ';
	}
	else
	{
		char *value = SDL_GetKeyName(key);
		if (strlen(value) == 1)
		{
			char x = value[0];
			bool special_char = false;
			char special[] = ",./;\'[]\\-=";
			for (unsigned int i = 0; i < strlen(special); ++i)
				if (special[i] == x)
					special_char = true;
			if ((x >= 'a' && x <= 'z') ||
			    (x >= '0' && x <= '9') ||
			    special_char)
			{
				if (mod & (KMOD_CAPS | KMOD_LSHIFT | KMOD_RSHIFT))
				{
					if (x >= 'a' && x <= 'z')
						x = (x - 'a') + 'A';
					else if (x >= '0' && x <= '9')
					{
						char up[] = ")!@#$%^&*(";
						x = up[x - '0'];
					}
					else if (special_char)
					{
						char up[] = ",<.>/?\\|[{]}-_=+;:\'\"";
						for (unsigned int i = 0; i < strlen(up); i += 2)
						{
							if (up[i] == x)
								x = up[i + 1];
						}
					}
				}
				m_input += x;
			}
		}
	}
}

void Console::new_line()
{
	if (m_input.size() == 0)
		return;
	add_line(m_input.c_str());
	m_input.clear();
}

void Console::add_line(const char *str)
{
	m_lines.push_back(string(str));
	while (m_lines.size() >= m_max_lines)
		m_lines.pop_front();
}

void Console::draw(Surface *dst) const
{
	if (m_visible == false)
		return;

	Uint32 bg_color = mapRGB(0, 0, 0);
	Uint32 text_color = mapRGB(0, 255, 0);
	int cur_line = 0;
	int base_x = 5;
	int base_y = 5;
	Rect dims(0, 0, dst->get_w(), 2 * base_y + m_font_size * m_max_lines);
	dst->draw_rect(bg_color, dims);
	list<string>::const_iterator it;
	for (it = m_lines.begin(); it != m_lines.end(); it++)
	{
		dst->draw_string(m_font, (*it).c_str(), text_color,
		                 base_x, base_y + m_font_size * cur_line);
		cur_line++;
	}
	dst->draw_string(m_font, m_input.c_str(), text_color,
	                 base_x, base_y + m_font_size * cur_line);
}
