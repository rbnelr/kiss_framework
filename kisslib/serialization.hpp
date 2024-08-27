#pragma once
#include "nlohmann/json_fwd.hpp"
#include "nlohmann/json.hpp"
#include "file_io.hpp"
#include "kissmath.hpp"
#include "tracy/Tracy.hpp"
#include <memory>
using json = nlohmann::ordered_json;

#ifndef SERIALIZE_LOG
	#include "stdio.h"
	#define SERIALIZE_LOG(type, ...) fprintf(stderr, __VA_ARGS__)
#endif

inline bool save_json (char const* filename, json const& json) {
	std::string json_str;
	try {
		ZoneScopedN("json::dump()");
		json_str = json.dump(1, '\t');
	} catch (std::exception& ex) {
		SERIALIZE_LOG(ERROR, "Error when serializing something: %s", ex.what());
		return true;
	}
	{
		ZoneScopedN("kiss::save_text_file()");
		if (!kiss::save_text_file(filename, json_str)) {
			SERIALIZE_LOG(ERROR, "Error when serializing something: Can't save file \"%s\"", filename);
			return false;
		}
	}
	return true;
}

template <typename T>
inline bool serialize (char const* filename, T const& obj) {
	ZoneScoped;
	json json = obj;
	return save_json(filename, json);
}

inline bool load_json (char const* filename, json* j) {

	std::string str;
	{
		ZoneScopedN("kiss::load_text_file()");
		if (!kiss::load_text_file(filename, &str)) {
			SERIALIZE_LOG(WARNING, "[load_json] Can't load file \"%s\", using defaults.", filename);
			return false;
		}
	}
	try {
		ZoneScopedN("json::parse()");
		*j = json::parse(str, nullptr, true, true); // last arg: ignore_comments
	} catch (std::exception& ex) {
		SERIALIZE_LOG(ERROR, "[load_json] Error in json::parse: %s", ex.what());
		return false;
	}
	return true;
}

inline json load_json (char const* filename) {
	json j;
	load_json(filename, &j);
	return j;
}

template <typename T>
inline bool deserialize (char const* filename, T* obj) {
	ZoneScoped;

	try {
		json json;
		if (load_json(filename, &json)) {
			json.get_to(*obj);
			return true;
		}
	} catch (std::exception& ex) {
		SERIALIZE_LOG(ERROR, "Error when deserializing something: %s", ex.what());
	}
	return false;
}

template <typename T>
inline T deserialize (char const* filename) {
	T t = T();
	deserialize(filename, &t);
	return t;
}

// Terrifying macro abomination GO!

#define _JSON_TO(v1) j[#v1] = t.v1;
#define _JSON_FROM(v1) if (j.contains(#v1)) j.at(#v1).get_to(t.v1); // try get value from json

#define _JSON_EXPAND( x ) x
#define _JSON_GET_MACRO(_0,_1,_2,_3,_4,_5,_6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, NAME, ...) NAME

#define _JSON_PASTE(...) _JSON_EXPAND(_JSON_GET_MACRO(__VA_ARGS__, \
	_JSON_PASTE20, _JSON_PASTE19, _JSON_PASTE18, _JSON_PASTE17, _JSON_PASTE16, _JSON_PASTE15, _JSON_PASTE14, _JSON_PASTE13, _JSON_PASTE12, _JSON_PASTE11, \
	_JSON_PASTE10, _JSON_PASTE9, _JSON_PASTE8, _JSON_PASTE7, _JSON_PASTE6, _JSON_PASTE5, _JSON_PASTE4, _JSON_PASTE3, _JSON_PASTE2, _JSON_PASTE1, _JSON_PASTE0 \
	)(__VA_ARGS__))

#define _JSON_PASTE0(func)
#define _JSON_PASTE1(func,  v1)                                                                                         func(v1)
#define _JSON_PASTE2(func,  v1, v2)                                                                                     func(v1) _JSON_PASTE1(func, v2)
#define _JSON_PASTE3(func,  v1, v2, v3)                                                                                 func(v1) _JSON_PASTE2(func, v2, v3)
#define _JSON_PASTE4(func,  v1, v2, v3, v4)                                                                             func(v1) _JSON_PASTE3(func, v2, v3, v4)
#define _JSON_PASTE5(func,  v1, v2, v3, v4, v5)                                                                         func(v1) _JSON_PASTE4(func, v2, v3, v4, v5)
#define _JSON_PASTE6(func,  v1, v2, v3, v4, v5, v6)                                                                     func(v1) _JSON_PASTE5(func, v2, v3, v4, v5, v6)
#define _JSON_PASTE7(func,  v1, v2, v3, v4, v5, v6, v7)                                                                 func(v1) _JSON_PASTE6(func, v2, v3, v4, v5, v6, v7)
#define _JSON_PASTE8(func,  v1, v2, v3, v4, v5, v6, v7, v8)                                                             func(v1) _JSON_PASTE7(func, v2, v3, v4, v5, v6, v7, v8)
#define _JSON_PASTE9(func,  v1, v2, v3, v4, v5, v6, v7, v8, v9)                                                         func(v1) _JSON_PASTE8(func, v2, v3, v4, v5, v6, v7, v8, v9)
#define _JSON_PASTE10(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10)                                                    func(v1) _JSON_PASTE9(func, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define _JSON_PASTE11(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)                                               func(v1) _JSON_PASTE10(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define _JSON_PASTE12(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)                                          func(v1) _JSON_PASTE11(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define _JSON_PASTE13(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)                                     func(v1) _JSON_PASTE12(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define _JSON_PASTE14(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)                                func(v1) _JSON_PASTE13(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define _JSON_PASTE15(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)                           func(v1) _JSON_PASTE14(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define _JSON_PASTE16(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)                      func(v1) _JSON_PASTE15(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define _JSON_PASTE17(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)                 func(v1) _JSON_PASTE16(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define _JSON_PASTE18(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)            func(v1) _JSON_PASTE17(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define _JSON_PASTE19(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)       func(v1) _JSON_PASTE18(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define _JSON_PASTE20(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)  func(v1) _JSON_PASTE19(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)

