#include <vector>
#include <string>
#include <list>

#ifndef _UTLIS_H_
#define _UTLIS_H_

namespace idsia_utils{
	typedef struct position_t{
	  int node;
	  double x;
	  double y;
	  double z;
	} position;

	typedef struct app_t{
	  std::string type;
	  int sender;
	  int remote;
	  double interval;
	  double start;
	  double end;
	} app;

	app parseAppLine(std::string line);
	std::list<app>* readAppFile(std::string fileName);

	position parseLine(std::string line);
	std::vector<position> readFile(std::string fileName);

	
}
#endif /* _UTLIS_H_ */
