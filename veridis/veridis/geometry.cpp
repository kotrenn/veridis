#include "common.h"

#include "datfile.h"

Circle::Circle()
    :m_x(0), m_y(0),
     m_r(0)
{
}

Circle::Circle(double x, double y, double r)
    :m_x(x), m_y(y),
     m_r(r)
{
}

Circle::Circle(DATFile *in)
    :m_x(0), m_y(0),
     m_r(0)
{
    in->read_double(&m_x);
    in->read_double(&m_y);
    in->read_double(&m_r);
}

const double &Circle::operator[](int i) const
{
    if (i == 0)
	return m_x;
    else if (i == 1)
	return m_y;
    else
	return m_r;
}

double &Circle::operator[](int i)
{
    if (i == 0)
	return m_x;
    else if (i == 1)
	return m_y;
    else
	return m_r;
}

void Circle::save(DATFile *out) const
{
    out->write_double(m_x);
    out->write_double(m_y);
    out->write_double(m_r);
}

bool Circle::collides(const Shape *shape) const
{
    return shape->visit_collides(this);
}

bool Circle::visit_collides(const Circle *circle) const
{
    double dx = m_x - circle->m_x;
    double dy = m_y - circle->m_y;
    double r = m_r + circle->m_r;
    return dx * dx + dy * dy <= r * r;
}

bool Circle::visit_collides(const Line *line) const
{
    double u[2] = { line->get_x2() - line->get_x1(),
		    line->get_y2() - line->get_y1() };
    double R[2] = { m_x - line->get_x1(),
		    m_y - line->get_y1() };
    double R2[2] = { m_x - line->get_x2(),
		     m_y - line->get_y2() };
    if (R[0] * R[0] + R[1] * R[1] < m_r * m_r)
	return true;
    if (R2[0] * R2[0] + R2[1] * R2[1] < m_r * m_r)
	return true;
    double len = sqrt(u[0] * u[0] + u[1] * u[1]);
    u[0] /= len;
    u[1] /= len;
    double Rdotu = R[0] * u[0] + R[1] * u[1];
    if (Rdotu < 0)
	return false;
    double proj[2] = { Rdotu * u[0], Rdotu * u[1] };
    double proj_len = proj[0] * proj[0] + proj[1] * proj[1];
    if (proj_len > len * len)
	return false;
    double d[2] = { R[0] - proj[0], R[1] - proj[1] };
    double dist = d[0] * d[0] + d[1] * d[1];
    if (dist > m_r * m_r)
	return false;
    return true;
}

bool Circle::visit_collides(const Rect *rect) const
{
    double cx, cy;
    if (m_x < rect->get_x())
	cx = rect->get_x();
    else if (m_x > rect->get_x() + rect->get_w())
	cx = rect->get_x() + rect->get_w();
    else
	cx = m_x;
    if (m_y < rect->get_y())
	cy = rect->get_y();
    else if (m_y > rect->get_y() + rect->get_h())
	cy = rect->get_y() + rect->get_h();
    else
	cy = m_y;
    double dx = m_x - cx;
    double dy = m_y - cy;
    return dx * dx + dy * dy <= m_r * m_r;
}



Line::Line()
    :m_x1(0), m_y1(0),
     m_x2(0), m_y2(0)
{
}

Line::Line(double x1, double y1, double x2, double y2)
    :m_x1(x1), m_y1(y1),
     m_x2(x2), m_y2(y2)
{
}

Line::Line(DATFile *in)
    :m_x1(0), m_y1(0),
     m_x2(0), m_y2(0)
{
    in->read_double(&m_x1);
    in->read_double(&m_y1);
    in->read_double(&m_x2);
    in->read_double(&m_y2);
}

const double &Line::operator[](int i) const
{
    if (i == 0)
	return m_x1;
    else if (i == 1)
	return m_y1;
    else if (i == 2)
	return m_x2;
    else
	return m_y2;
}

double &Line::operator[](int i)
{
    if (i == 0)
	return m_x1;
    else if (i == 1)
	return m_y1;
    else if (i == 2)
	return m_x2;
    else
	return m_y2;
}

void Line::save(DATFile *out) const
{
    out->write_double(m_x1);
    out->write_double(m_y1);
    out->write_double(m_x2);
    out->write_double(m_y2);
}

bool Line::collides(const Shape *shape) const
{
    return shape->visit_collides(this);
}

