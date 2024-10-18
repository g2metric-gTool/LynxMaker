#pragma once

#include <list> 
#include "gParameters.h"

struct Connection {
	static std::list<gCore::Parameters> discover(); // TODO: En faire une interface pour tous les IDevice
};
