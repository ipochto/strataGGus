#include "lua/state.hpp"


void LuaState::require(sol::lib lib)
{ 
	if (not isLibraryLoaded(lib)) {
		loadLibrary(lib);
	}
}

void LuaState::loadLibrary(sol::lib lib)
{ 
	state.open_libraries(lib);
	loadedLibs.insert(lib);
}
