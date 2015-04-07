#ifndef DATA_MANAGER__H
#define DATA_MANAGER__H

#include "common.h"
#include "datfile.h"

class DataManager
{
public:
    static DataManager *get_instance();
    ~DataManager();

    void save() const;

    int get_int(const char *var);
    double get_double(const char *var);
    void get_str(const char *var, char *dst);
    vector2d get_vec(const char *var);
    void set_int(const char *var, int val);
    void set_double(const char *var, double val);
    void set_str(const char *var, const char *val);
    void set_vec(const char *var, const vector2d &val);
private:
    DataManager();
    static DataManager *m_instance;

    void error(const char *var) const;

    map<string, int> m_int;
    map<string, string> m_str;
    map<string, vector2d> m_vec;
    map<string, double> m_double;
};

#endif
