#ifndef VEC_H
#define VEC_H

#include <stdio.h>
#include <stdlib.h>

/*
A very UNSAFE fixed-size vector for very special personal situations
Use with CAUTIONS, can causes UNDEFINED BEHAVIORS
*/
template <typename T>
class vec
{
public:
	vec();
	~vec();

public:
	T* pointer = nullptr; // This shouldn't be tampered unless must
	int max_size = 0; // This shouldn't be tampered unless must
	int size = 0; // This shouldn't be tampered unless must
	T& operator [] (const int& index) { return pointer[index]; };
	void init(int max); // Must call this once right after create vec
	void add(const T& element); // There will be 1 copy maked if use this
	void pop();
	void clear(); // This won't free memory
	void erase(int index); // This won't free memory
	void free(); // This will free memory
};

template<typename T>
inline vec<T>::vec()
{
	pointer = nullptr;
	max_size = 0;
	size = 0;
}

template<typename T>
inline vec<T>::~vec()
{
	free();
}

template<typename T>
inline void vec<T>::init(int max)
{
	if (pointer != nullptr) return;
	//pointer = (T*)malloc(max * sizeof(T));
	pointer = new T[max];
	size = 0;
	max_size = max;
}

template<typename T>
inline void vec<T>::add(const T& element)
{
	++size;
	size = std::min(size, max_size);
	pointer[size - 1] = element;
}

template<typename T>
inline void vec<T>::pop()
{
	--size;
	size = std::max(size, 0);
}

template<typename T>
inline void vec<T>::clear()
{
	size = 0;
}

template<typename T>
inline void vec<T>::erase(int index)
{
	if (size <= 0) return;
	if (index < 0 || index >= size) return;
	for (int i = index; i < size - 1; ++i) {
		pointer[index] = pointer[index + 1];
	}
	pop();
}

template<typename T>
inline void vec<T>::free()
{
	if (pointer == nullptr) return;
	//free(pointer);
	delete[] pointer;
	pointer = nullptr;
}

#endif // VEC_H
