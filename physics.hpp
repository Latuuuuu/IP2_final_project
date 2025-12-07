#ifndef PHYSICS_HPP_INCLUDED
#define PHYSICS_HPP_INCLUDED

#include <utility>
#include <math.h>

//彈碰的速度變化
std::pair<std::pair<double,double>,std::pair<double,double>> elastic_collision_acceleration(double m1, std::pair<double,double> v1, std::pair<double,double> x1,double m2, std::pair<double,double> v2, std::pair<double,double> x2){
	double dist = sqrt(pow(x1.first - x2.first,2) + pow(x1.second - x2.second,2));
	double ip = (v1.first - v2.first) * (x1.first - x2.first) + (v1.second - v2.second) * (x1.second - x2.second);
	if(ip >= 0) return std::make_pair(v1,v1);
	double val = ip/pow(dist,2);
	double new_speed_x_1 = -1*(2*m2/(m1+m2))*val*(x1.first - x2.first);
	double new_speed_y_1 = -1*(2*m2/(m1+m2))*val*(x1.second - x2.second);
	double new_speed_x_2 = -1*(2*m1/(m1+m2))*val*(x2.first - x1.first);
	double new_speed_y_2 = -1*(2*m1/(m1+m2))*val*(x2.second - x1.second);
	return std::make_pair(std::make_pair(new_speed_x_1,new_speed_y_1),std::make_pair(new_speed_x_2,new_speed_y_2));
}

#endif
