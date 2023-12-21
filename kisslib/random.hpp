#pragma once
#include "kissmath.hpp"
#include <vector>
#include <unordered_map>
#include <random>

template <typename ENGINE>
struct _Random {
	ENGINE generator;

	_Random (); // seed with random value from global rng
	_Random (uint64_t seed): generator{(unsigned int)seed} {} // seed with value

	inline uint32_t uniform_u32 () {
		static_assert(sizeof(uint32_t) == sizeof(int), "");
		std::uniform_int_distribution<int> distribution (std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
		return (uint32_t)distribution(generator);
	}
	inline uint64_t uniform_u64 () {
		return (uint64_t)uniform_u32() << 32ull | (uint64_t)uniform_u32();
	}

	inline bool chance (float prob=0.5f) {
		std::bernoulli_distribution	distribution (prob);
		return distribution(generator);
	}

	inline int uniformi (int min, int max) {
		std::uniform_int_distribution<int> distribution (min, max -1);
		return distribution(generator);
	}

	inline float uniformf (float min=0, float max=1) {
		std::uniform_real_distribution<float>	distribution (min, max);
		return distribution(generator);
	}

	inline float normalf (float stddev, float mean=0) {
		std::normal_distribution<float> distribution (mean, stddev);
		return distribution(generator);
	}

	inline int2   uniform2i (int2   min  , int2   max   ) { return int2(  uniformi(min.x, max.x), uniformi(min.y, max.y)); }
	inline float2 uniform2f (float2 min=0, float2 max=1 ) { return float2(uniformf(min.x, max.x), uniformf(min.y, max.y)); }
	inline float2 normal2f  (float2 stddev, float2 mean=0) { return float2(normalf(stddev.x, mean.x), normalf(stddev.y, mean.y)); }

	inline int3   uniform3i (int3   min  , int3   max   ) { return int3(  uniformi(min.x, max.x), uniformi(min.y, max.y), uniformi(min.z, max.z)); }
	inline float3 uniform3f (float3 min=0, float3 max=1 ) { return float3(uniformf(min.x, max.x), uniformf(min.y, max.y), uniformf(min.z, max.z)); }
	inline float3 normal3f  (float3 stddev, float3 mean=0) { return float3(normalf(stddev.x, mean.x), normalf(stddev.y, mean.y), normalf(stddev.z, mean.z)); }

	inline int4   uniform4i (int4   min  , int4   max   ) { return int4(  uniformi(min.x, max.x), uniformi(min.y, max.y), uniformi(min.z, max.z), uniformi(min.w, max.w)    ); }
	inline float4 uniform4f (float4 min=0, float4 max=1 ) { return float4(uniformf(min.x, max.x), uniformf(min.y, max.y), uniformf(min.z, max.z), uniformf(min.w, max.w)    ); }
	inline float4 normal4f  (float4 stddev, float4 mean=0) { return float4(normalf(stddev.x, mean.x), normalf(stddev.y, mean.y), normalf(stddev.z, mean.z), normalf(stddev.w, mean.w)); }

	// TODO: replace with spherical coordinates

	// random point uniformly distributed in unit sphere
	inline float3 uniform_in_sphere () {
	#if 0
		float3 pos;
		do {
			pos = uniform3f(-1.0f, +1.0f);
		} while (length_sqr(pos) > 1.0f);
		return pos;
	#else
		std::uniform_real_distribution<float>	distribution (0.0f, 1.0f);

		float azim =      distribution(generator) * PI*2.0f;
		float elev = acos(distribution(generator) * 2.0f - 1.0f);
		float radius = cbrtf(distribution(generator)); // cbrtf = cube root

		float ac = cos(azim), as = sin(azim);
		float ec = cos(elev), es = sin(elev);

		return float3(radius * ac*es, radius * as*es, radius * ec);
	#endif
	}

	// random point uniformly distributed on surface of unit sphere
	inline float3 uniform_direction () {
	#if 0
		float3 pos;
		float len;
		do {
			pos = uniform3f(-1.0f, +1.0f);
			len = length_sqr(pos);
		} while (len > 1.0f || len == 0.0f);
		
		return pos / sqrt(len);
	#else
		std::uniform_real_distribution<float>	distribution (0.0f, 1.0f);

		float azim =      distribution(generator) * PI*2.0f;
		float elev = acos(distribution(generator) * 2.0f - 1.0f);

		float ac = cos(azim), as = sin(azim);
		float ec = cos(elev), es = sin(elev);

		return float3(ac*es, as*es, ec);
	#endif
	}

	// random uniform direction with random uniform length in some range
	// could be used as random velocity
	inline float3 uniform_vector (float min_magnitude=0, float max_magnitude=1) {
		return uniform_direction() * uniformf(min_magnitude, max_magnitude);
	}

};

typedef _Random<std::default_random_engine> Random;

template <typename IT, typename FUNC>
struct WeightedChoice {
	// TODO: could be optimized using a binary search

	IT begin, end;
	FUNC get_weight;
	float total;

	WeightedChoice (IT begin, IT end, FUNC get_weight): begin{begin}, end{end}, get_weight{get_weight} {
		float accum = 0;
		for (auto it=begin; it!=end; ++it) {
			accum += get_weight(it);
		}
		total = accum;
	}

	template <typename RAND>
	IT get_random (RAND& rand) {
		if (begin == end)
			return end;

		float val = rand.uniformf(0.0f, total);
		
		float accum = 0;
		for (auto it=begin; it!=end; ++it) {
			accum += get_weight(it);
			if (val < accum)
				return it;
		}
		return --end;
	}
};

template <typename K, typename V, typename FUNC>
WeightedChoice< typename std::unordered_map<K,V>::iterator, FUNC > weighted_choice (std::unordered_map<K,V>& map, FUNC get_weight) {
	return WeightedChoice(map.begin(), map.end(), get_weight);
}
template <typename T, typename FUNC>
WeightedChoice< typename std::vector<T>::iterator, FUNC > weighted_choice (std::vector<T>& vec, FUNC get_weight) {
	return WeightedChoice(vec.begin(), vec.end(), get_weight);
}

uint64_t get_initial_random_seed ();

// Global random number generator
inline Random random = Random( get_initial_random_seed() );

template <typename T>
_Random<T>::_Random (): _Random(random.uniform_u32()) {} // just use 32 bits because engine only takes u32 seed anyway
