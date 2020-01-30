//
// Created by Alex on 10/29/2019.
//
#pragma once

#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

template <class K, class V>
class Tree;

template <class K, class V>
class Tree_comparator
{
public:
	bool operator() (Tree<K, V>* left, Tree<K, V>* right)
	{
		return left->key < right->key;
	}
};

template <class K, class V>
class Tree
{
	friend Tree_comparator<K, V>;
private:
	K key;
	V value;

	Tree* l_child = nullptr;
	Tree* r_child = nullptr;

public:
	Tree() = default;
	Tree(K key, V value);

	void add_left(Tree* tree);
	void add_right(Tree* tree);

	Tree<K, V>* get_left();
	Tree<K, V>* get_right();

	V get_value();

	static Tree<K, V>* connect(Tree* l_tree, Tree* r_tree);
};

template <class K, class V>
Tree<K, V>::Tree(K key, V value)
{
	this->key = key;
	this->value = value;
}

template <class K, class V>
Tree<K, V>* Tree<K, V>::connect(Tree* l_tree, Tree* r_tree)
{
	auto result = new Tree<K, V>();

	result->add_left(l_tree);
	result->add_right(r_tree);

	result->key = l_tree->key + r_tree->key;

	return result;
}

template <class K, class V>
void Tree<K, V>::add_left(Tree* tree)
{
	if (l_child != nullptr)
		throw "left child already exists";
	this->l_child = tree;
}

template <class K, class V>
void Tree<K, V>::add_right(Tree* tree)
{
	if (r_child != nullptr)
		throw "right child already exists";
	this->r_child = tree;
}

template<class K, class V>
inline Tree<K, V>* Tree<K, V>::get_left()
{
	return l_child;
}

template<class K, class V>
inline Tree<K, V>* Tree<K, V>::get_right()
{
	return r_child;
}

template<class K, class V>
inline V Tree<K, V>::get_value()
{
	return value;
}

#endif //HUFFMAN_TREE_H
