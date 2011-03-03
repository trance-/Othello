#ifndef GAMETREE
#define GAMETREE

#include <cassert>
#include <iostream>

// Core borrowed from Doug Harder
namespace Data_structures {
	template <typename Object>
	class GameTree {
		public:
			class iterator;

			GameTree( Object const &obj = Object() );
			~GameTree();
			bool empty() const;
			int size() const;
			int height() const;

			void clear();

			iterator begin();
			iterator end();

		private:
			class tree_node;
			tree_node *root_node;

		friend class iterator;
		friend class tree_node;
	};

	template <typename Object>
	class GameTree<Object>::tree_node {
		public:
			Object element;
			tree_node *parent;
			tree_node *previous_sibling;
			tree_node *next_sibling;
			tree_node *children_head;
			tree_node *children_tail;

			int node_degree;
			int node_depth;
			int node_height;
			int node_size;

			tree_node( Object const &, tree_node *, tree_node *, tree_node *, int );
			void clear();
	};

	template <typename Object>
	class GameTree<Object>::iterator {
		public:
			iterator();

			int degree() const;
			int depth() const;
			int height() const;
			int size() const;
			bool leaf() const;
			bool root() const;

			void insert( Object const &obj );

			iterator &operator++();
			iterator operator++( int );
			Object &operator*();
			bool operator==( iterator const &rhs ) const;
			bool operator!=( iterator const &rhs ) const;

			iterator parent();
			iterator begin();
			iterator end();

		private:
			tree_node *current_node;
			tree_node *parent_node;
			iterator( tree_node *, tree_node * );

		friend class GameTree;
	};

	template <typename Object>
	GameTree<Object>::GameTree( Object const &obj ):
	root_node( new tree_node( obj, 0, 0, 0, 0 ) ) {
		// Empty constructor
	}

	template <typename Object>
	GameTree<Object>::~GameTree() {
		root_node->clear();
	}

	template <typename Object>
	bool GameTree<Object>::empty() const {
		return false;
	}

	template <typename Object>
	int GameTree<Object>::size() const {
		return root_node->node_size;
	}

	template <typename Object>
	int GameTree<Object>::height() const {
		return root_node->node_height;
	}

	template <typename Object>
	void GameTree<Object>::clear() {
		for ( tree_node *child = root_node->children_head; child != 0; child = child->next_sibling ) {
			child->clear();
		}
	}

   template <typename Object>
	typename GameTree<Object>::iterator GameTree<Object>::begin() {
		return iterator( root_node, 0 );
	}

	template <typename Object>
	typename GameTree<Object>::iterator GameTree<Object>::end() {
		return iterator( 0, 0 );
	}

	template <typename Object>
	GameTree<Object>::tree_node::tree_node( Object const &obj, tree_node *par, tree_node *p, tree_node *n, int d ):
	element( obj ),
	parent( par ),
	previous_sibling( p ),
	next_sibling( n ),
	children_head( 0 ),
	children_tail( 0 ),
	node_degree( 0 ),
	node_depth( d ),
	node_height( 0 ),
	node_size( 1 ) {
		// Empty constructor
	}

	template <typename Object>
	void GameTree<Object>::tree_node::clear() {
		for ( tree_node *child = children_head; child != 0; ) {
			tree_node *next = child->next_sibling;
			child->clear();
			child = next;
		}

		delete this;
	}

	template <typename Object>
	GameTree<Object>::iterator::iterator():
	current_node( 0 ),
	parent_node( 0 ) {
		// Empty constructor
	}

	template <typename Object>
	GameTree<Object>::iterator::iterator( tree_node *c, tree_node *p ):
	current_node( c ),
	parent_node( p ) {
		// Empty constructor
	}

	template <typename Object>
	typename GameTree<Object>::iterator GameTree<Object>::iterator::parent() {
		if ( current_node->parent_node == 0 ) {
			assert( false );
		}

		return iterator( current_node->parent_node, current_node->parent_node->parent );
	}

	template <typename Object>
	int GameTree<Object>::iterator::degree() const {
		return current_node->node_degree;
	}

	template <typename Object>
	int GameTree<Object>::iterator::depth() const {
		return current_node->node_depth;
	}

    template <typename Object>
	int GameTree<Object>::iterator::height() const {
		return current_node->node_height;
	}

	template <typename Object>
	int GameTree<Object>::iterator::size() const {
		return current_node->node_size;
	}

	template <typename Object>
	bool GameTree<Object>::iterator::leaf() const {
		return current_node->children_head == 0;
	}

	template <typename Object>
	bool GameTree<Object>::iterator::root() const {
		return current_node->parent == 0;
	}

	template <typename Object>
	typename GameTree<Object>::iterator GameTree<Object>::iterator::begin() {
		return iterator( current_node->children_head, current_node );
	}

	template <typename Object>
	typename GameTree<Object>::iterator GameTree<Object>::iterator::end() {
		return iterator( 0, current_node );
	}

	template <typename Object>
	typename GameTree<Object>::iterator &GameTree<Object>::iterator::operator++() {
		if ( current_node != 0 ) {
			current_node = current_node->next_sibling;
		}

		return *this;
	}

	template <typename Object>
	typename GameTree<Object>::iterator GameTree<Object>::iterator::operator++( int ) {
		iterator copy = *this;

		if ( current_node != 0 ) {
			current_node = current_node->next_sibling;
		}

		return copy;
	}

    template <typename Object>
	Object &GameTree<Object>::iterator::operator*() {
		return current_node->element;
	}

	template <typename Object>
	bool GameTree<Object>::iterator::operator==( iterator const &rhs ) const {
		return ( current_node == rhs.current_node );
	}

	template <typename Object>
	bool GameTree<Object>::iterator::operator!=( iterator const &rhs ) const {
		return ( current_node != rhs.current_node );
	}

    template <typename Object>
	void GameTree<Object>::iterator::insert( Object const &obj ) {
		if ( current_node->node_degree == 0 ) {
			current_node->children_head = new tree_node(
				obj, current_node, 0, 0, current_node->node_depth + 1
			);

			current_node->children_tail = current_node->children_head;
			current_node->node_degree = 1;

			current_node->node_height = 1;

			for (
				tree_node *current = current_node, *regress = current_node->parent;
				regress != 0;
				current = regress, regress = regress->parent
			) {
				regress->node_height = std::max( current->node_height + 1, regress->node_height );
			}

			current_node->node_size = 2;
		} else {
			tree_node *tmp = new tree_node(
				obj, current_node, current_node->children_tail, 0, current_node->node_depth + 1
			);

			current_node->children_tail->next_sibling = tmp;
			current_node->children_tail = tmp;
			++( current_node->node_degree );

			++( current_node->node_size );
		}

		for ( tree_node *regress = current_node->parent; regress != 0; regress = regress->parent ) {
			++( regress->node_size );
		}
	}
}

#endif
