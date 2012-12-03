namespace FractalSpace
{
  void adj_nina(Point& point,vector <Point*>& adj);
  double Age_of_the_universe (const double& omega_0, const double& omega_lambda, const double& redshift);
  void assign_density(Group& group, Fractal& fractal);
  void buffer_points(Group& group, Fractal& fractal,Misc& misc);
  void candidate_points();
  void check_for_edge_trouble(Fractal& fractal);
  bool check_high(Point& point,Fractal& fractal);
  void clean_up(Fractal_Memory& mem,Misc& misc,Fractal& fractal);
  double cosmos_power(const double& k,Fractal_Memory& fractal_memory);
  void daughter_group_nina(Group& new_group,Group& high_group,Fractal& fractal,Fractal_Memory& memo,Misc& misc);
  void density_edge(Group& group, Misc& misc);
  void dens_to_slices(Group& group,Fractal_Memory& mem,Fractal& frac);
  template <class T> T det(const vector <T>& m);
  double dGrowthdA(const double& omega_0, const double& omega_lambda, const double& redshift);
  double dGrowthdT(const double& omega_0, const double& omega_lambda, const double& redshift);
  double dist1(const double& x,const double& y);
  void dump(Point& point);
  void dump_all_particles(Fractal& fractal);
  void dump_group(Group& group,Misc& misc);
  void dump_tree(Fractal_Memory& fractal_memory,Fractal& fractal);
  void edge_buffer_inside(vector <int>& n,vector <int>& Box,vector <int>& BBox,vector <int>& Buffer,bool& MPIrun,
			  vector <bool>& periods,bool& inside,vector <bool>& buff,vector <bool>& edge);
  template <class M>  void energy_simple(M& mem, Fractal& fractal);
  void equivalence_class(Group& group);
  //  int fftw_where(const int& i,const int& j,const int& k,const int& l1,const int& l2);
  void fix_memory(Fractal& frac,const int& ispace,const int& jfield);
  void force_at_particle(Group& group, Fractal& fractal,const bool& conserve);
  void force_at_particle(vector <vector <Group*> >& all_groups, Fractal& fractal);
  void force_at_particle_sharp(Group& group, Fractal& fractal);
  void force_at_point(Group& group, Fractal& fractal);
  void force_at_point_initial(Group& group, Fractal& fractal);
  void force_shear_at_particle(Fractal_Memory& fractal_memory,Fractal& fractal,Misc& misc);
  void force_shear_at_point(Group& group,Fractal& fractal);
  void force_sum_particle(Group& group,Fractal& fractal,const bool& doit);
  template <class M>  void force_test(M& mem,Fractal& fractal);
  void fractal_force(Fractal& fractal,Fractal_Memory& fractal_memory);
  int fractal_force_wrapper(Fractal_Memory* p_fractal_memory,Fractal* p_fractal);
  template <class M> void fractal_memory_parameters(M& mem);
  void gather_particles(Fractal_Memory& mem,Fractal& frac);
  void go_ahead_points(Fractal& fractal,vector <Point*>& adj,vector <bool>& ins,vector <bool>& go_ahead);
  void groups_level(Fractal& fractal,vector < vector<Group*> >& all_groups);
  double Growth(const double& omega_0, const double& omega_lambda, const double& redshift);
  void halo_force(Fractal& fractal);
  void halo_force_fixed(Fractal& frac);
  void heavies(Fractal& fractal,Fractal& fractal_ghost,Misc& misc);
  void highest_level_group(Fractal_Memory& fractal_memory,Fractal& fractal,Misc& misc);
  void highest_level_group_other(vector <vector <Group*> >& all_groups,Fractal& fractal,Misc& misc);
  bool high_enough_level(Point& point,Group& group,Fractal& fractal,Misc& misc);
  void high_groups(Group& group);
  void high_pairs(Group& group, Misc& misc);
  void high_points(Group& group, Fractal& fractal,Misc& misc);
  double Hubble (const double& omega_0, const double& omega_lambda, const double& redshift);
  void hypre_solver(Fractal& fractal,Fractal_Memory& mem,const int& level);
  void hypre_struct_solver(vector <Point*>& p_points_left,vector <Point*>& p_points_right,Fractal& fractal,Fractal_Memory& mem,const int& level);
  void initial_forces_sharp(Fractal_Memory& fractal_memory,Fractal& fractal,Misc& misc);
  void isolated_solver(Group& group,Fractal_Memory& fractal_memory,Fractal& fractal,Misc& misc);
  bool it_is_inside(Point* p_point);
  double Lambda (const double& omega_0, const double& omega_lambda, const double& redshift);
  double Length(const double& omega_0, const double& omega_lambda, const double& redshift);
  bool LesserPoint(Point* p1,Point* p2);
  void left_right(Fractal& frac,vector <double>& pos_left,vector <double>& pos_right);
  void list_buffer(Point& point,const int& corner);
  void make_decisions_erika(Misc& misc);
  template <class M, class F>  void make_particles(M& mem,F& frac,int& count,const double& m,const bool& crash);
  void max_predict(Fractal_Memory& fractal_memory,Fractal& fractal,vector <double>& shear_force,double& min_vol);
  void neighbor_easy(vector <Point*>& p);
  void neighbors_nina(Point& point, vector <Point*>& adj);
  double Omega (const double& omega_0, const double& omega_lambda, const double& redshift);
  template <class T> bool overlap(vector <T>& xleft,vector <T>& xright,vector <T>& yleft,vector <T>& yright);
  template <class T> bool overlap(vector <T>& xleft,vector <T>& xright,vector <T>& yleftright);
  void particle_lists(vector <vector <Group*> >& all_groups,Fractal& fractal,Fractal& fractal_ghost,Misc& misc);
  void particle_lists_fixed(vector <vector <Group*> >& all_groups,Fractal& fractal,Misc& misc);
  void periodic_solver(Group& group, Fractal_Memory& fractal_memory,Fractal& fractal);
  void poisson_solver(Fractal& fractal,Fractal_Memory& mem,const int& level);
  void potential_start(Group& group);
  void power_spectrum(fftw_complex* rhoC,const int& length,vector <double>& variance_rho,vector <double>& variance_pot,
		      vector <double>& variance_force,vector <double>& variance_force_s,const int& lev,const double& d0,const bool& start_up,
		      Fractal_Memory& mem);
  bool rad_compare(Particle* par1,Particle* par2);
  void receive_dens(Fractal_Memory& mem,Fractal& frac,const int& length);
  void receive_potf(Group& group,Fractal_Memory& mem,Fractal& frac);
  void scatter_particles(Fractal_Memory& mem,Fractal& frac);
  void slices_to_potf(Fractal_Memory& mem,Fractal& frac,const int& length_1);
  void sor(Group& group, Fractal& fractal,vector <Point*>& list_left_x,const int& dir);
  void sor_solver(Group& group, Fractal& fractal);
  void sort3_list(Group& group,const int& what);
  void sort_3(Fractal& fractal,Group& group);
  template <class M, class F> int split_particle(M& mem,F& frac,const double& x0,const double& y0,const double& z0,
						  int& count,const double& m,const int& split_to,const bool& gen_part);
  template <class M>  void step_simple(M& mem,Fractal& fractal);
  void sum_pot_forces(Fractal& fractal);
  bool test_group(Group& group);
  bool test_tree(Fractal_Memory& fractal_memory,Fractal& fractal);
  void tree_start(Group& group,Fractal& fractal,Fractal_Memory& memo,Misc& misc);
  Point* try_harder(Point& point0,const int& ni,const bool& easy);
  void update_rv(Fractal& fractal,const int& param,const double& const1,const double& const2);
  void velocities(Fractal_Memory& mem,Fractal& frac);
  void write_rv(const int& step,Fractal& fractal);
}

