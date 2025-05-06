
#ifndef FILLERS_H
#define FILLERS_H

#include "helper.h"
#include <string>

namespace terraformer {

using namespace std::string_literals;

DEFINE_BASE(filler);

class RectNoReflectFiller : public filler {
public:
	enum Side { SIDE_LEFT = 0, SIDE_RIGHT = 1, SIDE_COUNT } side;
	enum Axis { AXIS_X = 0, AXIS_Y = 1, AXIS_COUNT } axis;

	RectNoReflectFiller() {}
	RectNoReflectFiller(int s, int a)
		: side(static_cast<Side>(s)), axis(static_cast<Axis>(a)) {}
	// std::vector<std::pair<std::string, >>
private:
	void Serialize_(std::stringstream &) const override;
};

} // namespace terraformer

#endif