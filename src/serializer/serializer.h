#pragma once

#include "binary/Serializer.h"
#include "json/Serializer.h"
#include "yaml/Serializer.h"

#include "standardTypes.h"

#ifdef ABUILD_UNITS
	#include <units/units.h>
	#include "unitTypes.h"
#endif
#ifdef ABUILD_ARMADILLO_BITS
	#include <armadillo>
	#include "armaTypes.h"
#endif

