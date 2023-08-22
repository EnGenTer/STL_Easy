#pragma once

/*
* 2023_05_20_csw_NOTE
* 2023_05_22_csw_TODO ѧϰ����ָ��&�Ĳ���
* 2023_05_22_csw_NOTE ����̳�ͬ����ͬ���͵ĳ�Ա�����������������Ϊ������ͬ�ĳ�Ա
*/ 

#include "simple_allocator.h"
#include "iterator.h"
#include "allocator.h"
#include <utility>

#define DEBUG

#ifndef RBTREEH
#define RBTREEH 

namespace MySTL {
	using std::pair; 

	typedef bool RBtree_node_color; 
	const RBtree_node_color _Color_Red = false; 
	const RBtree_node_color _Color_Black = true; 

	class RBtree_node_base {
	public:
		typedef RBtree_node_base* node_ptr_base; 

		RBtree_node_color NodeColor = _Color_Red; 
		node_ptr_base Left = nullptr; 
		node_ptr_base Right = nullptr; 
		node_ptr_base Parent = nullptr; 

		node_ptr_base minimum(void) {
			node_ptr_base curptr = this;
			while (curptr->Left != nullptr) curptr = curptr->Left;
			return curptr;
		}

		node_ptr_base maximum(void) {
			node_ptr_base curptr = this;
			while (curptr->Right != nullptr) curptr = curptr->Right;
			return curptr;
		}
	};

	template <typename Value>
	class RBtree_node : public RBtree_node_base {
	public:
		typedef Value				value_type; 
		typedef RBtree_node*		node_ptr; 

		value_type Value = value_type(); 
		/*
		static node_ptr& left() { return (node_ptr&)Left; }
		static node_ptr& right() { return (node_ptr&)Right; }
		static node_ptr& parent() { return (node_ptr&)Parent; }
		*/
#ifdef DEBUG
		// 2023_05_22_csw_NOTE ���ڹ�����������
		RBtree_node(value_type val, RBtree_node_color color = _Color_Red) : Value(val), Left(nullptr), Right(nullptr), NodeColor(color) {};
#endif // DEBUG
	};

	class RBtree_iterator_base {
	public:
		typedef bidirectional_iterator_tag	iterator_category;
		typedef ptrdiff_t			difference_type;
		typedef RBtree_node_base*	node_ptr_base;

		node_ptr_base NodePtr = nullptr; 

		// 2023_05_20_csw_NOTE ��̽ڵ�
		void incrembent() {
			// 2023_05_20_csw_NOTE �к��ӽڵ�
			if (NodePtr->Right != nullptr) {
				NodePtr = NodePtr->Right; 
				while (NodePtr->Left != nullptr) NodePtr = NodePtr->Left; 
			}
			// 2023_05_20_csw_NOTE �Ӹ��ڵ�����
			else {
				node_ptr_base parentPtr = NodePtr->Parent; 
				while (parentPtr != nullptr && parentPtr->Right == NodePtr) {
					NodePtr = parentPtr; 
					parentPtr = NodePtr->Parent; 
				}
				if (parentPtr != nullptr)
					NodePtr = parentPtr; 
			}
		}

		// 2023_05_20_csw_NOTE ǰ���ڵ�
		// 2023_05_20_csw_QUES node->parent->parent ��ʲô��header��ָ��ȫָ���Լ���
		void decrembent() {
			if (NodePtr->Left != nullptr) {
				NodePtr = NodePtr->Left; 
				while (NodePtr->Right != nullptr) NodePtr = NodePtr->Right; 
			}
			else {
				node_ptr_base parentPtr = NodePtr->Parent; 
				while (parentPtr != nullptr && parentPtr->Left == NodePtr) {
					NodePtr = parentPtr; 
					parentPtr = NodePtr->Parent; 
				}
				if (parentPtr != nullptr) {
					NodePtr = parentPtr; 
				}
			}
		}
	};