#define SERIALIZE_TO_JSON(Type) void to_json(nlohmann::ordered_json& j, const Type& t)
#define SERIALIZE_FROM_JSON(Type) void from_json(const nlohmann::ordered_json& j, Type& t)

// Cannot handle zero arguments because preprocessor is badly designed

#define SERIALIZE_TO_JSON_EXPAND(...)   _JSON_EXPAND(_JSON_PASTE(_JSON_TO, __VA_ARGS__))
#define SERIALIZE_FROM_JSON_EXPAND(...) _JSON_EXPAND(_JSON_PASTE(_JSON_FROM, __VA_ARGS__))

#define SERIALIZE(Type, ...)  \
    friend SERIALIZE_TO_JSON(Type)   { SERIALIZE_TO_JSON_EXPAND(__VA_ARGS__)   } \
    friend SERIALIZE_FROM_JSON(Type) { SERIALIZE_FROM_JSON_EXPAND(__VA_ARGS__) }

#define SERIALIZE_OUT_OF_CLASS(Type, ...)  \
    SERIALIZE_TO_JSON(Type)   { SERIALIZE_TO_JSON_EXPAND(__VA_ARGS__)   } \
    SERIALIZE_FROM_JSON(Type) { SERIALIZE_FROM_JSON_EXPAND(__VA_ARGS__) }

// Macros can't handle no arguments, instead use this version if class gets serialized somehere but calls does not want to serialize any members
// assign an ampty object because otherside j ends up as null
#define SERIALIZE_NONE(Type, ...)  \
    friend SERIALIZE_TO_JSON(Type)   { j = nlohmann::ordered_json::object(); } \
    friend SERIALIZE_FROM_JSON(Type) {}

namespace nlohmann {
	// It's kinda unsafe to allocate pointers through deserialization, since we don't know if the code might not already have raw copies of this poiner somewhere
	// (Think unique pointer for objects, raw pointers to store selected object), some people might argue to use shared_ptr/weak_ptr in that case but I disagree
	/*
	template<typename T>
	struct adl_serializer<std::unique_ptr<T>> {
		using type = std::unique_ptr<T>;
		static void to_json(ordered_json& j, const type& val) {
			j = *val;
		}
		static void from_json(const ordered_json& j, type& val) {
			val = std::make_unique<T>();
			j.get_to(*val);
		}
	};
	*/
	template<typename T>
	struct adl_serializer<std::unique_ptr<T>> {
		using type = std::unique_ptr<T>;
		static void to_json(ordered_json& j, const type& val) {
			j = *val; // This is safe
		}
		static void from_json(const ordered_json& j, type& val) {
			// Just don't deserialize if ptr is null, this is safe, but might not be what you want
			// Think std::vector<std::unique_ptr<T>>, where depending on what's in the file, you might want the vector to be filled with new ptrs
			// Could override vectors of uptrs specifically, or just manually deserialize?
			if (val) j.get_to(*val);
		}
	};

	template <>	struct adl_serializer<int2> {
		using type = int2;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
		}
	};

	template <>	struct adl_serializer<int3> {
		using type = int3;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
			if (len >= 3) j.at(2).get_to(val.z);
		}
	};

	template <>	struct adl_serializer<int4> {
		using type = int4;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
			if (len >= 3) j.at(2).get_to(val.z);
			if (len >= 3) j.at(3).get_to(val.w);
		}
	};

	template <>	struct adl_serializer<float2> {
		using type = float2;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
		}
	};

	template <>	struct adl_serializer<float3> {
		using type = float3;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y, val.z };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
			if (len >= 3) j.at(2).get_to(val.z);
		}
	};

	template <>	struct adl_serializer<float4> {
		using type = float4;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y, val.z, val.w };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
			if (len >= 3) j.at(2).get_to(val.z);
			if (len >= 3) j.at(3).get_to(val.w);
		}
	};

	template <>	struct adl_serializer<srgb8> {
		using type = srgb8;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y, val.z };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
			if (len >= 3) j.at(2).get_to(val.z);
		}
	};
	template <>	struct adl_serializer<srgba8> {
		using type = srgba8;
		static void to_json(ordered_json& j, const type& val) {
			j = { val.x, val.y, val.z, val.w };
		}
		static void from_json(const ordered_json& j, type& val) {
			if (!j.is_array()) return;
			size_t len = j.size();
			if (len >= 1) j.at(0).get_to(val.x);
			if (len >= 2) j.at(1).get_to(val.y);
			if (len >= 3) j.at(2).get_to(val.z);
			if (len >= 3) j.at(3).get_to(val.w);
		}
	};
}

#undef SERIALIZE_LOG
