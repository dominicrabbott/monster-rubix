#include "cube_display.h"
#include "Ogre.h"
#include "color.h"
#include "twist.h"
#include <string>
#include <array>
#include <sstream>
#include <algorithm>

using namespace ui;
using namespace Ogre;


CubeDisplay::Rotation::Rotation(const cube::Twist& move, const int cube_size, const int frames_per_rotation) : 
	frames_per_rotation(frames_per_rotation) {
	static std::unordered_map<cube::Face, cube::Face> opposing_faces = {
		{cube::Face::LEFT, cube::Face::RIGHT},
		{cube::Face::BOTTOM, cube::Face::TOP},
		{cube::Face::BACK, cube::Face::FRONT},
	};
	
	to_layer = move.layer;
	from_layer = move.wide_turn ? 0 : to_layer;
	
	if (!opposing_faces.count(move.face)) {
		total_degrees = remaining_degrees = move.degrees;
		axis = move.face;
	}
	else {
		total_degrees = remaining_degrees = -move.degrees;
		to_layer = cube_size - 1 - from_layer;
		from_layer = cube_size - 1 - move.layer;
		axis = opposing_faces[move.face];
	}
	
}

CubeDisplay::CubeDisplay(const int size, SceneManager* scene_mgr) : scene_mgr(scene_mgr) {
	create_cube(size);
	frames_per_rotation = 10;
}

void CubeDisplay::rotate(const cube::Twist& move) {
	std::unique_lock<std::mutex> lock(mex);
	pending_rotations.push(Rotation(move, cube.size(), frames_per_rotation));
}

void CubeDisplay::frameRendered(const Ogre::FrameEvent& event)  {
	std::unique_lock<std::mutex>(mex);
	if (!pending_rotations.empty()) {
		auto& curr_rot = pending_rotations.front();
		if (curr_rot.remaining_degrees == curr_rot.total_degrees) {
			for (int i = curr_rot.from_layer; i <= curr_rot.to_layer; i++) {
				auto layer = find_layer(curr_rot.axis, i);
				for (auto& ptr_vec : layer) {
					for (auto node_ptr : ptr_vec) {
						adopt_scene_node(*node_ptr, skeleton[curr_rot.axis][i]);
					}
				}
			}
		}
		int degrees_per_frame = curr_rot.total_degrees/curr_rot.frames_per_rotation;
		for (int i = curr_rot.from_layer; i <= curr_rot.to_layer; i++) {
			skeleton[curr_rot.axis][i] -> roll(Degree(degrees_per_frame));
		}
		curr_rot.remaining_degrees -= degrees_per_frame;
		if (curr_rot.remaining_degrees == 0) {
			for (int i = curr_rot.from_layer; i <= curr_rot.to_layer; i++) {
				rotate_layer(find_layer(curr_rot.axis, i), curr_rot.total_degrees);
			}
			pending_rotations.pop();
		}
	}
} 
void CubeDisplay::create_cube(const int size) {
	create_skeleton(size);

	float translation = static_cast<float>(size*piece_size)/2;

	for (int x = 0; x < size; x++) {
		cube.push_back(std::vector<std::vector<SceneNode*>>());
		for (int y = 0; y < size; y++) {
			cube[x].push_back(std::vector<SceneNode*>());
			for (int z = 0; z < size; z++) {
				auto* cube_node = create_piece(create_piece_colors(Vector3(x,y,z), size));
				cube_node -> getParentSceneNode() -> removeChild(cube_node);
				root_node -> addChild(cube_node);
				cube_node -> setPosition(x*piece_size-translation, y*piece_size-translation, z*piece_size-translation);

				cube[x][y].push_back(cube_node);
			}	
		}	
	}
}

std::unordered_map<cube::Face, Color> CubeDisplay::create_piece_colors(const Vector3 coords, const int size) {
	using namespace cube;
	
	std::unordered_map<Face, Color> result;
	if (coords[0] == 0) {
		result.insert(std::make_pair(Face::LEFT, Color::GREEN));	
	}
	else if (coords[0] == size-1) {
		result.insert(std::make_pair(Face::RIGHT, Color::BLUE));	
	}
	
	if (coords[1] == 0) {
		result.insert(std::make_pair(Face::BOTTOM, Color::ORANGE));	
	}
	else if (coords[1] == size-1) {
		result.insert(std::make_pair(Face::TOP, Color::RED));	
	}
	
	if (coords[2] == 0) {
		result.insert(std::make_pair(Face::BACK, Color::WHITE));	
	}
	else if (coords[2] == size-1) {
		result.insert(std::make_pair(Face::FRONT, Color::YELLOW));	
	}
	

	return result;
}

void CubeDisplay::adopt_scene_node(SceneNode* child, SceneNode* new_parent) {
	auto global_pos = child -> convertLocalToWorldPosition(Vector3(0,0,0));
	auto global_orientation = child -> convertLocalToWorldOrientation(Quaternion());
	
	child -> getParentSceneNode() -> removeChild(child);
	new_parent -> addChild(child);
	child -> setOrientation(new_parent -> convertWorldToLocalOrientation(global_orientation));
	child -> setPosition(new_parent -> convertWorldToLocalPosition(global_pos));
}

