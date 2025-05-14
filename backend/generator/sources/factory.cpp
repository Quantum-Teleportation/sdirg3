#include "factory.h"

namespace terraformer {

void RectGridFactory::Serialize_(std::stringstream &out) const {
	out << "size = " << size_.first << ", " << size_.second << std::endl;
	out << "origin = " << origin_.first << ", " << origin_.second << std::endl;
	out << "spacing = " << spacing_.first << ", " << spacing_.second
		<< std::endl;
}

} // namespace terraformer