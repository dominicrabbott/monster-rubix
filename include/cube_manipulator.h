#ifndef CUBE_MANIPULATOR_H
#define CUBE_MANIPULATOR_H

#include <vector>

namespace cube {
	enum class Face;

	//performs nessesary matrix operations to manipulate a cube
	template <typename T>
	class CubeManipulator {
		private:
			//transposes the given square matrix. Pointers to the elements to be transposed are accepted as arguments
			static void transpose(const std::vector<std::vector<T*>>& matrix);
			
			//reverses the rows in the given matrix. Pointers to the elements to be reversed are accepted as arguments
			static void reverse_rows(const std::vector<std::vector<T*>>& matrix);
		public:
			//performs the given rotation on the given matrix. Pointers to the elements that are to be
			//rotated are accepted as arguments
			static void rotate(const std::vector<std::vector<T*>>& matrix, const int degrees);

			//returns a matrix that contains pointers to the elements that make up the specified layer in the given cube
			static std::vector<std::vector<T*>> find_layer(std::vector<std::vector<std::vector<T>>>& cube, const Face face, const int layer);
	};
}

#endif
