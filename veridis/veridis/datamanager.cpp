#include "datamanager.h"

DataManager *DataManager::m_instance = NULL;

DataManager *DataManager::get_instance()
{
	if (m_instance == NULL)
		m_instance = new DataManager();
	return m_instance;
}

DataManager::DataManager()
	:m_int(),
	 m_str(),
	 m_vec(),
	 m_double()
{
	DATFile data;
	data.open("settings.dat", "r");

	int num_objs;
	data.read_int(&num_objs);

	string type;
	string name;
	for (int i = 0; i < num_objs; ++i)
	{
		data.read_str(&type);
		data.read_str(&name);
		if (type == "int")
		{
			int x;
			data.read_int(&x);
			m_int[name] = x;
		}
		else if (type == "double")
		{
			double x;
			data.read_double(&x);
			m_double[name] = x;
		}
		else if (type == "vec")
		{
			vector2d x;
			data.read_vec(x);
			m_vec[name] = x;
		}
		else if (type == "str")
		{
			string x;
			data.read_str(&x);
			m_str[name] = x;
		}
	}
    
	data.close();
}

DataManager::~DataManager()
{
}

void DataManager::save() const
{
	DATFile out;
	out.open("settings.dat", "w");
	out.write_int(m_int.size() + m_str.size() +
	              m_vec.size() + m_double.size());
    
	map<string, int>::const_iterator ii;
	map<string, string>::const_iterator si;
	map<string, vector2d>::const_iterator vi;
	map<string, double>::const_iterator di;
	for (ii = m_int.begin(); ii != m_int.end(); ii++)
	{
		out.write_str("int");
		out.write_str((*ii).first.c_str());
		out.write_int((*ii).second);
	}
	for (si = m_str.begin(); si != m_str.end(); si++)
	{
		out.write_str("str");
		out.write_str((*si).first.c_str());
		out.write_str((*si).second.c_str());
	}
	for (vi = m_vec.begin(); vi != m_vec.end(); vi++)
	{
		out.write_str("vec");
		out.write_str((*vi).first.c_str());
		out.write_vec((*vi).second);
	}
	for (di = m_double.begin(); di != m_double.end(); di++)
	{
		out.write_str("double");
		out.write_str((*di).first.c_str());
		out.write_double((*di).second);
	}
	out.close();
	printf("wrote settings.dat\n");
}

void DataManager::error(const char *var) const
{
	fprintf(stderr, "Could not find value <%s> in settings.dat\n", var);
	exit(1);
}

int DataManager::get_int(const char *var)
{
	string var_str(var);
	if (m_int.find(var_str) == m_int.end())
		error(var);
	return m_int[var_str];
}

double DataManager::get_double(const char *var)
{
	string var_str(var);
	if (m_double.find(var_str) == m_double.end())
		error(var);
	return m_double[var_str];
}

void DataManager::get_str(const char *var, char *dst)
{
	string var_str(var);
	if (m_str.find(var_str) == m_str.end())
		error(var);
	strcpy(dst, m_str[var_str].c_str());
}

vector2d DataManager::get_vec(const char *var)
{
	string var_str(var);
	if (m_vec.find(var_str) == m_vec.end())
		error(var);
	return m_vec[var_str];
}

void DataManager::set_int(const char *var, int val)
{
	string var_str(var);
	m_int[var_str] = val;
}

void DataManager::set_double(const char *var, double val)
{
	string var_str(var);
	m_str[var_str] = val;
}
void DataManager::set_str(const char *var, const char *val)
{
	string var_str(var);
	string val_str(val);
	m_str[var_str] = val_str;
}

void DataManager::set_vec(const char *var, const vector2d &val)
{
	string var_str(var);
	m_vec[var_str] = val;
}
