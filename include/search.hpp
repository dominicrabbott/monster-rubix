#include <memory>
#include <deque>
#include <unordered_set>
#include <queue>
#include "heuristic_cube_state.h"
#include "cube_state.h"

namespace ai {
	namespace search {
		template<typename StateType>
		std::vector<cube::Twist> trace_twists(const StateType* state) {
			std::vector<cube::Twist> twists;
			const StateType* curr_state = state;
			while (curr_state->parent != nullptr) {
				auto& curr_twists = curr_state->twist_seq.get();
				for (auto it = curr_twists.rbegin(); it != curr_twists.rend(); it++) {
					twists.push_back(*it);	
				}
				curr_state = curr_state->parent.get();
			}
			std::reverse(twists.begin(), twists.end());
			
			return twists;
		}

		template<typename CubeType, typename Heuristic>
		std::vector<cube::Twist> best_first_search(
			const CubeType& root_state, 
			const std::vector<TwistSequence> twist_sequences, 
			const std::function<bool(const CubeType&)>& is_finished) {
				typedef HeuristicCubeState<CubeType, Heuristic> State;

				if (is_finished(root_state)) {
					return std::vector<cube::Twist>();	
				}
				auto state_compare = [](const std::shared_ptr<State> state1, const std::shared_ptr<State> state2) {
					return state1->score > state2->score;
				};
				std::priority_queue<
					std::shared_ptr<State>, 
					std::vector<std::shared_ptr<State>>, 
					decltype(state_compare)> open(state_compare);
				open.push(std::make_shared<State>(root_state));
				std::unordered_set<CubeType> seen = {root_state};
				
				while (!open.empty()) {
					auto curr_state = open.top();
					open.pop();
					for (const auto& twist_seq : twist_sequences) {
						CubeType child_cube(curr_state->cube);
						for (const auto& twist : twist_seq) {
							child_cube.rotate(twist);
						}
						if (!seen.count(child_cube)) {
							seen.insert(child_cube);
							auto child_state = std::make_shared<State>(curr_state, std::move(child_cube), twist_seq);
							if (is_finished(child_state->cube)) {
								return trace_twists(child_state.get());
							}
							open.push(child_state);
						}
					}
				}

				throw std::invalid_argument("The given cube couldn't be solved");
		}

		template<typename CubeType>
		std::vector<cube::Twist> breadth_first_search(
			const CubeType& root_state,
			const std::vector<TwistSequence> twist_sequences,
			const std::function<bool(const CubeType&)>& is_finished) {
				typedef CubeState<CubeType> State;

				if (is_finished(root_state)) {
					return std::vector<cube::Twist>();	
				}
				std::deque<std::shared_ptr<State>> open;
				open.push_back(std::make_shared<State>(root_state));
				std::unordered_set<CubeType> seen = {root_state};
				
				while (!open.empty()) {
					auto curr_state = open.front();
					open.pop_front();
					for (const auto& twist_seq : twist_sequences) {
						CubeType child_cube(curr_state->cube);
						for (const auto& twist : twist_seq) {
							child_cube.rotate(twist);
						}
						if (!seen.count(child_cube)) {
							seen.insert(child_cube);
							auto child_state = std::make_shared<State>(curr_state, std::move(child_cube), twist_seq);
							if (is_finished(child_state->cube)) {
								return trace_twists(child_state.get());
							}
							open.push_back(child_state);
						}
					}
				}

				throw std::invalid_argument("The given cube couldn't be solved");
		}
	}
}
