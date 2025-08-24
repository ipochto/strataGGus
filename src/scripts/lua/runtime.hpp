#pragma once

#include <set>
#include <string_view>
#include <vector>

#include "sol2.hpp"
#include "utils/optional_ref.hpp"


class LuaState;

// Sandboxed lua runtime
class LuaRuntime
{
public:
	enum class Presets {Empty, Configs};

	explicit LuaRuntime(LuaState &lua, Presets preset = Presets::Empty);

	

private:
	using NamesList = std::vector<std::string_view>;
	using LibsList = std::vector<sol::lib>;
	using SandboxPresetsMap = std::unordered_map<Presets, LibsList>;

	struct LibSymbolsRules {
		bool allowedAllExceptRestricted {false};
		NamesList allowed {}; // This will be ignored if allowedAllExceptRestricted is set
		NamesList restricted {};
	};

	using LibsSandboxingRulesMap = std::unordered_map<sol::lib, LibSymbolsRules>;

	auto checkRulesFor(sol::lib lib)-> opt_cref<LibSymbolsRules>;
	void loadLibs(const LibsList &names);
	void loadLib(sol::lib lib);
	void addLibToSandbox(sol::lib lib, const LibSymbolsRules &rules);
	
	//	sol::function_result loadfile(sol::stack_object file);
	//	sol::function_result dofile(sol::stack_object file);

	LuaState &luaRT;
	sol::table sandbox;
	std::set<sol::lib> loadedLibs;

	static const SandboxPresetsMap sandboxPresets;
	static const LibsSandboxingRulesMap libsSandboxingRules;
};

inline const LuaRuntime::SandboxPresetsMap
LuaRuntime::sandboxPresets {
	{Presets::Empty, {}},
	{Presets::Configs, {
			sol::lib::base,
			sol::lib::table,
			sol::lib::string}}
};

inline const LuaRuntime::LibsSandboxingRulesMap
LuaRuntime::libsSandboxingRules {
	{sol::lib::base, {
		.allowed = {"assert", "error", "ipairs", "next", "pairs", "pcall", "select",
					"tonumber", "tostring", "type", "unpack", "_VERSION", "xpcall"}}},
	{sol::lib::os, {
		.allowed = {"clock", "date", "difftime", "time"}}},
	{sol::lib::math, {
		.allowedAllExceptRestricted = true,
		.restricted = {"random", "randomseed"}}},
	{sol::lib::string, {
		.allowedAllExceptRestricted = true,
		.restricted = {"dump"}}},
	{sol::lib::coroutine, {
		.allowedAllExceptRestricted = true}},
};
