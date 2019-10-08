#ifndef TWIST_H
#define TWIST_H

#include <cassert>

namespace cube {
	enum class Face;

	//specifies a twist made to a cube
	struct Twist {
		//specifies the rotation of the twist - either 90 or -90 degrees
		int degrees;
		
		//specifies the side of the cube to be rotated
		Face face;

		//the layer, or 'slice' of the cube to rotate. Layers are numbered starting at zero, the outermost face,
		//and moving inwards
		int layer;

		//if wide_turn is true, all layers less than or equal to the aforementioned 'layer' variable are rotated
		//simultaneously. Otherwise, only the layer specified by the 'layer' variable is rotated
		bool wide_turn;

		//specifies a rotation on the outermost layer of the given face
		Twist(const int degrees, const Face face) : face(face), degrees(degrees), layer(0), wide_turn(false) {
			assert(degrees == 90 || degrees == -90 && "Degrees parameter must be 90 or -90");
		}
		
		//specifies a given rotation on the layer specified by the 'face' and 'layer' paramaters. If wide_turn is true,
		//all layers less than and equal to the provided layer paramater are rotated. Otherwise, only the layer specified by
		//the layer attribute are rotated
		Twist(const int degrees, const Face face, const int layer, const bool wide_turn=true) : 
			wide_turn(wide_turn), face(face), degrees(degrees), layer(layer) {
				assert(degrees == 90 || degrees == -90 && "Degrees parameter must be 90 or -90");
			}

		bool operator==(const Twist& twist) const {
			return  degrees == twist.degrees 
				&& face == twist.face
				&& layer == twist.layer 
				&& wide_turn == twist.wide_turn;
		}

		bool operator!=(const Twist& twist) const {return !(*this == twist);}
	};
}

#endif
