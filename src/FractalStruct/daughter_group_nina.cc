#include "libs.hh"
#include "classes.hh"
#include "headers.hh"
namespace FractalSpace
{
  void daughter_group_nina(Group& new_group,Group& high_group,Fractal& fractal,
			   Fractal_Memory& mem,Misc& misc)
  { 
    FILE* PFDau=fractal.p_file->PFDau;
    fractal.timing(-1,14);
    vector <Point*> p_point_tmp(27); 
    vector <Point*>adj(27);
    vector <bool> go_ahead(27);
    vector <bool> ins(27);
    vector <double> pos(3);
    Point* p_point=0;
    Point* new_points=0;
    Group* p_new=&new_group;
    bool buff_group=false;
    bool buff=false;
    bool edge=false;
    bool pass=false;
    bool really=false;
    mem.total_points_generated=0;
    mem.total_points_used=0;
    int new_points_gen=mem.new_points_gen;
    int new_counter=0;
    int point_counter=0;
    Group* p_mother=new_group.get_mother_group();
    assert(p_mother);
    Group& mother=*p_mother;
    bool mom_buff_group=mother.get_buffer_group();
    int new_level=mother.get_level()+1;
    new_group.set_level(new_level);
    double grid_multiplier=misc.grid_multiply;
    int d_point=Misc::pow(2,fractal.get_level_max()-new_group.get_level());
    new_group.set_points_in_group(0);
    //--------------------------------------------------------------------------------------------------------------------------------
    // Loop over all points in a high_group to generate the new points
    //--------------------------------------------------------------------------------------------------------------------------------
    for(vector<Point*>::const_iterator high_point_itr=high_group.list_high_points.begin();
	high_point_itr != high_group.list_high_points.end();++high_point_itr)
      {
	Point& high_point=**high_point_itr;
	//--------------------------------------------------------------------------------------------------------------------------------
	// Do a search for the 26 neighbor high_points of the high_point
	//--------------------------------------------------------------------------------------------------------------------------------
	p_point_tmp.assign(27,Point::nothing);
	adj_nina(high_point,adj);
	//--------------------------------------------------------------------------------------------------------------------------------
	// Check if any 
	//--------------------------------------------------------------------------------------------------------------------------------
	go_ahead_points(adj,ins,go_ahead);
	int h_x,h_y,h_z;
	high_point.get_pos_point(h_x,h_y,h_z);
	high_point.set_eureka_dau(go_ahead);
	for(int p=0;p<27;++p)
	  {
	    p_point_tmp[p]=0;
	    if(!go_ahead[p])
	      continue;
	    //--------------------------------------------------------------------------------------------------------------------------------
	    // If we have a go_ahead generate the new point, add it to the list, give it a position, do some house keeping
	    // Points at 0,1,3,4,9,10,12,13 are always generated. Other points are generated only if no point with
	    // lower position value will be generated by neighboring high_points.
	    //--------------------------------------------------------------------------------------------------------------------------------
	    if(mem.total_points_used ==mem.total_points_generated)
	      {
		try
		  {
		    new_points=new Point[new_points_gen];
		  }
		catch(bad_alloc& ba)
		  {
		    cerr << " bad allocation in daughter group " << mem.p_mess->FractalRank;
		    cerr << " level " << new_group.get_level();
		    cerr << " " << mem.total_points_generated;
		    cerr << " " << ba.what() << endl;
		    exit(0);
		  }
		mem.total_points_generated+=new_points_gen;
		new_group.list_new_points.push_back(new_points);
		new_counter=0;
	      }
	    p_point=&new_points[new_counter];
	    new_counter++;
	    assert(p_point);
	    Point& point=*p_point;
	    new_group.list_points.push_back(p_point);
	    // point.set_point_to_number(point_counter);
	    point.set_number_in_list(point_counter);
	    mem.total_points_used++;
	    int x=h_x+(p%3)*d_point;
	    int y=h_y+((p/3) %3)*d_point;
	    int z=h_z+(p/9)*d_point; 
	    point.set_pos_point(x,y,z);
	    point.set_real_pointer(p);
	    point.set_point_pointer(0);
	    point.set_p_in_group(p_new);
	    if(p==0)
	      {
		high_point.set_p_daughter_point(p_point); 
		point.set_point_pointer(*high_point_itr);
	      }
	    p_point_tmp[p]=p_point;
	    assert(p_point_tmp[p]);
	    point.set_inside(ins[p]);
	    if(mom_buff_group)
	      {
		fractal.assign_edge_buffer_passive(point,new_level,edge,buff,pass,really);
		buff_group=buff_group || buff;
		if(pass)
		  point.set_inside(false);
	      }
	    point.set_edge_buffer_passive_really_point(edge,buff,pass,really);
	    point_counter++;
	  }
	neighbor_easy(p_point_tmp);
	//--------------------------------------------------------------------------------------------------------------------------------
	// Splitting the particles from the mother point into the eight points that can have particles
	//--------------------------------------------------------------------------------------------------------------------------------
	double ah_x=h_x;
	double ah_y=h_y;
	double ah_z=h_z;
	for(vector <Particle*>::const_iterator particle_itr=high_point.list_particles.begin();particle_itr !=high_point.list_particles.end();++particle_itr)
	  {
	    Particle* p=*particle_itr;
	    p->get_pos(pos);
	    int p_x=static_cast<int>(pos[0]*grid_multiplier-ah_x)/d_point;
	    int p_y=static_cast<int>(pos[1]*grid_multiplier-ah_y)/d_point;
	    int p_z=static_cast<int>(pos[2]*grid_multiplier-ah_z)/d_point;
	    int pp=p_x+p_y*3+p_z*9;
	    p_point_tmp[pp]->list_particles.push_back(p);
	    p->set_p_highest_level_group(p_new);
	    p->set_highest_level(new_level);
	  }
      }
    new_group.set_buffer_group(buff_group);
    fractal.timing(1,14);
    fractal.timing(-1,15);
    if(new_group.list_points.size() != 27)
      {
	for(vector <Point*>::const_iterator point_itr=new_group.list_points.begin();point_itr != new_group.list_points.end();++point_itr)
	  {
	    Point& point=**point_itr;
	    if(point.get_point_up_x() == 0)
	      point.set_point_up_x(try_harder(point,14,false));
	    if(point.get_point_up_y() == 0)
	      point.set_point_up_y(try_harder(point,16,false));
	    if(point.get_point_up_z() == 0)
	      point.set_point_up_z(try_harder(point,22,false));
	  }
      }
    fractal.timing(1,15);
    fractal.timing(-1,14);
    for(vector <Point*>::const_iterator point_itr=new_group.list_points.begin();point_itr != new_group.list_points.end();++point_itr)
      {
	//--------------------------------------------------------------------------------------------------------------------------------
	// Move up in order to find point_down_x etc.
	//--------------------------------------------------------------------------------------------------------------------------------
	Point& point=**point_itr;
	point.down_from_up();
      }
    for(vector<Point*>::const_iterator high_point_itr=high_group.list_high_points.begin();
	high_point_itr != high_group.list_high_points.end();++high_point_itr)
      {
	//--------------------------------------------------------------------------------------------------------------------------------
	// In the corners of the new 3x3 cube point to the point in the mother group that is at the same position
	//--------------------------------------------------------------------------------------------------------------------------------
	Point* p_high_point=*high_point_itr;
	if(p_high_point == 0)
	  fprintf(PFDau,"high point wrong \n");
	Point* p_point=p_high_point->get_p_daughter_point();
	if(p_point == 0)
	  fprintf(PFDau,"daughter wrong \n");
	p_point->point_pointers_all(*p_high_point);
      }
    fractal.timing(1,14);
    fractal.timing(-1,16);
    if(test_group(new_group)) 
      fprintf(PFDau,"badd group %p %d \n",&new_group,new_group.get_level());
    fractal.timing(1,16);
  }
}
