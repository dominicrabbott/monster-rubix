#ifndef MULTI_CUBE_SOLVER_H
#define MULTI_CUBE_SOLVER_H

#include "twist_listener.h"
#include "twist.h"
#include "cube_display.h"
#include "combined_cube.h"
#include "ui_manager.h"
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

namespace ui {
	class MultiCubeUI : public UIManager, public ai::TwistListener {
		private:

			class SolutionExecutor : public ai::TwistListener {
				private:
					std::mutex solution_mex;
					CubeDisplay* cube;
					std::vector<cube::Twist> solution;
					std::atomic<bool> solution_started;
				public:
					SolutionExecutor(CubeDisplay* cube) : cube(cube), solution_started(false) {}
					void twist(const cube::Twist& twist) override {
						if (solution_started) {
							cube -> rotate(twist);
						}
						else {
							std::unique_lock<std::mutex>(solution_mex);
							solution.push_back(twist);
						}
					}
					void start_solution();
			};

			std::mutex executor_mex;
			std::vector<cube::CombinedCube> sym_cubes;
			std::vector<std::unique_ptr<CubeDisplay>> cube_displays;
			std::vector<std::unique_ptr<SolutionExecutor>> executors;

			bool keyPressed(const OgreBites::KeyboardEvent& event);

		public:
			MultiCubeUI(const std::vector<int>& cube_sizes);
			void twist(const cube::Twist& twist) override {
				cube_displays[0] -> rotate(twist);
			}
			void setup() override;
			void solve_cube(const int cube_index);

	};
}

#endif
