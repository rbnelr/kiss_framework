#pragma once
#include "kissmath/output/int3.hpp"
#include "macros.hpp"
#include "assert.h"

template <typename T>
struct array3D {
	MOVE_ONLY_CLASS(array3D)
public:

	T*		data = nullptr;
	int3	size = 0;

	friend void swap (array3D& l, array3D& r) {
		std::swap(l.data, r.data);
		std::swap(l.size, r.size);
	}

	array3D () {}

	array3D (int3 const& size) {
		resize(size);
	}

	~array3D () {
		if (data) ::free(data);
	}

	void resize (int3 new_size) {
		size = new_size;
		if (data) ::free(data);
		data = (T*)malloc(sizeof(T) * size.x * size.y * size.z);
	}

	size_t count () {
		return size.z * size.y * size.x;
	}

	void clear (T const& val) {
		size_t n = count();

		if (sizeof(T) == 1) {
			memset(data, val, n);
		} else {
			for (size_t i=0; i<n; ++i) {
				data[i] = val;
			}
		}
	}

	size_t index (int x, int y, int z) {
		assert( (unsigned)x < (unsigned)size.x &&
			(unsigned)y < (unsigned)size.y &&
			(unsigned)z < (unsigned)size.z );
		return (z * size.y + y) * size.x + x;
	}

	T const& get (int x, int y, int z) const {
		return data[index(x,y,z)];
	}
	T& get (int x, int y, int z) {
		return data[index(x,y,z)];
	}

	T const& get (int3 const& pos) const {
		return get(pos.x, pos.y, pos.z);
	}
	T& get (int3 const& pos) {
		return get(pos.x, pos.y, pos.z);
	}
	T const& operator[] (int3 const& pos) const {
		return get(pos.x, pos.y, pos.z);
	}
	T& operator[] (int3 const& pos) {
		return get(pos.x, pos.y, pos.z);
	}
};


// like a queue but is just implemented as a flat array, push() pop() like queue, but a push when the size is at it's max capacity will pop() automatically
//  push() add the items to the "head" of the collection which is accessed via [0] (so everything shifts by one index in the process, but the implementation does not copy anything)
//  pop() removes the items from the "tail" of the collection which is accessed via [size() - 1]
template <typename T>
class circular_buffer {
	std::unique_ptr<T[]> arr = nullptr;
	size_t cap = 0;
	size_t head; // next item index to be written
	size_t cnt = 0;

public:
	circular_buffer () {}
	circular_buffer (size_t capacity) {
		resize(capacity);
	}

	T* data () {
		return arr.get();
	}
	size_t capacity () const {
		return cap;
	}
	size_t count () const {
		return cnt;
	}

	void resize (size_t new_capacity) {
		auto old = std::move(*this);

		cap = new_capacity;
		arr = cap > 0 ? std::make_unique<T[]>(cap) : nullptr;

		cnt = cap <= old.cnt ? cap : old.cnt;
		for (size_t i=0; i<cnt; ++i) {
			arr[i] = old.arr[(old.head -cnt + old.cap + i) % old.cap];
		}

		head = cnt % cap;
	}

	void push (T const& item) {
		assert(cap > 0);

		// write in next free slot or overwrite if count == cap
		arr[head++] = item;
		head %= cap;

		if (cnt < cap)
			cnt++;
	}
	void push (T&& item) {
		assert(cap > 0);

		// write in next free slot or overwrite if count == cap
		arr[head++] = std::move(item);
		head %= cap;

		if (cnt < cap)
			cnt++;
	}

	T pop () {
		assert(cap > 0 && cnt > 0);

		size_t tail = head + (cap - cnt);
		tail %= cap;

		cnt--;
		return std::move( arr[tail] );
	}

	// get ith oldest value
	T& get_oldest (size_t index) {
		assert(index >= 0 && index < cnt);
		size_t i = head + (cap - cnt);
		i += index;
		i %= cap;
		return arr[i];
	}

	// get ith newest value
	T& get_newest (size_t index) {
		assert(index >= 0 && index < cnt);
		size_t i = head + cap - 1;
		i -= index;
		i = (i + cap) % cap;
		return arr[i];
	}

	T& operator [] (size_t index) {
		return get_newest(index);
	}
};
