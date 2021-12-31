#include "layer.h"

Layer::~Layer()
{
	for (int i = 0; i < this->data.get_size(); ++i) {
		this->data[i].destroy();
	}
	this->data.destroy();
}

void Layer::init(int group_count)
{
	this->data.init(group_count);
	for (int i = 0; i < group_count; ++i) this->data.add(vec<Node>());
	for (int i = 0; i < group_count; ++i) this->data[i].init(GROUP_MAX_NODE);
}

void Layer::add(const Node& element)
{
	// Normal
	for (int i = 0; i < this->data.get_size(); ++i) {
		if (this->data[i].get_size() < GROUP_MAX_NODE) {
			this->data[i].add(element);
			std::push_heap(this->data[i].iter_begin(), this->data[i].iter_end());
			return;
		}
	}

	// Case overflow
	this->data.add(vec<Node>());
	this->data[this->data.get_size() - 1].init(GROUP_MAX_NODE);
	this->data[this->data.get_size() - 1].add(element);
}

void Layer::pop(int group_index)
{
	std::pop_heap(this->data[group_index].iter_begin(), this->data[group_index].iter_end());
	this->data[group_index].pop();
}

void Layer::clear()
{
	for (int i = 0; i < this->data.get_size(); ++i) {
		this->data[i].clear();
	}
}

int Layer::max_group()
{
	int result = -1;
	for (int i = 0; i < this->data.get_size(); ++i) {
		if (this->data[i].get_size() > 0) {
			result = i;
			break;
		}
	}
	for (int i = result + 1; i < this->data.get_size(); ++i) {
		if (this->data[i].get_size() > 0 && this->data[result][0] < this->data[i][0]) {
			result = i;
			break;
		}
	}
	return result;
}

int Layer::get_size()
{
	int result = 0;
	for (int i = 0; i < this->data.get_size(); ++i) {
		result += this->data[i].get_size();
	}
	return result;
}

int Layer::get_capicity()
{
	return this->data.get_size() * GROUP_MAX_NODE;
}
