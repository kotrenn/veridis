#ifndef TIMER__H
#define TIMER__H

#include "common.h"
#include "datfile.h"

class Timer
{
public:
    Timer(double time);
    Timer(DATFile *in);

    void save(DATFile *out) const;

    void pause();
    void unpause();
    void reset(double time);
    void clear();

    double get_time() const;
    bool done() const { return get_time() <= 0; }
private:
    Uint32 m_start;
    Uint32 m_pause;
    double m_time;
    bool m_paused;
};

#endif
