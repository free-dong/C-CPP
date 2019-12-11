//
#include "loadargs.h"
void loadargs(arguments* args)
{

	fstream cfgfile;
	cfgfile.open("../etc/config.cfg");
	if (!cfgfile.is_open())
	{
		cerr << "Error: can not find config file " << endl;
		return;
	}
	string cfgline;
	string ch1;
	string ch2;
	while (getline(cfgfile, cfgline))
	{
		istringstream ss;
		ss.str(cfgline);
		ss >> ch1 >> ch2;
		if (ch1 == "videoname")
			ss >> args->seq;
		if (ch1 == "videopath")
			ss >> args->videopath;
		
		if (ch1 == "modelCFG")
			ss >> args->modelCFG;		
		if (ch1 == "modelWeights")
			ss >> args->modelWeights;		
		if (ch1 == "classNamesFile")
			ss >> args->classNamesFile;	
		
		if (ch1 == "httpd_ip")
			ss >> args->httpd_ip;
		if (ch1 == "httpd_port")
			ss >> args->httpd_port;
		if (ch1 == "redis_host")
			ss >> args->redis_host;
		if (ch1 == "redis_port")
			ss >> args->redis_port;
		if (ch1 == "threshold")
			ss >> args->threshold;
		if (ch1 == "thre_flag")
			ss >> args->thre_flag;
		if (ch1 == "display")
			ss >> args->display;
		if (ch1 == "mode")
			ss >> args->mode;
		if (ch1 == "use_crow")
			ss >> args->use_crow;
		if (ch1 == "crow_port")
			ss >> args->crow_port;
		
		if (ch1 == "point_A")
			ss >> args->point_A;
		if (ch1 == "point_B")
			ss >> args->point_B;
		if (ch1 == "point_C")
			ss >> args->point_C;
		if (ch1 == "point_D")
			ss >> args->point_D;
		
		if (ch1 == "expire_time")
			ss >> args->expire_time;
		if (ch1 == "frame_skip")
			ss >> args->frame_skip;
		if (ch1 == "pt_interval")
			ss >> args->pt_interval;
		if (ch1 == "videofps")
			ss >> args->videofps;
		else
			continue;
	}
}