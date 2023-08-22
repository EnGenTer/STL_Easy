#pragma once
/*
* 2023_05_18_csw_
* 2023_05_18_csw_NOTE Value是（Key, Data）
* 2023_05_18_csw_NOTE KeyOfValue用于从Value中取出Key
*/

#include "allocator.h"

#ifndef RBTREEH
#define RBTREEH

namespace MySTL {
	typedef bool RBtree_node_color;
	const RBtree_node_color Color_Red = false;
	const RBtree_node_color Color_Black = true;

	template <typename Tp>
	class less {
		bool operator()(const Tp &a, const Tp &b) const{
			return a < b; 
		}
	};

	template <typename ValueType, typename KeyType = ValueType> 
	class setKey {
	public:
		KeyType operator()(const ValueType &Val) {
			return Val; 
		}
	};

	// 2023_05_18_csw_NOTE Key被包含在Value中，需要使用
	template <typename Tp>
	class RBTreeNode {
	public:
		typedef Tp					value_type; 
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef RBTreeNode<Tp>		TreeNode;
		typedef TreeNode*			node_pointer;

		RBtree_node_color NodeColor = Color_Red;
		node_pointer Left = nullptr;
		node_pointer Right = nullptr;
		node_pointer Parent = nullptr;
		value_type Value;

		RBTreeNode(void) : Left(nullptr), Right(nullptr), Parent(nullptr), NodeColor(Color_Red) {}; // 2023_05_18_csw_NOTE 不能再变量初始化中初始化其他类
		RBTreeNode(pointer parent) : Left(nullptr), Right(nullptr), Parent(parent), NodeColor(Color_Red) {};
		RBTreeNode(pointer parent, value_type val) : Left(nullptr), Right(nullptr), Parent(parent), NodeColor(Color_Red), Value(val) {};
		RBTreeNode(value_type val) : Left(nullptr), Right(nullptr), Parent(nullptr), NodeColor(Color_Red), Value(val) {};

		node_pointer rightRotate() {
			node_pointer leftchild = this->Left; 
			this->Left = leftchild->Right; 
			leftchild->Right = this; 
			this->Parent = leftchild; 
			return leftchild;
		}

		node_pointer leftRotate() {
			node_pointer rightchild = this->Right; 
			this->Right = rightchild->Left; 
			rightchild->Left = this; 
			this->Parent = rightchild; 
			return rightchild; 
		}

		// 2023_05_18_csw_NOTE 当前节点为祖宗，让childnode和它下两层的左节点旋转
		node_pointer RRRotate() {
			node_pointer ancesterptr = this->Parent;
			this->leftRotate()->Parent = ancesterptr;
			return this->Parent;
		}

		node_pointer RLRotate() {
			node_pointer ancesterptr = this->Parent;
			this->Right = this->Right->rightRotate();
			this->Right->Parent = this;
			this->leftRotate()->Parent = ancesterptr;
			return this->Parent;
		}

		node_pointer LRRotate() {
			node_pointer ancesterptr = this->Parent;
			this->Left = this->Left->leftRotate(); 
			this->Left->Parent = this;
			this->rightRotate()->Parent = ancesterptr; 
			return this->Parent; 
		}

		node_pointer LLRotate() {
			node_pointer ancesterptr = this->Parent;
			this->rightRotate()->Parent = ancesterptr;
			return this->Parent;
		}

		// 2023_05_19_csw_NOTE 偷懒做法，把一个顶点涂黑，子节点涂红
		void colorBlack() {
			NodeColor = Color_Black;
			if (Left != nullptr) Left->NodeColor = Color_Red;
			if (Right != nullptr) Right->NodeColor = Color_Red;
		}

		// 2023_05_19_csw_NOTE 偷懒做法，把一个顶点涂黑，子节点涂红
		void colorRed() {
			NodeColor = Color_Red;
			if (Left != nullptr) Left->NodeColor = Color_Black;
			if (Right != nullptr) Right->NodeColor = Color_Black;
		}
	};

