#pragma once

#include "../Streaming.h"
#include "ControlClasses.h"
#include "ShapeBase.h"
#include "Templates.h"

namespace Engine
{
	namespace UI
	{
		namespace Loader
		{
			void LoadUserInterfaceFromBinary(InterfaceTemplate& Template, Streaming::Stream* Source, IResourceLoader* ResourceLoader, IResourceResolver* ResourceResolver);
		}
	}
}