	template <typename Value>
	class RBtree_iterator: public RBtree_iterator_base {
	public:
		typedef RBtree_node<Value>* node_ptr; 
		typedef Value				value_type; 
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef RBtree_iterator<value_type>		self; 
		
		RBtree_iterator(void) {}
		RBtree_iterator(const self &it) {
			NodePtr = it.NodePtr; 
		}
		RBtree_iterator(node_ptr node) {
			NodePtr = node; 
		}

		value_type operator*() { return ((node_ptr)NodePtr)->Value; }
		reference operator->() { return &(*(NodePtr)); }
		self operator=(const self &inputit) { 
			self outputit; 
			outputit.NodePtr = inputit.NodePtr; 
			return outputit; 
		};
		bool operator==(const self &it) { return NodePtr == it.NodePtr; }
		bool operator!=(const self &it) { return NodePtr != it.NodePtr; }
		self operator++(int) { return RBtree_iterator_base::incrembent(); }
		self& operator++() {
			self copyit = *this;
			RBtree_iterator_base::incrembent();
			return copyit;
		}
		self operator--(int) { return RBtree_iterator_base::decrembent(); }
		self& operator--() {
			self copyit = *this;
			RBtree_iterator_base::decrembent();
			return copyit;
		}
	};

	template <typename Key>
	class KeyIsValue {
	public: 
		Key operator()(const Key &k) { return k; }
	};

	template <typename Tp>
	class less {
	public:
		bool operator()(const Tp& a, const Tp& b) { return a < b; }
	};

	template <typename Key, typename Value = Key, typename KeyOfValue = KeyIsValue<Key>, 
		typename Compare = less<Key>, typename Alloc = Simple_Allocator>
	class RBtree {
	public:
		typedef Key				key_type;
		typedef Value			value_type;
		typedef Value*			pointer;
		typedef Value&			reference;
		typedef RBtree_node<value_type>		tree_node; 
		typedef tree_node*		node_pointer;
		typedef Allocator<tree_node, Alloc>	tree_node_allocator; 
		typedef RBtree_iterator<value_type>	iterator; 

		node_pointer Header = nullptr; 
		size_t NodeNum = 0;

		KeyOfValue key_functor;
		Compare comp_functor;

		RBtree() : NodeNum(0) { Header = createNode(); }

		iterator begin() { return iterator((node_pointer)Header->minimum()); }
		iterator end() { return iterator(Header); }
		const size_t size()const { return NodeNum; }
		key_type key(const value_type &Val) { return key_functor(Val); }
		bool comp(const key_type& k1, const key_type& k2) { return comp_functor(k1, k2); }
		// 2023_05_21_csw_NOTE ������Ϊд��, typename Value = Key�����Ժ���һ���������س�ͻ��
		// bool comp(const value_type& val1, const value_type& val2) { return comp_functor(key(val1), key(val2)); }
		bool comp(const iterator& it1, const iterator& it2) { return comp_functor(key(*it1), key(*it2)); }

#ifndef DEBUG
		static node_pointer& left(node_pointer ptr) { return (node_pointer&)ptr->Left; }
		static node_pointer& right(node_pointer ptr) { return (node_pointer&)ptr->Right; }
		static node_pointer& parent(node_pointer ptr) { return (node_pointer&)ptr->Parent; }
		static reference value(node_pointer ptr) { return ptr->Value; }
		static RBtree_node_color& color(node_pointer ptr) { return ptr->NodeColor; }

