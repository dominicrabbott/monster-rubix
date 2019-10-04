#include <string>
#include <array>
#include <sstream>
#include <iostream>

#include "cube_display.h"
#include "cube_manipulator.h"
#include "Ogre.h"
#include "color.h"
#include "twist.h"

using namespace ui;
using namespace Ogre;


CubeDisplay::Rotation::Rotation(const cube::Twist& move, const int cube_size) {
	static std::unordered_map<cube::Face, cube::Face> opposing_faces = {
		{cube::Face::RIGHT, cube::Face::LEFT},
		{cube::Face::TOP, cube::Face::BOTTOM},
		{cube::Face::FRONT, cube::Face::BACK},
	};
	
	to_layer = move.layer;
	from_layer = move.wide_turn ? 0 : to_layer;
	
	if (opposing_faces.count(move.face) == 0) {
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
}

void CubeDisplay::rotate(const cube::Twist& move) {
	std::unique_lock<std::mutex> lock(mex);
	pending_rotations.push_back(Rotation(move, cube.size()));
}

void CubeDisplay::frameRendered(const Ogre::FrameEvent& event)  {
	mex.lock();
	int queue_size = pending_rotations.size();
	mex.unlock();

	if (queue_size > 0) {
		mex.lock();
		Rotation& curr_rot = pending_rotations[0];
		mex.unlock();

		if (curr_rot.remaining_degrees == curr_rot.total_degrees) {
			for (int i = curr_rot.from_layer; i<= curr_rot.to_layer; i++) {
				std::vector<std::vector<SceneNode**>> layer = CubeManipulator::find_layer(cube, curr_rot.axis, i);
				for (auto& ptr_vec : layer) {
					for (auto node_ptr : ptr_vec) {
						adopt_scene_node(*node_ptr, skeleton[curr_rot.axis][i]);
					}
				}
			}
		}
		
		int degrees_per_frame = curr_rot.total_degrees/15;

		for (int i = curr_rot.from_layer; i<= curr_rot.to_layer; i++) {
			skeleton[curr_rot.axis][i] -> roll(Degree(degrees_per_frame));
		}
		
		curr_rot.remaining_degrees -= degrees_per_frame;
		
		if (curr_rot.remaining_degrees == 0) {
			for (int i = curr_rot.from_layer; i <= curr_rot.to_layer; i++) {
				CubeManipulator::rotate(CubeManipulator::find_layer(cube, curr_rot.axis, i), curr_rot.total_degrees);
			}

			mex.lock();
			pending_rotations.pop_front();
			mex.unlock();
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
				SceneNode* cube_node = create_piece(create_piece_colors(Vector3(x,y,z), size));
				cube_node -> setPosition(x*piece_size-translation, y*piece_size-translation, z*piece_size-translation);

				cube[x][y].push_back(cube_node);
			}	
		}	
	}
}

std::unordered_map<cube::Face, Color> CubeDisplay::create_piece_colors(const Vector3 coords, const int size) {
	std::unordered_map<cube::Face, Color> result;

	if (coords[0] == 0) {
		result.insert(std::make_pair(cube::Face::LEFT, Color::GREEN));	
	}
	else if (coords[0] == size-1) {
		result.insert(std::make_pair(cube::Face::RIGHT, Color::BLUE));	
	}
	
	if (coords[1] == 0) {
		result.insert(std::make_pair(cube::Face::BOTTOM, Color::ORANGE));	
	}
	else if (coords[1] == size-1) {
		result.insert(std::make_pair(cube::Face::TOP, Color::RED));	
	}
	
	if (coords[2] == 0) {
		result.insert(std::make_pair(cube::Face::BACK, Color::WHITE));	
	}
	else if (coords[2] == size-1) {
		result.insert(std::make_pair(cube::Face::FRONT, Color::YELLOW));	
	}
	

	return result;
}