	// 2023_05_18_csw_NOTE 不确定能不能设置默认key提取方法为集合方法
	template <
		typename Key, 
		typename Value = Key, 
		typename KeyOfValue = setKey<Key, Value>, 
		typename Compare = less<Key>, 
		typename Alloc = Simple_Allocator
	>
	class RBTree {
	public:
		typedef Key					key_type;
		typedef Value				value_type;
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef RBTreeNode<key_type>	TreeNode; 
		typedef TreeNode*			node_pointer;
		typedef Allocator<TreeNode, Simple_Allocator> Node_Allocator; 
		
		size_t NodeNum = 0; 
		node_pointer header; // 2023_05_18_csw_NOTE dummyhead
		Compare compare_func; 
		KeyOfValue key_func;

		RBTree(void) : NodeNum(0) {
			header = getNewNode(); 
		};

		key_type key(value_type Val) {
			return key_func(Val); 
		}

		node_pointer getNewNode(value_type Val = value_type(), node_pointer Parent = nullptr, RBTreeNodeColor Color = Color_Red, node_pointer Left = nullptr, node_pointer Right = nullptr) {
			node_pointer nodeptr = Node_Allocator::allocate();
			nodeptr->Parent = Parent;
			nodeptr->Left = Left;
			nodeptr->Right = Right;
			nodeptr->Value = Val;
			// 2023_05_18_csw_TODO Color
			nodeptr->NodeColor = Color; 
			return nodeptr; 
		}

		// 2023_05_18_csw_NOTE 父节点为黑色 =》 插入后不调整
		// 2023_05_18_csw_NOTE 祖父节点为红色且有叔父节点（23树） =》 变色+旋转
		// 2023_05_18_csw_NOTE 祖父节点为红色且插入后需要上溢（4树） =》 递归变色
		bool insert(value_type Val) {
			// 2023_05_18_csw_NOTE 找到对应位置
			if (header->Left == nullptr && header->Right == nullptr) {
				node_pointer NewNodePtr = getNewNode(Val, header, Color_Black);
				header->Left = NewNodePtr;
				header->Right = NewNodePtr;
				return true; 
			}

			node_pointer CurNodePtr = header->Left;
			node_pointer NewNodePtr = getNewNode(Val);
			while (1) {
				if (key(Val) < key(CurNodePtr->Value)) {
					if (CurNodePtr->Left == nullptr) {
						CurNodePtr->Left = NewNodePtr;
						NewNodePtr->Parent = CurNodePtr;
						break; 
					}
					else {
						CurNodePtr = CurNodePtr->Left;
					}
				}
				else if (key(Val) == key(CurNodePtr->Value)) {
					return false; // 2023_05_18_csw_NOTE 重复元素
				}
				else {
					if (CurNodePtr->Right == nullptr) {
						CurNodePtr->Right = NewNodePtr;
						NewNodePtr->Parent = CurNodePtr;
						break;
					}
					else {
						CurNodePtr = CurNodePtr->Right;
					}
				}
			}
			insertNode(CurNodePtr, NewNodePtr);
			return true; // 2023_05_19_csw_NOTE 有插入内容
		}

		void insertNode(node_pointer CurNodePtr, node_pointer NewNodePtr) {
			// 2023_05_18_csw_NOTE 父节点为黑色 =》 插入后不调整
			if (CurNodePtr == header) {
				CurNodePtr->Left = NewNodePtr; 
				CurNodePtr->Right = NewNodePtr; 
				NewNodePtr->NodeColor = Color_Black; 
			}
			else if (CurNodePtr->NodeColor == Color_Black) {
				return;
			}
			// 2023_05_18_csw_NOTE 祖父节点为红色且插入后需要上溢（4树） =》 递归变色
			else if (CurNodePtr->Parent->Left != nullptr && CurNodePtr->Parent->Right != nullptr) {
				colorAdjust(CurNodePtr->Parent, CurNodePtr, NewNodePtr);
			}

			// 2023_05_18_csw_NOTE 祖父节点为红色且无叔父节点（23树） =》 变色+旋转
			else {
				rotateAdjust(CurNodePtr->Parent, CurNodePtr, NewNodePtr);
			}
		}

