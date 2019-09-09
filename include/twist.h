#ifndef TWIST_H
#define TWIST_H

namespace cube {
	enum class Face;

	//specifies a twist made to a cube
	class Twist {
		public:
			//specifies the rotation of the twist - either 90 or -90 degrees
			const int degrees;
			
			//specifies the side of the cube to be rotated
			const Face face;

			//the layer, or 'slice' of the cube to rotate. Layers are numbered starting at zero, the outermost face,
			//and moving inwards
			const int layer;

			//if wide_turn is true, all layers less than or equal to the aforementioned 'layer' variable are rotated
			//simultaneously. Otherwise, only the layer specified by the 'layer' variable is rotated
			const bool wide_turn;

			//specifies a rotation on the outermost layer of the given face
			Twist(const int degrees, const Face& face) : face(face), degrees(degrees), layer(0), wide_turn(false) {}
			
			//specifies a given rotation on the layer specified by the 'face' and 'layer' paramaters. If wide_turn is true,
			//all layers less than and equal to the provided layer paramater are rotated. Otherwise, only the layer specified by
			//the layer attribute are rotated
			Twist(const int degrees, const Face& face, const int layer, const bool wide_turn=true) : 
				wide_turn(wide_turn), face(face), degrees(degrees), layer(layer) {}
	};
}

#endif
