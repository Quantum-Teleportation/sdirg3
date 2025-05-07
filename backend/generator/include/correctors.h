#ifndef CORRECTORS_H
#define CORRECTORS_H

#include "helper.h"
#include <string>

namespace terraformer {

using namespace std::string_literals;

class Corrector : public BaseHelper {
public:
	Corrector() : BaseHelper("corrector") {}
	virtual ~Corrector() = 0;
};

class PointSourceCorrector2D : public Corrector {
public:
private:
	// only append in string
	virtual void Serialize_(std::stringstream &) const override {}
};

class ForceRectElasticBoundary2D : public Corrector {
public:
private:
	// only append in string
	virtual void Serialize_(std::stringstream &) const override {}
};

} // namespace terraformer

#endif