		void colorAdjust(node_pointer AncesterNodePtr, node_pointer CurNodePtr, node_pointer NewNodePtr) {
			// 2023_05_19_csw_TODO 懒得考虑是否是孩子的情况了
			// 2023_05_19_csw_TODO 这里处理的很粗暴，不知道是不是对的
			AncesterNodePtr->NodeColor = Color_Red; 
			AncesterNodePtr->Left->NodeColor = Color_Black; 
			AncesterNodePtr->Right->NodeColor = Color_Black; 
			NewNodePtr->NodeColor = Color_Red; 

			node_pointer PreAncesterNodePtr = AncesterNodePtr->Parent; 
			insertNode(PreAncesterNodePtr, AncesterNodePtr); 
		}

		void rotateAdjust(node_pointer AncesterNodePtr, node_pointer CurNodePtr, node_pointer NewNodePtr) {
			// 2023_05_19_csw_TODO 懒得考虑是否是孩子的情况了
			node_pointer PreAncesterNodePtr = AncesterNodePtr->Parent; 
			if (isLeft(AncesterNodePtr, CurNodePtr) == true) {
				if (isLeft(CurNodePtr, NewNodePtr) == true) {		// 2023_05_19_csw_NOTE LL
					if (PreAncesterNodePtr == header) {
						PreAncesterNodePtr->Left = AncesterNodePtr->LLRotate(); 
						PreAncesterNodePtr->Right = PreAncesterNodePtr->Left; 
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else if (isLeft(PreAncesterNodePtr, AncesterNodePtr)) {
						PreAncesterNodePtr->Left = AncesterNodePtr->LLRotate();
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else {
						PreAncesterNodePtr->Right = AncesterNodePtr->LLRotate();
						PreAncesterNodePtr->Right->colorBlack(); 
					}
				}
				else {												// 2023_05_19_csw_NOTE LR
					if (PreAncesterNodePtr == header) {
						PreAncesterNodePtr->Left = AncesterNodePtr->LRRotate();
						PreAncesterNodePtr->Right = PreAncesterNodePtr->Left;
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else if (isLeft(PreAncesterNodePtr, AncesterNodePtr)) {
						PreAncesterNodePtr->Left = AncesterNodePtr->LRRotate();
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else {
						PreAncesterNodePtr->Right = AncesterNodePtr->LRRotate();
						PreAncesterNodePtr->Right->colorBlack(); 
					}
				}
			}
			else {
				if (isLeft(CurNodePtr, NewNodePtr) == true) {		// 2023_05_19_csw_NOTE RL
					if (PreAncesterNodePtr == header) {
						PreAncesterNodePtr->Left = AncesterNodePtr->RLRotate();
						PreAncesterNodePtr->Right = PreAncesterNodePtr->Left;
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else if (isLeft(PreAncesterNodePtr, AncesterNodePtr)) {
						PreAncesterNodePtr->Left = AncesterNodePtr->RLRotate();
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else {
						PreAncesterNodePtr->Right = AncesterNodePtr->RLRotate();
						PreAncesterNodePtr->Right->colorBlack(); 
					}
				}
				else {												// 2023_05_19_csw_NOTE RR
					if (PreAncesterNodePtr == header) {
						PreAncesterNodePtr->Left = AncesterNodePtr->RRRotate();
						PreAncesterNodePtr->Right = PreAncesterNodePtr->Left;
						PreAncesterNodePtr->Left->colorBlack();
					}
					else if (isLeft(PreAncesterNodePtr, AncesterNodePtr)) {
						PreAncesterNodePtr->Left = AncesterNodePtr->RRRotate();
						PreAncesterNodePtr->Left->colorBlack(); 
					}
					else {
						PreAncesterNodePtr->Right = AncesterNodePtr->RRRotate();
						PreAncesterNodePtr->Right->colorBlack(); 
					}
				}
			}
		}

		bool isLeft(const node_pointer &ParentPtr, const node_pointer &ChildPtr) {
			if (ParentPtr->Left == ChildPtr)
				return true;
			else
				return false; 
		}

		bool isRight(const node_pointer& ParentPtr, const node_pointer& ChildPtr) {
			return !isLeft(ParentPtr, ChildPtr); 
		}



		bool erase()

	};
}

#endif // !RBTREEH
