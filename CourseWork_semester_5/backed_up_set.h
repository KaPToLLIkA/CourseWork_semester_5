#pragma once
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG


#include <stdint.h>
#include <vector>
#include "stack.h"
#include "state.h"



template<class T>
struct set_node {
	T data;
	uint8_t height = 1;
	set_node<T>* left = nullptr, * right = nullptr;
	stack<state> history;
	
	set_node(T data, uint32_t generation) {
		this->data = data;
		this->history.push(state(generation, states::ADD));
	}
};

template<class T>
class backed_up_set
{
private:
	uint32_t generation = 0;
	set_node<T> *root = nullptr;


	static uint8_t get_height(set_node<T>* node);
	static int8_t get_balance_factor(set_node<T>* node);
	static void correct_height(set_node<T>* node);
	static set_node<T>* right_rotate(set_node<T>* node);
	static set_node<T>* left_rotate(set_node<T>* node);

	static set_node<T>* balance(set_node<T>* node);
	static set_node<T>* find_min_key(set_node<T>* node);
	static set_node<T>* remove_min_key(set_node<T>* node);
	set_node<T>* rec_insert(set_node<T>* node, T& value);
	set_node<T>* rec_remove(set_node<T>* node, T& value);
	set_node<T>* rec_recovery(set_node<T>* node, uint32_t version, bool remove_deleted);
	set_node<T>* rec_remove_unused(set_node<T>* node);
	set_node<T>* rec_clean(set_node<T>* node);
	static set_node<T>* complete_removal(set_node<T>* node);

#ifdef _DEBUG
	static void rec_print(
		std::ostream& out,
		set_node<T>* node,
		std::vector<bool>& levels,
		int32_t depth,
		uint32_t state,
		uint32_t l,
		char spacer = '-',
		char down = '|',
		char cross = '+');
#endif // _DEBUG

public:
	void save_backup();
	void go_to_backup(uint32_t version, bool remove_deleted = false);
	void remove(T key);
	void remove_unused_items();
	void insert(T key);
	void clean();

	uint32_t get_current_version();

	~backed_up_set();
#ifdef _DEBUG
	
	void print(std::ostream& out, uint32_t state);
#endif // _DEBUG

};

template<class T>
inline uint8_t backed_up_set<T>::get_height(set_node<T>* node)
{
	return node ? node->height : 0;
}

template<class T>
inline int8_t backed_up_set<T>::get_balance_factor(set_node<T>* node)
{
	return get_height(node->right) - get_height(node->left);
}

template<class T>
inline void backed_up_set<T>::correct_height(set_node<T>* node)
{
	uint8_t h_right = get_height(node->right),
		h_left = get_height(node->left);
	node->height = (h_left > h_right ? h_left : h_right) + 1;
}

template<class T>
inline set_node<T>* backed_up_set<T>::right_rotate(set_node<T>* node)
{
	set_node<T>* new_node = node->left;
	node->left = new_node->right;
	new_node->right = node;

	correct_height(node);
	correct_height(new_node);

	return new_node;
}

template<class T>
inline set_node<T>* backed_up_set<T>::left_rotate(set_node<T>* node)
{
	set_node<T>* new_node = node->right;
	node->right = new_node->left;
	new_node->left = node;

	correct_height(node);
	correct_height(new_node);

	return new_node;
}

template<class T>
inline set_node<T>* backed_up_set<T>::balance(set_node<T>* node)
{
	correct_height(node);
	if (get_balance_factor(node) == 2)
	{
		if (get_balance_factor(node->right) < 0)
			node->right = right_rotate(node->right);
		return left_rotate(node);
	}
	if (get_balance_factor(node) == -2)
	{
		if (get_balance_factor(node->left) > 0)
			node->left = left_rotate(node->left);
		return right_rotate(node);
	}
	return node;
}

template<class T>
inline set_node<T>* backed_up_set<T>::find_min_key(set_node<T>* node)
{
	return node->left ? find_min_key(node->left) : node;
}

template<class T>
inline set_node<T>* backed_up_set<T>::remove_min_key(set_node<T>* node)
{
	if (!node->left) {
		return node->right;
	}
	node->left = remove_min_key(node->left);
	return balance(node);
}

template<class T>
inline set_node<T>* backed_up_set<T>::rec_insert(set_node<T>* node, T& value)
{
	if (node == nullptr) {
		return new set_node<T>(value, this->generation);
	}
	if (value < node->data) {
		node->left = rec_insert(node->left, value);
	}
	else if (value > node->data) {
		node->right = rec_insert(node->right, value);
	}
	else {
		if (node->history.peek()->check_flag(states::UNUSED)) {

			node->history.pop();
			node->history.push(state(this->generation, states::ADD));

		}
		else if (node->history.peek()->check_flag(states::REMOVE)
			&& node->history.peek()->get_generation() == this->generation) {
			node->history.pop();
		}
		else if (node->history.peek()->check_flag(states::REMOVE)) {
			node->history.push(state(this->generation, states::ADD));
		}
	}
	return balance(node);
}

