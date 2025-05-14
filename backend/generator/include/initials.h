#ifndef INITIALS_H
#define INITIALS_H

#include "helper.h"
#include <string>

namespace terraformer {

using namespace std::string_literals;

class Initial : public BaseHelper {
public:
	Initial() : BaseHelper("initial") {}
	virtual ~Initial() = 0;
};

inline Initial::~Initial() {}

class StructuredFileLoader : public Initial {
	DEFINE_CLASSNAME(StructuredFileLoader);

public:
	std::string value_ = "rho";						 // = rho
	std::string path_ = "main_"s + value_ + ".bin"s; //  = main_rho.bin

	bool binary_ = true; // = true
	size_t order_ = 0;	 // 3

	StructuredFileLoader() {}

	StructuredFileLoader *Init(const std::string &value, const std::string path,
							   bool binary, size_t order) {
		value_ = value;
		path_ = path;
		binary_ = binary;
		order_ = order;
		return this;
	}

private:
	void Serialize_(std::stringstream &) const override;
};

using InitialFactory = HelperFactory<Initial>;
REGISTER_CLASS(Initial, StructuredFileLoader);

} // namespace terraformer

#endif