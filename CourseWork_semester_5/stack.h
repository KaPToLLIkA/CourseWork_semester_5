#pragma once

template <class T>
struct stack_node {
	stack_node<T>* prev = nullptr;
	T data;
};

template <class T>
struct stack_desc {
	stack_node<T>* current = nullptr;
};

template <class T>
class stack
{
private:
	stack_desc<T>* head = nullptr;
public:
	explicit stack();

	void push(T item);
	void pop();
	T* peek();

	bool is_empty();

	~stack();
};

template<class T>
inline stack<T>::stack()
{
	this->head = new stack_desc<T>();
}

template<class T>
inline void stack<T>::push(T item)
{
	stack_node<T>* new_node = new stack_node<T>();
	new_node->data = item;
	new_node->prev = head->current;
	head->current = new_node;
}

template<class T>
inline void stack<T>::pop()
{
	if (!is_empty()) {
		stack_node<T>* deleted_node = this->head->current;
		this->head->current = deleted_node->prev;
		delete deleted_node;
	}
}

template<class T>
inline T* stack<T>::peek()
{
	if (!is_empty()) {
		return &this->head->current->data;
	}
	else {
		return nullptr;
	}
}

template<class T>
inline bool stack<T>::is_empty()
{
	return head->current == nullptr;
}

template<class T>
inline stack<T>::~stack()
{
	while (!is_empty()) {
		pop();
	}
	delete head;
}