		static node_pointer& left(RBtree_node_base* ptr) { return (node_pointer&)ptr->Left; }
		static node_pointer& right(RBtree_node_base* ptr) { return (node_pointer&)ptr->Right; }
		static node_pointer& parent(RBtree_node_base* ptr) { return (node_pointer&)ptr->Parent; }
		// static reference value(RBtree_node_base* ptr) { return ptr->Value; }
		// static const key_type key(const node_pointer const ptr)const { return key(ptr->Value); }
		static RBtree_node_color& color(RBtree_node_base* ptr) { return ptr->NodeColor; }
#else
		node_pointer& left(node_pointer ptr) { return (node_pointer&)ptr->Left; }
		node_pointer& right(node_pointer ptr) { return (node_pointer&)ptr->Right; }
		node_pointer& parent(node_pointer ptr) { return (node_pointer&)ptr->Parent; }
		reference value(node_pointer ptr) { return ptr->Value; }
		RBtree_node_color& color(node_pointer ptr) { return ptr->NodeColor; }

		node_pointer& left(RBtree_node_base* ptr) { return (node_pointer&)ptr->Left; }
		node_pointer& right(RBtree_node_base* ptr) { return (node_pointer&)ptr->Right; }
		node_pointer& parent(RBtree_node_base* ptr) { return (node_pointer&)ptr->Parent; }
		// static reference value(RBtree_node_base* ptr) { return ptr->Value; }
		// static const key_type key(const node_pointer const ptr)const { return key(ptr->Value); }
		RBtree_node_color& color(RBtree_node_base* ptr) { return ptr->NodeColor; }
#endif // !DEBUG

		node_pointer createNode(value_type Value = value_type()) { 
			node_pointer newnodeptr = tree_node_allocator::allocate(); 
			left(newnodeptr) = nullptr;
			right(newnodeptr) = nullptr; 
			parent(newnodeptr) = nullptr; 
			value(newnodeptr) = Value;
			color(newnodeptr) = _Color_Red; 
			return newnodeptr; 
		}

		// 2023_05_21_csw_TODO��ɫ�仯�е����
		void rightRotate(node_pointer CurNode) {
			node_pointer ParentNode = parent(CurNode);
			node_pointer LeftNode = left(CurNode); 
			RBtree_node_color tempcolor = color(CurNode);

			left(CurNode) = right(LeftNode); 
			if (left(CurNode) != nullptr) parent(left(CurNode)) = CurNode;
			right(LeftNode) = CurNode; 
			parent(CurNode) = LeftNode; 
			parent(LeftNode) = ParentNode; 
			if (left(ParentNode) == CurNode) left(ParentNode) = LeftNode;
			else right(ParentNode) = LeftNode; 

			color(CurNode) = color(LeftNode); 
			color(LeftNode) = tempcolor; 
		}

		// 2023_05_21_csw_TODO��ɫ�仯�е����
		void leftRotate(node_pointer CurNode) {
			node_pointer ParentNode = parent(CurNode);
			node_pointer RightNode = right(CurNode);
			RBtree_node_color tempcolor = color(CurNode);

			right(CurNode) = left(RightNode);
			if (right(CurNode) != nullptr) parent(right(CurNode))= CurNode;
			left(RightNode) = CurNode;
			parent(CurNode) = RightNode;
			parent(RightNode) = ParentNode;
			if (right(ParentNode) == CurNode) right(ParentNode) = RightNode;
			else left(ParentNode) = RightNode;
		}

		iterator find (const value_type &Val) {
			node_pointer curnode = left(Header); 
			while (curnode != nullptr) {
				// 2023_05_21_csw_NOTE ��Ҫ������if �ж��Ƿ����
				if (comp(key(Val), key(value(curnode))) == true) 
					curnode = left(curnode); 
				else {
					if (comp(key(value(curnode)), key(Val)) == true) {
						curnode = right(curnode); 
					}
					else {
						return iterator(curnode); 
					}
				}
			}
			return end(); 
		}

		template <typename ptr>
		inline void _switchPtr(ptr &p1, ptr &p2) {
			ptr tempp = p1; 
			p1 = p2; 
			p2 = tempp; 
		}

