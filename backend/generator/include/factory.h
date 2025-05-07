#ifndef FACTORY_H
#define FACTORY_H

#include "helper.h"
#include <string>

namespace terraformer {

using namespace std::string_literals;

class Factory : public BaseHelper {
public:
	Factory() : BaseHelper("factory") {}
	virtual ~Factory() = 0;
};

inline Factory::~Factory() {}

class RectGridFactory : public Factory {
	DEFINE_CLASSNAME(RectGridFactory);

public:
	RectGridFactory() {}

	std::pair<size_t, size_t> size_ = std::make_pair<size_t, size_t>(2386, 828);
	std::pair<size_t, size_t> origin_ = std::make_pair<size_t, size_t>(0, 0);
	std::pair<float, float> spacing_ = std::make_pair<float, float>(1.6f, 1.6f);

private:
	void Serialize_(std::stringstream &) const override;
};

using FactoryFactory = HelperFactory<Factory>;
REGISTER_CLASS(Factory, RectGridFactory);

} // namespace terraformer

#endif
