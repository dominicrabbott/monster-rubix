#include "cube_solver.h"
#include "three_cube_solver.h"
#include "edge_solver.h"
#include "center_solver.h"

using namespace ai;

void CubeSolver::solve(const cube::CombinedCube& comb_cube) {
	this->comb_cube = comb_cube;

	CenterSolver center_solver;
	center_solver.add_twist_listener(this);
	center_solver.solve(this->comb_cube.get().get_cube_centers());

	EdgeSolver edge_solver;
	edge_solver.add_twist_listener(this);
	edge_solver.solve(this->comb_cube.get().get_cube());
	
	ThreeCubeSolver three_solver;
	three_solver.add_twist_listener(this);
	three_solver.solve(comb_cube);
}
