#ifndef CORRECTORS_H
#define CORRECTORS_H

#include "helper.h"
#include <string>

namespace terraformer {

using namespace std::string_literals;

DEFINE_BASE(corrector);

class PointSourceCorrector2D : public corrector {
public:
private:
	// only append in string
	virtual void Serialize_(std::stringstream &) const override {}
};

class ForceRectElasticBoundary2D : public corrector {
public:
private:
	// only append in string
	virtual void Serialize_(std::stringstream &) const override {}
};

} // namespace terraformer

#endif