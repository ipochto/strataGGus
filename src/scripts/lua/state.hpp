#pragma once

#include <optional>
#include <set>
#include <string_view>
#include <unordered_map>

#include "sol2.hpp"

class LuaState
{
public:
	void require(sol::lib lib);

	sol::state state;

private:
	bool isLibraryLoaded(sol::lib lib) const { return loadedLibs.contains(lib); }
	void loadLibrary(sol::lib lib);

	std::set<sol::lib> loadedLibs;
};

namespace lua
{
	namespace details
	{
		using LibsLookupTable = std::unordered_map<sol::lib, std::string_view>;

		inline const LibsLookupTable libsLookupTable = {
			{sol::lib::base,		"base"},
			{sol::lib::package,		"package"},
			{sol::lib::coroutine,	"coroutine"},
			{sol::lib::string,		"string"},
			{sol::lib::os,			"os"},
			{sol::lib::math,		"math"},
			{sol::lib::table,		"table"},
			{sol::lib::debug,		"debug"},
			{sol::lib::bit32,		"bit32"}, // Lua 5.2+
			{sol::lib::io,			"io"},
			{sol::lib::ffi,			"ffi"}, // LuaJIT only
			{sol::lib::jit,			"jit"}, // LuaJIT only
			{sol::lib::utf8,		"utf8"} // Lua 5.3+
		};
	}

	inline auto libName(sol::lib lib)
		-> std::optional<std::string_view>
	{
		const auto &names = lua::details::libsLookupTable;
		const auto it = names.find(lib);
		if (it == names.end()) {
			return std::nullopt;
		}
		return it->second;
	}

	inline auto libByName(std::string_view libName)
		-> std::optional<sol::lib>
	{
		for (const auto [lib, name] : lua::details::libsLookupTable) {
			if (name == libName) {
				return lib;
			}
		}
		return std::nullopt;
	}
}