		void _insert(node_pointer ParentNode, node_pointer CurNode) {
			// 2023_05_22_csw_NOTE ��һ���ڵ㣬����Ҫ����
			if (ParentNode == Header) {
				left(Header) = CurNode, right(Header) = CurNode;
				return;
			}

			//2023_05_21_csw_NOTE ���ڵ�Ϊ��ɫ������Ҫ����
			if (color(ParentNode) == _Color_Black) return; 

			node_pointer ancesternode = parent(ParentNode); 

			// 2023_05_21_csw_NOTE �游����һ���ӽڵ㣬ֻ��Ҫ��ת����
			if (left(ancesternode) == nullptr) {
				// 2023_05_22_csw_NOTE ��ת
				if (left(ParentNode) == CurNode) {
					rightRotate(ParentNode);
					_switchPtr(ParentNode, CurNode);
				}
				leftRotate(ancesternode);
				_switchPtr(ancesternode, ParentNode);
				// 2023_05_22_csw_NOTE Ⱦɫ
				color(ancesternode) = _Color_Black;
				color(left(ancesternode)) = _Color_Red;
				color(right(ancesternode)) = _Color_Red;
			}
			else if (right(ancesternode) == nullptr) {
				// 2023_05_22_csw_NOTE ��ת
				if (right(ParentNode) == CurNode) {
					leftRotate(ParentNode);
					_switchPtr(ParentNode, CurNode);
				}
				rightRotate(ancesternode);
				_switchPtr(ancesternode, ParentNode);
				// 2023_05_22_csw_NOTE Ⱦɫ
				color(ancesternode) = _Color_Black;
				color(left(ancesternode)) = _Color_Red;
				color(right(ancesternode)) = _Color_Red;
			}
			// 2023_05_21_csw_NOTE �游��˫�ӽڵ㣬�������
			else {
				color(left(ancesternode)) = _Color_Black; 
				color(right(ancesternode)) = _Color_Black; 
				color(ancesternode) = _Color_Red; 
				_insert(parent(ancesternode), ancesternode);
			}
			if (parent(ancesternode) == Header) left(Header) = ancesternode, right(Header) = ancesternode; 
		}

		// 2023_05_21_csw_NOTE �����һ�β��붥������
		bool headerInsert(value_type Val) {
			if (left(Header) == nullptr && right(Header) == nullptr) {
				node_pointer newnodeptr = createNode(Val);
				parent(newnodeptr) = Header; 
				color(newnodeptr) = _Color_Black; 
				left(Header) = newnodeptr; 
				right(Header) = newnodeptr; 
				return true; 
			}
			return false; 
		}

		pair<iterator, bool> uniqueInsert(value_type Val) {
			if (headerInsert(Val) == true) {
				++NodeNum; 
				return pair<iterator, bool>(iterator(left(Header)), true);
			}
			node_pointer parentnode = Header;
			node_pointer curnode = left(Header);
			while (curnode != nullptr) {
				// 2023_05_21_csw_NOTE ��Ҫ������if �ж��Ƿ����
				if (comp(key(Val), key(value(curnode))) == true) {
					parentnode = curnode;
					curnode = left(curnode);
				}
				else {
					if (comp(key(value(curnode)), key(Val)) == true) {
						parentnode = curnode;
						curnode = right(curnode);
					}
					else {
						return pair<iterator, bool>(end(), false); // 2023_05_21_csw_NOTE ����ʧ��
					}
				}
			}
			node_pointer newnode = createNode(Val);
			if (comp(key(Val), key(value(parentnode))) == true)
				left(parentnode) = newnode;
			else
				right(parentnode) = newnode; 
			parent(newnode) = parentnode;
			_insert(parentnode, newnode);
			color(left(Header)) = _Color_Black; // 2023_05_22_csw_NOTE �����ʱ����ܻ���ĸ��ڵ���ɫ
			++NodeNum; 
			return pair<iterator, bool>(iterator(newnode), true);
		}

