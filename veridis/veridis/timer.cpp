#include "timer.h"

Timer::Timer(double time)
    :m_start(SDL_GetTicks()),
     m_pause(0),
     m_time(time),
     m_paused(false)
{
}

Timer::Timer(DATFile *in)
    :m_start(0),
     m_pause(0),
     m_time(0),
     m_paused(false)
{
    int ticks;
    in->read_int(&ticks);
    in->read_double(&m_time);
    in->read_bool(&m_paused);
    if (m_paused)
	m_pause = ticks;
    else
	m_start = SDL_GetTicks() - ticks;
}

void Timer::save(DATFile *out) const
{
    int ticks = SDL_GetTicks() - m_start;
    if (m_paused)
	ticks = m_paused;
    out->write_int(ticks);
    out->write_double(m_time);
    out->write_bool(m_paused);
}

void Timer::pause()
{
    if (!m_paused)
    {
	m_paused = true;
	m_pause = SDL_GetTicks() - m_start;
    }
}

void Timer::unpause()
{
    if (m_paused)
    {
	m_paused = false;
	m_start = SDL_GetTicks() - m_pause;
	m_pause = 0;
    }
}

void Timer::reset(double time)
{
    m_start = SDL_GetTicks();
    m_pause = 0;
    m_time = time;
    m_paused = false;
}

void Timer::clear()
{
    m_time = -1;
}

double Timer::get_time() const
{
    double ret;
    if (m_paused)
	ret = m_pause / 1000.0;
    else
	ret = (SDL_GetTicks() - m_start) / 1000.0;
    return m_time - ret;
}