bool Line::visit_collides(const Circle *circle) const
{
    return circle->visit_collides(this);
}

bool Line::visit_collides(const Line *line) const
{
    double denom  = (line->m_y2 - line->m_y1) * (m_x2 - m_x1);
    double numera = (line->m_x2 - line->m_x1) *	(m_x1 - line->m_y1);
    double numerb = (m_x2 - m_x1) * (m_y1 - line->m_y1);
    denom  -= (line->m_x2 - line->m_x1) * (m_y2 - m_y1);
    numera -= (line->m_y2 - line->m_y1) * (m_x1 - line->m_x1);
    numerb -= (m_y2 - m_y1) * (m_x1 - line->m_x1);
    if (fabs(denom) < EPSILON)
    {
	if (fabs(numera) < EPSILON && fabs(numerb) < EPSILON)
	{
	    double min_x = min(m_x1, m_x2);
	    double min_y = min(m_y1, m_y2);
	    double max_x = max(m_x1, m_x2);
	    double max_y = max(m_y1, m_y2);
	    Rect bounds(min_x, min_y,
			max_x - min_x + 1,
			max_y - min_y + 1);
	    return bounds.contains(line->m_x1, line->m_y1) ||
   		   bounds.contains(line->m_x2, line->m_y2);
	}
    }
    double ua = numera / denom;
    double ub = numerb / denom;
    return 0 < ua && ua < 1 && 0 < ub && ub < 1;
}

bool Line::visit_collides(const Rect *rect) const
{
    Line line1(rect->get_x(), rect->get_y(),
	       rect->get_x() + rect->get_w() - 1,
	       rect->get_y());
    Line line2(rect->get_x(), rect->get_y(),
	       rect->get_x(),
	       rect->get_y() + rect->get_h() - 1);
    Line line3(rect->get_x() + rect->get_w() - 1,
	       rect->get_y(),
	       rect->get_x() + rect->get_w() - 1,
	       rect->get_y() + rect->get_h() - 1);
    Line line4(rect->get_x(),
	       rect->get_y() + rect->get_h() - 1,
	       rect->get_x() + rect->get_w() - 1,
	       rect->get_y() + rect->get_h() - 1);
    return rect->contains(m_x1, m_y1) ||
	rect->contains(m_x2, m_y2) ||
	visit_collides(&line1) || visit_collides(&line2) ||
	visit_collides(&line3) || visit_collides(&line4);
    return false;
}



Rect::Rect()
    :m_x(0), m_y(0),
     m_w(0), m_h(0)
{
}

Rect::Rect(double x, double y, double w, double h)
    :m_x(x), m_y(y),
     m_w(w), m_h(h)
{
}

Rect::Rect(DATFile *in)
    :m_x(0), m_y(0),
     m_w(0), m_h(0)
{
    in->read_double(&m_x);
    in->read_double(&m_y);
    in->read_double(&m_w);
    in->read_double(&m_h);
}

const double &Rect::operator[](int i) const
{
    if (i == 0)
	return m_x;
    else if (i == 1)
	return m_y;
    else if (i == 2)
	return m_w;
    else
	return m_h;
}

double &Rect::operator[](int i)
{
    if (i == 0)
	return m_x;
    else if (i == 1)
	return m_y;
    else if (i == 2)
	return m_w;
    else
	return m_h;
}

void Rect::save(DATFile *out) const
{
    out->write_double(m_x);
    out->write_double(m_y);
    out->write_double(m_w);
    out->write_double(m_h);
}

bool Rect::collides(const Shape *shape) const
{
    return shape->visit_collides(this);
}

bool Rect::visit_collides(const Circle *circle) const
{
    return circle->visit_collides(this);
}

bool Rect::visit_collides(const Line *line) const
{
    return line->visit_collides(this);
}

bool Rect::visit_collides(const Rect *rect) const
{
    return !(
	m_x >= rect->m_x + rect->m_w ||
	m_y >= rect->m_y + rect->m_h ||
	rect->m_x >= m_x + m_w ||
	rect->m_y >= m_y + m_h);
}

bool Rect::contains(double x, double y) const
{
    double min_x = m_x;
    double min_y = m_y;
    double max_x = m_x + m_w - 1;
    double max_y = m_y + m_h - 1;
    return min_x <= x && x <= max_x && min_y <= y && y <= max_y;
}
