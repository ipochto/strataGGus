#include "lua/state.hpp"
#include "lua/runtime.hpp"

LuaRuntime::LuaRuntime(LuaState &lua, Presets preset/* = Presets::Empty */) : luaRT(lua)
{
	sandbox = lua.state.create_table();
	sandbox["_G"] = sandbox;
	loadLibs(sandboxPresets.at(preset));
};

void LuaRuntime::loadLibs(const LibsList &libs)
{
	for (const auto lib : libs) {
		loadLib(lib);
	}
}

auto LuaRuntime::checkRulesFor(sol::lib lib)
	-> opt_cref<LibSymbolsRules> 
{
	const auto it = libsSandboxingRules.find(lib);
	if (it == libsSandboxingRules.end()) {
		return  std::nullopt;
	}
	return it->second;
}

void LuaRuntime::loadLib(sol::lib lib)
{
	const auto rules = checkRulesFor(lib);
	if (!rules) {
		return;
	}
	luaRT.require(lib);
	addLibToSandbox(lib, *rules);
}

void LuaRuntime::addLibToSandbox(sol::lib lib, const LibSymbolsRules &rules)
{
	const auto libName = lua::libName(lib);
	if (!libName) {
		return;
	}
	const auto libLookupName = (lib == sol::lib::base) ? "_G" : *libName;
	
	const sol::table src = luaRT.state[libLookupName];
	if (!src.valid()) {
		return;
	}
	sol::table dst = sandbox[libLookupName];

	if (rules.allowedAllExceptRestricted) { 
		for (const auto &[name, object]: src) {
			dst[name] = object;
		}
	} else {
		for (const auto &name : rules.allowed) {
			dst[name] = src[name];
		}
	}
	for (const auto &name : rules.restricted) {
		dst[name] = sol::nil;
	}
}