		iterator equalInsert(value_type Val) {
			if (headerInsert(Val) == true) {
				++NodeNum; 
				return iterator(left(Header));
			}
			node_pointer parentnode = Header;
			node_pointer curnode = left(Header);
			while (curnode != nullptr) {
				// 2023_05_21_csw_NOTE ��Ҫ������if �ж��Ƿ����
				if (comp(key(Val), key(value(curnode))) == true) {
					parentnode = curnode;
					curnode = left(curnode);
				}
				else {
					parentnode = curnode;
					curnode = right(curnode);
				}
			}
			node_pointer newnode = createNode(Val);
			if (comp(key(Val), key(value(parentnode))) == true)
				left(parentnode) = newnode;
			else
				right(parentnode) = newnode;
			parent(newnode) = parentnode;
			_insert(parentnode, newnode);
			color(left(Header)) = _Color_Black;
			++NodeNum; 
			return iterator(newnode);
		}

		// 2023_05_21_csw_NOTE ���漰��ǰ�������ӽڵ㽻���Ĳ��裬�������൱��erase���Ӳ���
		void _erase(node_pointer ParentNodePtr, node_pointer CurNodePtr) {
			if (ParentNodePtr == Header) {
				return; 
			}

			bool nosonofbroderflag = false; 
			// 2023_05_22_csw_NOTE ��ɫ�ڵ㲻�ù�
			if (color(CurNodePtr) == _Color_Red) {
				return;
			}
			// 2023_05_22_csw_NOTE һ�����ֵܽڵ㣬���򲻷�������·����Ҷ�ӽڵ��ɫ�ڵ�����ȵ�����
			else {
				node_pointer brodernodeptr, childnodeptr;
				RBtree_node_color tempcolor = color(ParentNodePtr); 
				if (left(ParentNodePtr) == nullptr) {					// R
					brodernodeptr = right(ParentNodePtr);
					if (right(brodernodeptr) != nullptr) {				// RR
						childnodeptr = right(brodernodeptr);
						leftRotate(ParentNodePtr);
						_switchPtr(ParentNodePtr, brodernodeptr);
					}
					else if (left(brodernodeptr) != nullptr) {			// RL
						childnodeptr = left(brodernodeptr);
						rightRotate(brodernodeptr);
						_switchPtr(brodernodeptr, childnodeptr);
						leftRotate(ParentNodePtr);
						_switchPtr(ParentNodePtr, brodernodeptr);
					}
					else nosonofbroderflag = true; 
				}
				else {													// L
					brodernodeptr = left(ParentNodePtr);
					if (left(brodernodeptr) != nullptr) {				// LL
						childnodeptr = left(brodernodeptr);
						rightRotate(ParentNodePtr);
						_switchPtr(ParentNodePtr, brodernodeptr);
					}
					else if (right(brodernodeptr) != nullptr) {			// LR
						childnodeptr = right(brodernodeptr);
						leftRotate(brodernodeptr);
						_switchPtr(brodernodeptr, childnodeptr);
						rightRotate(ParentNodePtr);
						_switchPtr(ParentNodePtr, brodernodeptr);
					}
					else nosonofbroderflag = true;
				}

				if (nosonofbroderflag == false) { // 2023_05_22_csw_NOTE ����Ⱦɫ��
					// 2023_05_22_csw_TODO ����Ĵ����е�ֱ���Ҳ��֪���ǲ��ǶԵ�
					color(ParentNodePtr) = tempcolor;
					color(left(ParentNodePtr)) = _Color_Black;
					color(right(ParentNodePtr)) = _Color_Black;
					if (left(left(ParentNodePtr)) != nullptr) color(left(left(ParentNodePtr))) = _Color_Red;
					if (left(right(ParentNodePtr)) != nullptr) color(left(right(ParentNodePtr))) = _Color_Red;
					if (right(left(ParentNodePtr)) != nullptr) color(right(left(ParentNodePtr))) = _Color_Red;
					if (right(right(ParentNodePtr)) != nullptr) color(right(right(ParentNodePtr))) = _Color_Red;
				}
				else {
					// 2023_05_22_csw_NOTE ���ڵ�Ϊ��ɫ����ʾ���ڵ���ֻ��һ����ɫ�ڵ㣬Ⱦɫ�������ڵ�Ϊ��ɫ
					if (color(ParentNodePtr) == _Color_Red) {
						color(ParentNodePtr) = _Color_Black; 
						color(brodernodeptr) = _Color_Red; 
					}
					// 2023_05_22_csw_NOTE ���ڵ�Ϊ��ɫ�����磬������������
					else {
						// 2023_05_22_csw_QUES ���ﲻ̫�������ϵ�����ֻ�����ɫ����������������ת+��ɫ����
						color(brodernodeptr) = _Color_Red; 
						color(ParentNodePtr) = _Color_Black; 
						_erase(parent(ParentNodePtr), ParentNodePtr); 
					}
				}
			}
		}

