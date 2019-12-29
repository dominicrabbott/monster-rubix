#include "cube_solver.h"
#include "three_cube_solver.h"
#include "edge_solver.h"
#include "center_solver.h"

using namespace ai;

void CubeSolver::solve(const cube::Cube& cube, const cube::CubeCenters& centers) {
	this->cube = cube;
	this->centers = centers;

	CenterSolver center_solver;
	center_solver.add_twist_listener(this);
	center_solver.solve(this->centers.get());

	EdgeSolver edge_solver;
	edge_solver.add_twist_listener(this);
	edge_solver.solve(this->cube.get());
	
	ThreeCubeSolver three_solver;
	three_solver.add_twist_listener(this);
	three_solver.solve(this->cube.get(), this->centers.get());
}
