#pragma once

#include "node.h"

constexpr int GROUP_MAX_NODE = 1600;

class Layer
{
public:
	~Layer();
public:
	vec<vec<Node>> data;
public:
	void init(int group_count);
	void add(const Node& element);
	void pop(int group_index);
	void clear();
public:
	int max_group();
	int get_size();
	int get_capicity();
};