template<class T>
inline set_node<T>* backed_up_set<T>::rec_remove(set_node<T>* node, T& value)
{
	if (!node) {
		return nullptr;
	}
	else if (value < node->data) {
		node->left = rec_remove(node->left, value);
	}
	else if (value > node->data) {
		node->right = rec_remove(node->right, value);
	}
	else { // value == node->data
		state* current_state = node->history.peek();
		if ((current_state->check_flag(states::ADD) 
			&& current_state->get_generation() == this->generation)
			|| current_state->check_flag(states::UNUSED)) {
			return complete_removal(node);
		}
		else {
			if (!current_state->check_flag(states::REMOVE)) {
				node->history.push(state(generation, states::REMOVE));
			}
			return node;
		}
	}
	return balance(node);
}

template<class T>
inline set_node<T>* backed_up_set<T>::rec_recovery(set_node<T>* node, uint32_t version, bool remove_deleted)
{
	if (node) {
		node->left = rec_recovery(node->left, version, remove_deleted);
		node->right = rec_recovery(node->right, version, remove_deleted);

		while (node->history.peek()->get_generation() > version) {
			node->history.pop();
			if (node->history.is_empty()) {
				break;
			}
		}

		if (node->history.is_empty()) {
			if (remove_deleted) {
				return complete_removal(node);
			}
			else {
				node->history.push(state(version, states::UNUSED));
			}
		}
		
		if (remove_deleted) {
			return balance(node);
		}
	}
	return node;
}

template<class T>
inline set_node<T>* backed_up_set<T>::rec_remove_unused(set_node<T>* node)
{
	if (node) {
		node->left = rec_remove_unused(node->left);
		node->right = rec_remove_unused(node->right);

		if (node->history.peek()->check_flag(states::UNUSED)) {
			return complete_removal(node);
		}
		return balance(node);
	}
	return nullptr;
}

template<class T>
inline set_node<T>* backed_up_set<T>::rec_clean(set_node<T>* node)
{
	if (node->left != nullptr) {
		node->left = rec_clean(node->left);
	}
	if (node->right != nullptr) {
		node->right = rec_clean(node->right);
	}
	delete node;
	return nullptr;
}

template<class T>
inline set_node<T>* backed_up_set<T>::complete_removal(set_node<T>* node)
{
	set_node<T>* left_child = node->left;
	set_node<T>* right_child = node->right;
	delete node;

	if (!right_child) {
		return left_child;
	}

	set_node<T>* min_node = find_min_key(right_child);
	min_node->right = remove_min_key(right_child);
	min_node->left = left_child;
	return balance(min_node);
}

#ifdef _DEBUG
template<class T>
inline void backed_up_set<T>::rec_print(
	std::ostream& out,
	set_node<T>* node, 
	std::vector<bool> &levels, 
	int32_t depth, 
	uint32_t state,
	uint32_t l,
	char spacer,
	char down,
	char cross
)
{
	
	for (int32_t i = 0; i < depth; ++i) {
		if (i != depth - 1) {
			if (levels[i]) {
				out << down;
			}
			else {
				out << " ";
			}
			for (uint32_t x = 0; x < l; ++x) {
				out << " ";
			}
		}
		else {
			out << cross;
		}
	}
		
	if (depth) {
		for (uint32_t i = 0; i < l; ++i) {
			out << spacer;
		}
	}
	
	if (node) {
		if (node->history.peek()->check_flag(state)) {
			out << node->data << std::endl;
		}
		else {
			out << std::endl;
		}

		levels.push_back(true);
		rec_print(out, node->left, levels, depth + 1, state, l, spacer, down, cross);
		levels[levels.size() - 1] = false;
		rec_print(out, node->right, levels, depth + 1, state, l, spacer, down, cross);
		levels.erase(--levels.end());
	}
	else {
		out << std::endl;
	}
}

#endif // _DEBUG


template<class T>
inline void backed_up_set<T>::save_backup()
{
	++generation;
}

template<class T>
inline void backed_up_set<T>::go_to_backup(uint32_t version, bool remove_deleted)
{
	if (version >= this->generation) {
		return;
	}

	this->generation = version;

	root = rec_recovery(root, version, remove_deleted);
}

template<class T>
inline void backed_up_set<T>::remove(T key)
{
	root = rec_remove(root, key);
}

template<class T>
inline void backed_up_set<T>::remove_unused_items()
{
	root = rec_remove_unused(root);
}

template<class T>
inline void backed_up_set<T>::insert(T key)
{
	root = rec_insert(root, key);
}

template<class T>
inline void backed_up_set<T>::clean()
{
	if (root != nullptr) {
		root = rec_clean(root);
	}
	generation = 0;
}

template<class T>
inline uint32_t backed_up_set<T>::get_current_version()
{
	return this->generation;
}

template<class T>
inline backed_up_set<T>::~backed_up_set()
{
	clean();
}

#ifdef _DEBUG
template<class T>
inline void backed_up_set<T>::print(std::ostream& out, uint32_t state)
{
	std::vector<bool> levels;
	rec_print(out, root, levels, 0, state, 3);
}
#endif // _DEBUG
