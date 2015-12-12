#pragma once

#include "binary/Serializer.h"
#include "json/Serializer.h"
#include "yaml/Serializer.h"

#include "standardTypes.h"

#ifdef ABUILD_UNITS
	#include "unitTypes.h"
#endif
#ifdef ABUILD_ARMADILLO_BITS
	#include "armaTypes.h"
#endif