void CubeDisplay::adopt_scene_node(SceneNode* child, SceneNode* new_parent) {
	Vector3 global_pos = child -> convertLocalToWorldPosition(Vector3(0,0,0));
	Quaternion global_orientation = child -> convertLocalToWorldOrientation(Quaternion());
	
	child -> getParentSceneNode() -> removeChild(child);
	new_parent -> addChild(child);
	child -> setOrientation(new_parent -> convertWorldToLocalOrientation(global_orientation));
	child -> setPosition(new_parent -> convertWorldToLocalPosition(global_pos));
}

void CubeDisplay::create_skeleton(const int size) {
	SceneNode* root_node = scene_mgr -> getRootSceneNode();
	float axis_length = size-1;
	
	for (float f = -axis_length/2; f <= axis_length/2; f++) {
		SceneNode* skeleton_node_z = root_node -> createChildSceneNode();
		skeleton_node_z -> lookAt(Vector3(0,0,1), Node::TransformSpace::TS_LOCAL);
		skeleton_node_z -> setPosition(0,0,f*piece_size);

		SceneNode* skeleton_node_y = root_node -> createChildSceneNode();
		skeleton_node_y -> lookAt(Vector3(0,1,0), Node::TransformSpace::TS_LOCAL);
		skeleton_node_y -> setPosition(0,f*piece_size,0);

		SceneNode* skeleton_node_x = root_node -> createChildSceneNode();
		skeleton_node_x -> lookAt(Vector3(1,0,0), Node::TransformSpace::TS_LOCAL);
		skeleton_node_x -> setPosition(f*piece_size,0,0);

		skeleton[cube::Face::LEFT].push_back(skeleton_node_x);
		skeleton[cube::Face::BOTTOM].push_back(skeleton_node_y);
		skeleton[cube::Face::BACK].push_back(skeleton_node_z);
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
	
	static std::vector<std::vector<int>> verticies = {
		{0,0,0},//0
		{piece_size,0,0},//1
		{piece_size,0,piece_size},//2 	
		{0,0,piece_size},//3
		{0,piece_size,0},//4
		{piece_size,piece_size,0},//5
		{piece_size,piece_size,piece_size},//6
		{0,piece_size,piece_size},//7
	};

	static std::unordered_map<cube::Face, std::array<int, 4>> face_verticies = {	
		{cube::Face::FRONT, std::array<int, 4> {3,2,6,7}},
		{cube::Face::BACK, std::array<int, 4> {4,5,1,0}},
		{cube::Face::RIGHT, std::array<int, 4> {2,1,5,6}},
		{cube::Face::LEFT, std::array<int, 4> {0,3,7,4}},
		{cube::Face::TOP, std::array<int, 4> {7,6,5,4}},
		{cube::Face::BOTTOM, std::array<int, 4> {0,1,2,3}},
	};

	static std::vector<std::vector<int>> texture_verticies = {
		{0,0},	
		{1,0},	
		{1,1},	
		{0,1},	
	};

	static cube::Face faces[] = {cube::Face::FRONT, cube::Face::BACK, cube::Face::RIGHT, cube::Face::LEFT, cube::Face::TOP, cube::Face::BOTTOM};

	ManualObject* piece_entity = scene_mgr -> createManualObject();

	for (const auto face : faces) {
		std::string material = "black";
		if (piece.count(face) > 0) {
			material = materials[piece.at(face)];
		}
		
		piece_entity -> begin(material, RenderOperation::OT_TRIANGLE_LIST);

		for (int i = 0; i < 4; i++) {
			std::vector<int>& vertex = verticies[face_verticies[face][i]];
			piece_entity -> position(vertex[0], vertex[1], vertex[2]);
			piece_entity -> textureCoord(texture_verticies[i][0], texture_verticies[i][1]);	
		}

		piece_entity -> quad(0,1,2,3);
		piece_entity -> end();
		
	}

	SceneNode* result_node = scene_mgr -> getRootSceneNode() -> createChildSceneNode();
	result_node -> attachObject(piece_entity);

	return result_node;
}
