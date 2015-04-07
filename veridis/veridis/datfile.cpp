#include "datfile.h"

DATFile::DATFile()
	:m_file(NULL),
	 m_filename(),
	 m_line(0)
{
}

DATFile::~DATFile()
{
}

int DATFile::open(const char *filename, const char *mode)
{
	char dir[] = "dat/";
	char fullname[strlen(dir) + strlen(filename)];
	sprintf(fullname, "%s%s", dir, filename);
	if ((m_file = fopen(fullname, mode)) == NULL)
	{
		fprintf(stderr, "DATFile: could not open file \"%s\" ", filename);
		fprintf(stderr, "in mode \"%s\"\n", mode);
		return -1;
	}

	m_filename = string(filename);
	m_line = 0;

	char header[] = "DAT v1.0\n";
    
	if (strcmp(mode, "r") == 0)
	{
		/* check for file version */
		string buf;
		int tmp = 0;
		while (tmp != '\n')
			buf.push_back(tmp = fgetc(m_file));
		if (buf != header)
		{
			fprintf(stderr,
			        "Incorrect version of DAT file: %s\n",
			        filename);
			fprintf(stderr,
			        "File uses version: %s\n", buf.c_str());
			exit(1);
		}
	}

	if (strcmp(mode, "w") == 0)
		fprintf(m_file, header);

	return 0;
}

void DATFile::close()
{
	fclose(m_file);
}

/* read methods */
void DATFile::corrupt(const char *message)
{
	fprintf(stderr, "Corrupt DAT file <%s>\n", m_filename.c_str());
	fprintf(stderr, "Line %d\n", m_line);
	fprintf(stderr, "Error: %s\n", message);
	exit(1);
}

int DATFile::read_char(int c, bool str)
{
	int ret = c;
	if (str)
	{
		if (ret == '\\')
		{
			ret = fgetc(m_file);
			switch(ret)
			{
			case '\\': ret = '\\'; break;
			case '\"': ret = '\"'; break;
			case '\'': ret = '\''; break;
			case 'n': ret = '\n'; break;
			case 'r': ret = '\r'; break;
			case 'b': ret = '\b'; break;
			case 't': ret = '\t'; break;
			case 'f': ret = '\f'; break;
			case 'a': ret = '\a'; break;
			case 'v': ret = '\v'; break;
			case '?': ret = '\?'; break;
			}
		}
	}
	return ret;
}

void DATFile::read_data(string *data)
{
	data->clear();
	int tmp = read_char(fgetc(m_file), false);
	if (tmp == '\"')
	{
		tmp = fgetc(m_file);
		while (tmp != '\"')
		{
			tmp = read_char(tmp, true);
			data->push_back(tmp);
			tmp = fgetc(m_file);
		}
		if ((tmp = fgetc(m_file)) != '\n')
			corrupt("Line did not end with a newline");
	}
	else
		corrupt("Line did not begin with a \"");
	m_line++;
}

void DATFile::read_int(int *val)
{
	string data;
	read_data(&data);
	sscanf(data.c_str(), "%d", val);
}

void DATFile::read_double(double *val)
{
	string data;
	read_data(&data);
	sscanf(data.c_str(), "%lf", val);
}

void DATFile::read_vec(vector2d &val)
{
	string data;
	read_data(&data);
	double x, y;
	sscanf(data.c_str(), "%lf %lf", &x, &y);
	val[0] = x;
	val[1] = y;
}

void DATFile::read_str(string *val)
{
	val->clear();
	read_data(val);
}

void DATFile::read_color(Uint32 *val)
{
	string data;
	read_data(&data);
	sscanf(data.c_str(), "%x", val);
}

void DATFile::read_bool(bool *val)
{
	string data;
	read_data(&data);
	int x;
	sscanf(data.c_str(), "%d", &x);
	*val = x == 1;
}

/* write methods */
void DATFile::write_char(char c, bool str)
{
	if (str)
	{
		switch(c)
		{
		case '\\': fprintf(m_file, "\\\\"); break;
		case '\"': fprintf(m_file, "\\\""); break;
		case '\'': fprintf(m_file, "\\\'"); break;
		case '\n': fprintf(m_file, "\\\n"); break;
		case '\r': fprintf(m_file, "\\\r"); break;
		case '\b': fprintf(m_file, "\\\b"); break;
		case '\t': fprintf(m_file, "\\\t"); break;
		case '\f': fprintf(m_file, "\\\f"); break;
		case '\a': fprintf(m_file, "\\\a"); break;
		case '\v': fprintf(m_file, "\\\v"); break;
		case '\?': fprintf(m_file, "\\\?"); break;
		default: fputc(c, m_file); break;
		}
	}
	else
		fputc(c, m_file);
}

void DATFile::write_data(const char *val)
{
	write_char('\"', false);
	int len = strlen(val);
	for (int i = 0; i < len; ++i)
		write_char(val[i], true);
	write_char('\"', false);
	write_char('\n', false);
	m_line++;
}

void DATFile::write_int(int val)
{
	char val_str[80];
	snprintf(val_str, 80, "%d", val);
	write_data(val_str);
}

void DATFile::write_double(double val)
{
	char val_str[80];
	snprintf(val_str, 80, "%lf", val);
	write_data(val_str);
}

void DATFile::write_vec(const vector2d &val)
{
	char val_str[80];
	snprintf(val_str, 80, "%lf %lf", val[0], val[1]);
	write_data(val_str);
}

void DATFile::write_str(const char *val)
{
	write_data(val);
}

void DATFile::write_color(Uint32 val)
{
	char val_str[80];
	snprintf(val_str, 80, "%x", val);
	write_data(val_str);
}

void DATFile::write_bool(bool val)
{
	char val_str[80];
	snprintf(val_str, 80, "%d", val? 1 : 0);
	write_data(val_str);
}
