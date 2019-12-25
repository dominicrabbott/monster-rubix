#include <memory>
#include <queue>
#include <unordered_set>

namespace ai {
	template<typename StateType, typename Heuristic>
	std::vector<cube::Twist> Search<StateType, Heuristic>::trace_twists(const HeuristicCubeState<StateType, Heuristic>* state) {
		std::vector<cube::Twist> twists;
		const State* curr_state = state;
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

	template<typename StateType, typename Heuristic>
	std::vector<cube::Twist> Search<StateType, Heuristic>::best_first_search(const StateType& root_state, const std::vector<TwistSequence> twist_sequences, const std::function<bool(const StateType&)>& is_finished) {
		if (is_finished(root_state)) {
			return std::vector<cube::Twist>();	
		}
		auto state_compare = [](const std::shared_ptr<State> state1, const std::shared_ptr<State> state2) {
			return state1->score > state2->score;
		};
		std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, decltype(state_compare)> open(state_compare);
		open.push(std::make_shared<State>(root_state));
		std::unordered_set<StateType> seen = {root_state};
		
		while (!open.empty()) {
			auto curr_state = open.top();
			open.pop();
			for (const auto& twist_seq : twist_sequences) {
				StateType child_cube(curr_state->cube);
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
}