		// 2023_05_21_csw_NOTE �漰��ǰ�������ӽڵ㽻���Ĳ���
		bool erase(iterator CurIt) {
			if (CurIt == end()) return false; 

			// 2023_05_22_csw_NOTE Ѱ��ǰ�����ߺ�̲��滻
			node_pointer curnodeptr = (node_pointer)CurIt.NodePtr; 
			node_pointer removenodeptr; 
			if (left(curnodeptr) != nullptr) {
				removenodeptr = (node_pointer)(left(curnodeptr)->maximum()); 
				value(curnodeptr) = value(removenodeptr); 
			}
			else if (right(curnodeptr) != nullptr) {
				removenodeptr = (node_pointer)(right(curnodeptr)->minimum());
				value(curnodeptr) = value(removenodeptr); 
			}
			else {
				removenodeptr = curnodeptr; 
			}
			node_pointer parentnodeptr = parent(removenodeptr); 
			if (left(parentnodeptr) == removenodeptr) left(parentnodeptr) = nullptr;
			else right(parentnodeptr) = nullptr; 
			_erase(parentnodeptr, removenodeptr);
			tree_node_allocator::deallocate(removenodeptr);
			--NodeNum;
			return true;
		}

		// 2023_05_21_csw_NOTE �漰��ǰ�������ӽڵ㽻���Ĳ���
		bool erase(value_type val) {
			return erase(find(val));
		}

#ifdef DEBUG
		void debug_BBB() {
			node_pointer node = createNode(4);
			node_pointer root = node;
			color(node) = _Color_Black;
			left(Header) = node;
			right(Header) = node;
			parent(node) = Header;

			node = createNode(2);
			color(node) = _Color_Black;
			left(root) = node;
			parent(node) = root;

			node = createNode(1);
			color(node) = _Color_Black;
			left(left(root)) = node;
			parent(node) = left(root);

			node = createNode(3);
			color(node) = _Color_Black;
			right(left(root)) = node;
			parent(node) = left(root);

			node = createNode(6);
			color(node) = _Color_Black;
			right(root) = node;
			parent(node) = root;

			node = createNode(5);
			color(node) = _Color_Black;
			left(right(root)) = node;
			parent(node) = right(root);

			node = createNode(7);
			color(node) = _Color_Black;
			right(right(root)) = node;
			parent(node) = right(root);
		}

		void debug_BRB() {
			node_pointer node = createNode(4);
			node_pointer root = node;
			color(node) = _Color_Black;
			left(Header) = node;
			right(Header) = node;
			parent(node) = Header;

			node = createNode(2);
			color(node) = _Color_Black;
			left(root) = node;
			parent(node) = root;

			node = createNode(6);
			color(node) = _Color_Red;
			right(root) = node;
			parent(node) = root;

			node = createNode(5);
			color(node) = _Color_Black;
			left(right(root)) = node;
			parent(node) = right(root);

			node = createNode(7);
			color(node) = _Color_Black;
			right(right(root)) = node;
			parent(node) = right(root);
		}
#endif // DEBUG
	};
}

#endif // !RBTREEH
