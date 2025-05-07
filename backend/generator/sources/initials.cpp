#include "initials.h"
#include <sstream>

namespace terraformer {

void StructuredFileLoader::Serialize_(std::stringstream &out) const {
	out << "path = " << path_ << std::endl;
	out << "value = " << value_ << std::endl;
	out << "binary = " << (binary_?"true":"false") << std::endl;
	out << "order = " << order_ << std::endl;
}

} // namespace terraformer
