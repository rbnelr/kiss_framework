#pragma once
#include "stdarg.h"
#include <string>
#include <string_view>
#include <regex>

namespace kiss {
	// Printf that appends to a std::string
	void vprints (std::string* s, char const* format, va_list vl);

	// Printf that appends to a std::string
	void prints (std::string* s, char const* format, ...);

	// Printf that outputs to a std::string
	std::string prints (char const* format, ...);

	bool starts_with (std::string_view str, std::string_view substr);
	bool contains (std::string_view str, std::string_view substr);

	// remove whitespace at front and back
	std::string_view trim (std::string_view sv);

	void to_upper_inplace (std::string& str);
	std::string to_upper (std::string_view str);

	// should be slightly more efficient than trying to do this in any other way, since multiple string 
	inline std::string concat (std::string_view const& a, std::string_view const& b) {
		std::string s;
		s.reserve(a.size() + b.size()); // null terminator is implicit see https://stackoverflow.com/questions/30111288/stdstringreserve-and-end-of-string-0
		s.insert(0, a);
		s.insert(a.size(), b);
		return s;
	}
	inline std::string concat (std::string_view const& a, std::string_view const& b, std::string_view const& c) {
		std::string s;
		s.reserve(a.size() + b.size() + c.size());
		size_t offs = 0;
		s.insert(offs, a); offs += a.size();
		s.insert(offs, b); offs += b.size();
		s.insert(offs, c);
		return s;
	}
	inline std::string concat (std::string_view const& a, std::string_view const& b, std::string_view const& c, std::string_view const& d) {
		std::string s;
		s.reserve(a.size() + b.size() + c.size() + d.size());
		size_t offs = 0;
		s.insert(offs, a); offs += a.size();
		s.insert(offs, b); offs += b.size();
		s.insert(offs, c); offs += c.size();
		s.insert(offs, d);
		return s;
	}
	inline std::string operator+ (std::string_view const& l, std::string_view const& r) {
		return concat(l, r);
	}

	inline std::vector<std::string_view> split (std::string_view const& str, char seperator) {
		std::vector<std::string_view> results;
		const char* cur = str.data();
		const char* end = str.data()+str.size();
		while (cur != end) {
			auto pos = (const char*)memchr(cur, seperator, end-cur);
			if (!pos) break;
			results.push_back(std::string_view(cur, pos-cur));
			cur = pos+1; // always exlucde seperator
		}
		results.push_back(std::string_view(cur, end-cur));
		return results;
	}
	// returns actual number of results
	inline int split (std::string_view const& str, char seperator, std::string_view* results, int max_results=0) {
		int count = 0;

		const char* cur = str.data();
		const char* end = str.data()+str.size();
		while (cur != end) {
			auto pos = (const char*)memchr(cur, seperator, end-cur);
			if (!pos) break;

			if (count < max_results)
				results[count++] = std::string_view(cur, pos-cur);

			cur = pos+1; // always exlucde seperator
		}

		if (count < max_results)
			results[count++] = std::string_view(cur, end-cur);

		return count;
	}

	template <typename FUNC>
	std::string regex_replace (std::string const& str, std::regex const& re, FUNC for_match) {
		size_t cur = 0;

		std::string out;

		for (auto it = std::sregex_iterator(str.begin(), str.end(), re); it != std::sregex_iterator(); ++it) {
			auto match = *it;

			out += str.substr(cur, match.position());
			cur = match.position() + match.length();

			out += for_match(match);
		}

		out += str.substr(cur, str.size());

		return out;
	}

#ifdef _WIN32
	// Prefer this being here rather than platform independed string.hpp
	std::basic_string<wchar_t> utf8_to_wchar (std::string_view utf8);
	std::string wchar_to_utf8 (std::basic_string_view<wchar_t> wchar);
#endif

	inline bool is_utf8_code (char c) {
		return (c & 0x80) != 0;
	}
	inline bool is_utf8_code (char const* str) {
		return (str[0] & 0x80) != 0;
	}

	inline int utf8_decode_single (char const* str, int* out_codepoint) { // TODO: test this!
		if ((str[0] & 0x80) == 0) {
			*out_codepoint = str[0]; // return ascii as int
			return 1;
		}

		int count;
		if (((int)str[0] & 0xE0) == 0xC0) {
			// two-byte encoding
			*out_codepoint = (((int)str[0] & 0x1F) << 6) | ((int)str[1] & 0x3F);
			count = 2;
		} else if (((int)str[0] & 0xF0) == 0xE0) {
			// three-byte encoding
			*out_codepoint = (((int)str[0] & 0x0F) << 6) | (((int)str[1] & 0x3F) << 12) | ((int)str[2] & 0x3F);
			count = 3;
		} else if (((int)str[0] & 0xF8) == 0xF0) {
			// four-byte encoding
			*out_codepoint = (((int)str[0] & 0x07) << 6) | (((int)str[1] & 0x3F) << 12) | (((int)str[2] & 0x3F) << 18) | ((int)str[3] & 0x3F);
			count = 4;
		} else {
			//assert(false);
			*out_codepoint = 0;
			count = 1;
		}
		return count;
	}
	inline int utf8_decode_single (char const*& str) {
		int codepoint;
		str += utf8_decode_single(str, &codepoint);
		return codepoint;
	}
	inline int _utf8_decode_single (char const* str) {
		int codepoint;
		utf8_decode_single(str, &codepoint);
		return codepoint;
	}
}
