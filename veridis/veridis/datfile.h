#ifndef DAT_FILE__H
#define DAT_FILE__H

#include "common.h"

class vector2d;

class DATFile
{
public:
    DATFile();
    ~DATFile();

    int open(const char *filename, const char *mode);
    void close();

    /* read methods */
    void read_int(int *val);
    void read_double(double *val);
    void read_vec(vector2d &val);
    void read_str(string *val);
    void read_color(Uint32 *val);
    void read_bool(bool *val);
    
    /* write methods */
    void write_int(int val);
    void write_double(double val);
    void write_vec(const vector2d &val);
    void write_str(const char *val);
    void write_color(Uint32 val);
    void write_bool(bool val);
private:
    FILE *m_file;
    string m_filename;
    int m_line;

    /* read methods */
    void corrupt(const char *message);
    int read_char(int c, bool str);
    void read_data(string *data);

    /* write methods */
    void write_char(char c, bool str);
    void write_data(const char *val);
};

#endif
