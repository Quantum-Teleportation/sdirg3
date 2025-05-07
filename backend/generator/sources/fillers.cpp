#include "fillers.h"
#include <sstream>

namespace terraformer {

void RectNoReflectFiller::Serialize_(std::stringstream &out) const {
	out << "axis = " << static_cast<int>(axis) << std::endl;
	out << "side = " << static_cast<int>(side) << std::endl;
}

} // namespace terraformer
