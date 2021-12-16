#pragma once

#include <stdio.h>
#include <stdlib.h>

template <typename T>
class vec
{
public:
	vec();
	~vec();
private:
	T* pointer = nullptr;
	int capacity = 0;
	int size = 0;
public:
	T& operator [] (const int& index) { return pointer[index]; };
	void init(int count);
	void add(const T& element);
	void pop();
	void clear();
	void erase(int index);
	void destroy();
public:
	int get_size();
	int get_capacity();
public:
	T* iter_begin();
	T* iter_end();
};

template<typename T>
inline vec<T>::vec()
{
	this->pointer = nullptr;
	this->capacity = 0;
	this->size = 0;
}

template<typename T>
inline vec<T>::~vec()
{
	this->destroy();
}

template<typename T>
inline void vec<T>::init(int count)
{
	if (this->pointer != nullptr) return;
	this->pointer = (T*)malloc(count * sizeof(T));
	this->size = 0;
	this->capacity = count;
}

template<typename T>
inline void vec<T>::add(const T& element)
{
	if (this->size >= this->capacity) {
		T* new_pointer = (T*)realloc(this->pointer, (size_t)this->capacity * (size_t)2 * sizeof(T));
		if (new_pointer == NULL) return;
		this->pointer = new_pointer;
		this->capacity = this->capacity * 2;
	}
	++this->size;
	this->pointer[size - 1] = element;
}

template<typename T>
inline void vec<T>::pop()
{
	--this->size;
	this->size = std::max(size, 0);
}

template<typename T>
inline void vec<T>::clear()
{
	this->size = 0;
}

template<typename T>
inline void vec<T>::erase(int index)
{
	if (this->size <= 0) return;
	if (index < 0 || index >= this->size) return;
	for (int i = index; i < this->size - 1; ++i) {
		pointer[i] = pointer[i + 1];
	}
	pop();
}

template<typename T>
inline void vec<T>::destroy()
{
	if (this->pointer == nullptr) return;
	free(this->pointer);
	this->pointer = nullptr;
}

template<typename T>
inline int vec<T>::get_size()
{
	return this->size;
}

template<typename T>
inline int vec<T>::get_capacity()
{
	return this->capacity;
}

template<typename T>
inline T* vec<T>::iter_begin()
{
	return this->pointer;
}

template<typename T>
inline T* vec<T>::iter_end()
{
	return this->pointer + this->size;
}
