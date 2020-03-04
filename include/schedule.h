#pragma once

#include "pattern.h"
#include "prefix.h"
#include "disjoint_set_union.h"

#include <vector>

class Schedule
{
public:
    //TODO : more kinds of constructors to construct different Schedules from one Pattern
    Schedule(const Pattern& pattern, bool& is_pattern_valid, int performance_modeling_type, int v_cnt, int e_cnt);
    // performance_modeling type = 0 : not use modeling
    //                      type = 1 : use our modeling
    //                      type = 2 : use GraphZero's modeling
    ~Schedule();
    inline int get_total_prefix_num() const { return total_prefix_num;}
    inline int get_father_prefix_id(int prefix_id) const { return father_prefix_id[prefix_id];}
    inline int get_loop_set_prefix_id(int loop) const { return loop_set_prefix_id[loop];}
    inline int get_size() const { return size;}
    inline int get_last(int i) const { return last[i];}
    inline int get_next(int i) const { return next[i];}
    inline int get_in_exclusion_optimize_num() const { return in_exclusion_optimize_num;}
    void add_restrict(const std::vector< std::pair<int, int> >& restricts);
    inline int get_total_restrict_num() const { return total_restrict_num;}
    inline int get_restrict_last(int i) const { return restrict_last[i];}
    inline int get_restrict_next(int i) const { return restrict_next[i];}
    inline int get_restrict_index(int i) const { return restrict_index[i];}
    int get_max_degree();
    int aggressive_optimize(const int *adj_mat, std::vector< std::pair<int,int> >& ordered_pairs) const;
    int aggressive_optimize_get_all_pairs(const int *adj_mat, std::vector< std::vector< std::pair<int,int> > >& ordered_pairs_vector);
    void aggressive_optimize_dfs(Pattern base_dag, std::vector< std::vector<int> > isomorphism_vec, std::vector< std::vector< std::vector<int> > > permutation_groups, std::vector< std::pair<int,int> > ordered_pairs, std::vector< std::vector< std::pair<int,int> > >& ordered_pairs_vector);
    
    //------GraphZero's algorithm for restriction generation
    int GraphZero_aggressive_optimize(const int *adj_mat, std::vector< std::pair<int,int> >& ordered_pairs) const;
    void GraphZero_get_automorphisms(const int *adj_mat, std::vector< std::vector<int> > &Aut) const;
    //------Graph Zero's function end

    std::vector< std::vector<int> > get_isomorphism_vec(const int *adj_mat) const;
    static std::vector< std::vector<int> > calc_permutation_group(const std::vector<int> vec, int size);
    inline const int* get_adj_mat_ptr() const {return adj_mat;}
    
    //use principle of inclusion-exclusion to optimize
    void init_in_exclusion_optimize(int optimize_num);

    void print_schedule();

    std::vector< std::vector< std::vector<int> > >in_exclusion_optimize_group;
    std::vector< int > in_exclusion_optimize_val;
private:
    int* adj_mat;
    int* father_prefix_id;
    int* last;
    int* next;
    int* loop_set_prefix_id;
    Prefix* prefix;
    int* restrict_last;
    int* restrict_next;
    int* restrict_index;
    int size;
    int total_prefix_num;
    int total_restrict_num;
    int in_exclusion_optimize_num;

    void build_loop_invariant();
    int find_father_prefix(int data_size, const int* data);
    void get_full_permutation(std::vector< std::vector<int> >& vec, bool use[], std::vector<int> tmp_vec, int depth) const;
    void performance_modeling(const int *adj_mat, int* best_order, int v_cnt, int e_cnt);
    //------GraphZero's algorithm for restriction generation
    void GraphZero_performance_modeling(const int *adj_mat, int* best_order, int v_cnt, int e_cnt);
    //------Graph Zero's function end
    //TODO function above have not defined
    void get_in_exclusion_optimize_group(int depth, int* id, int id_cnt, int* in_exclusion_val);
};