void CubeDisplay::create_skeleton(const int size) {
	root_node = scene_mgr -> getRootSceneNode() -> createChildSceneNode();
	float axis_length = size-1;
	
	for (float f = -axis_length/2; f <= axis_length/2; f++) {
		auto* skeleton_node_z = root_node -> createChildSceneNode();
		skeleton_node_z -> lookAt(Vector3(0,0,1), Node::TransformSpace::TS_LOCAL);
		skeleton_node_z -> setPosition(0,0,f*piece_size);

		auto* skeleton_node_y = root_node -> createChildSceneNode();
		skeleton_node_y -> lookAt(Vector3(0,1,0), Node::TransformSpace::TS_LOCAL);
		skeleton_node_y -> setPosition(0,f*piece_size,0);

		auto* skeleton_node_x = root_node -> createChildSceneNode();
		skeleton_node_x -> lookAt(Vector3(1,0,0), Node::TransformSpace::TS_LOCAL);
		skeleton_node_x -> setPosition(f*piece_size,0,0);

		skeleton[cube::Face::RIGHT].push_back(skeleton_node_x);
		skeleton[cube::Face::TOP].push_back(skeleton_node_y);
		skeleton[cube::Face::FRONT].push_back(skeleton_node_z);
	}

}

SceneNode* CubeDisplay::create_piece(const std::unordered_map<cube::Face, Color>& piece) {
	static std::unordered_map<Color, std::string> materials = {
		{Color::GREEN, "green_sticker"},	
		{Color::BLUE, "blue_sticker"},	
		{Color::YELLOW, "yellow_sticker"},	
		{Color::WHITE, "white_sticker"},	
		{Color::RED, "red_sticker"},	
		{Color::ORANGE, "orange_sticker"},	
		{Color::BLACK, "black"},	
	};
	static std::unordered_map<int, Vector3i> verticies = {
		{0, {0,0,0}},
		{1, {piece_size,0,0}},
		{2, {piece_size,0,piece_size}},
		{3, {0,0,piece_size}},
		{4, {0,piece_size,0}},
		{5, {piece_size,piece_size,0}},
		{6, {piece_size,piece_size,piece_size}},
		{7, {0,piece_size,piece_size}},
	};
	static std::unordered_map<cube::Face, std::array<int, 4>> face_verticies = {	
		{cube::Face::FRONT, {3,2,6,7}},
		{cube::Face::BACK, {4,5,1,0}},
		{cube::Face::RIGHT, {2,1,5,6}},
		{cube::Face::LEFT, {0,3,7,4}},
		{cube::Face::TOP, {7,6,5,4}},
		{cube::Face::BOTTOM, {0,1,2,3}},
	};
	static std::vector<std::vector<int>> texture_verticies = {{0,0},{1,0},{1,1},{0,1}};

	auto* piece_entity = scene_mgr -> createManualObject();
	for (const auto face : cube::ALL_FACES) {
		std::string material = "black";
		if (piece.count(face)) {
			material = materials[piece.at(face)];
		}
		
		piece_entity -> begin(material, RenderOperation::OT_TRIANGLE_LIST);

		for (int i = 0; i < 4; i++) {
			auto& vertex = verticies[face_verticies[face][i]];
			piece_entity -> position(vertex[0], vertex[1], vertex[2]);
			piece_entity -> textureCoord(texture_verticies[i][0], texture_verticies[i][1]);	
		}

		piece_entity -> quad(0,1,2,3);
		piece_entity -> end();
		
	}
	auto* result_node = scene_mgr -> getRootSceneNode() -> createChildSceneNode();
	result_node -> attachObject(piece_entity);

	return result_node;
}

void CubeDisplay::transpose_layer(const std::vector<std::vector<SceneNode**>>& layer) {
	using std::swap;
	for (int i = 0; i < layer.size()-1; i++) {
		for (int j = i+1; j < layer.size(); j++) {
			swap(*layer[i][j], *layer[j][i]);	
		}	
	}
}

void CubeDisplay::reverse_layer_rows(const std::vector<std::vector<SceneNode**>>& layer) {
	using std::swap;
       	for (const auto& row : layer) {
	       for (int col = 0; col < row.size()/2; col++) {
	      		swap(*row[col], *row[row.size()-1-col]); 
	       }
       	}
}

void CubeDisplay::rotate_layer(const std::vector<std::vector<SceneNode**>>& layer, const int degrees) {
	if (degrees == 90) {
		transpose_layer(layer);
		reverse_layer_rows(layer);	
	}

	else if (degrees == -90) {
		reverse_layer_rows(layer);
		transpose_layer(layer);	
	}
}

std::vector<std::vector<SceneNode**>> CubeDisplay::find_layer(const cube::Face face, const int layer) {
	using namespace cube;
	
	std::vector<std::vector<SceneNode**>> result;
	auto layer_coords = [this](const int i, const int j, const Face face, const int layer) {
		Vector3 coords = {0,0,0};
		switch (face) {
			case Face::FRONT:
				coords[0] = i;
				coords[1] = j;
				coords[2] = this->cube.size() - 1 - layer;
				break;
			case Face::BACK:
				coords[0] = i;
				coords[1] = j;
				coords[2] = layer;
				break;
			case Face::TOP:
				coords[0] = j;
				coords[1] = this->cube.size() - 1 - layer;
				coords[2] = i;
				break;
			case Face::BOTTOM:
				coords[0] = j;
				coords[1] = layer;
				coords[2] = i;
				break;
			case Face::RIGHT:
				coords[0] = this->cube.size() - 1 - layer;
				coords[1] = i;
				coords[2] = j;
				break;
			case Face::LEFT:
				coords[0] = layer;
				coords[1] = i;
				coords[2] = j;
				break;

		};

		return coords;
	};
	for (int i = 0; i < cube.size(); i++) {
		result.push_back(std::vector<SceneNode**>());
		for (int j = 0; j < cube.size(); j++) {
			auto coords = layer_coords(i,j,face,layer);
			result[i].push_back(&cube[coords[0]][coords[1]][coords[2]]);	
		}	
	}

	return result;
}
