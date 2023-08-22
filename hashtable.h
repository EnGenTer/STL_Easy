#pragma once
/*
* 2023_05_23_csw_NOTE
* 2023_05_23_csw_TODO 没有设计好rehash机制。目前是使用bucket_size()的一般代替的
*/

#include "list.h"
#include "allocator.h"
#include "simple_allocator.h"

namespace MySTL {
	static const unsigned int _HashtableBucketSize[12] = { 53,97,193,389,769,1543,3079,6151,12289,24593,49157,98317 };

	template <typename Key>
	class SimpleEqualKey {
	public:
		bool operator()(const Key& k1, const Key& k2) { return k1 == k2; }
	};

	template <typename Value>
	class hash_table_node {
	public:
		Value Val = Value(); 
		hash_table_node* next = nullptr; 

		hash_table_node(Value Val) : Val(Val), next(nullptr) {}
		hash_table_node(Value Val, hash_table_node *next) : Val(Val), next(next) {}
	};

	template <typename Value, typename Reference, typename Pointer>
	class hash_table_iterator {
	public:
		typedef input_iterator_tag			iterator_category;
		typedef ptrdiff_t					difference_type;
		typedef hash_table_node<Value>		hash_node;
		typedef hash_node*					hash_node_pointer;
		typedef hash_node_pointer*			bucket; 
		typedef hash_table_iterator<Value, Reference, Pointer> self; 

		bucket Start, Finish, CurBucket; 
		hash_node_pointer CurNode;

		hash_table_iterator():Start(nullptr), Finish(nullptr), CurNode(nullptr) {};
		hash_table_iterator(bucket start, bucket finish, bucket curbucket, hash_node_pointer curnode) :
			Start(start), Finish(finish), CurBucket(curbucket), CurNode(curnode) {}

		Reference operator*() { return (Reference)(CurNode->Val); };
		Pointer operator&() { return (Pointer)(CurNode->Val); };
		self& operator++() {
			if (CurNode->next != nullptr) {
				CurNode = CurNode->next; 
				return *this; 
			}
			++CurBucket;
			while (CurBucket != Finish) ++CurBucket;
			return *this; 
		}
		self operator++(int) {
			self tempit = *this; 
			++*this; 
			return tempit; 
		}
		bool operator==(self it) {
			return CurNode == it.CurNode; 
		}
	};

	template <typename Key>
	class HashFunctor{ };

	template <>
	class HashFunctor<int> {
	public:
		size_t operator()(int Val) { return (size_t)Val; }
	};

	template <typename Key, typename Value = Key, typename ExtractKey = KeyIsValue<Value>,
	typename EqualKey = SimpleEqualKey<Key>, typename HashFunc = HashFunctor<Key>, typename Alloc = Simple_Allocator>
	class hash_table {
	public:
		typedef Key					key_type; 
		typedef Value				value_type; 
		typedef value_type*			value_pointer;
		typedef value_type&			value_reference;
		typedef hash_table_node<value_type> hash_node;
		typedef Allocator<hash_node, Alloc> hash_node_allocator;
		typedef hash_node*			hash_node_pointer;
		typedef Allocator<hash_node_pointer, Alloc> bucket_allocator; 
		typedef hash_table_iterator<value_type, const value_pointer, const value_reference> iterator; 

		EqualKey isEqual; 
		ExtractKey key; 
		HashFunc hash_key; 

		hash_node_pointer* bucket;
		// 2023_05_23_csw_QUES 不知道为什么不能用static修饰
		unsigned int BucketSizeIdx = 0; 
		size_t NodeNum = 0; 

		hash_table() { 
			bucket = bucket_allocator::allocate(_HashtableBucketSize[BucketSizeIdx]); 
			fillBucketByNullptr(bucket, _HashtableBucketSize[BucketSizeIdx]); 
		}
		inline const size_t bucket_size() const { return _HashtableBucketSize[BucketSizeIdx]; }
		iterator begin() { return ++(iterator(bucket, bucket + bucket_size(), bucket, *bucket)); }
		iterator end() { return iterator(bucket, bucket + bucket_size(), bucket + bucket_size(), *(bucket + bucket_size())); }
		size_t size() { return NodeNum; }

		iterator insert(value_type Val) {
			if (NodeNum > ((bucket_size() * bucket_size()) >> 1))
				rehash(); 
			size_t bucketidx = hash_key(key(Val)) % bucket_size(); 
			hash_node_pointer newnode = hash_node_allocator::allocate(); 
			newnode->next = *(bucket + bucketidx); 
			newnode->Val = Val;
			*(bucket + bucketidx) = newnode; 
			++NodeNum; 
			return iterator(bucket, bucket + bucket_size(), bucket + bucketidx, newnode); 
		}

		iterator find(value_type Val) const {
			size_t bucketidx = hash_key(key(Val)) % bucket_size();
			hash_node_pointer nodeptr = *(bucket + bucketidx); 
			while (nodeptr != nullptr) {
				if (isEqual(key(Val), key(nodeptr->Val)) == true) 
					return iterator(bucket, bucket + bucket_size(), bucket + bucketidx, nodeptr); 
				nodeptr = nodeptr.next;
			}
			return end(); 
		}

		// 2023_05_23_csw_NOTE 如果设置成双向链表倒还是可以试试和find()一起使用
		bool erase(const value_type &Val) {
			size_t bucketidx = hash_key(key(Val)) % bucket_size();
			hash_node_pointer nodeptr = *(bucket + bucketidx);
			hash_node_pointer eraseptr; 
			// 2023_05_23_csw_TODO 如果第一个节点使用的dummy head就可以统一一下操作
			if (nodeptr == nullptr) return false;
			else if (isEqual(key(nodeptr->Val), key(Val)) == true) {
				eraseptr = nodeptr; 
				nodeptr = eraseptr->next; 
				hash_node_allocator::deallocate(eraseptr); 
				--NodeNum;
				return true;
			}
			else {
				eraseptr = nodeptr->next; 
				while (eraseptr != nullptr) {
					if (isEqual(key(eraseptr->Val), key(Val)) == true) {
						nodeptr->next = eraseptr->next;
						hash_node_allocator::deallocate(eraseptr);
						--NodeNum;
						return true; 
					}
					nodeptr = eraseptr; 
					eraseptr = eraseptr->next; 
				}
			}
			return false; 
		}

		void rehash() {
			size_t newbucketsize = _HashtableBucketSize[BucketSizeIdx + 1], newbucketidx; 
			hash_node_pointer* newbucket = bucket_allocator::allocate(newbucketsize);
			fillBucketByNullptr(newbucket, newbucketsize); 
			hash_node_pointer curnode, nextnode; 
			for (size_t bucketidx = 0; bucketidx < bucket_size(); ++bucketidx) {
				curnode = *(bucket + bucketidx); 
				while (curnode != nullptr) {
					nextnode = curnode->next; 
					newbucketidx = hash_key(key(curnode->Val)) % newbucketsize;
					curnode->next = *(newbucket + newbucketidx); 
					*(newbucket + newbucketidx) = curnode; 
					curnode = nextnode; 
				}
			}
			bucket = newbucket;
			++BucketSizeIdx;
		}

		void fillBucketByNullptr(hash_node_pointer* bucket, size_t bucketsize) {
			for (size_t idx = 0; idx < bucketsize; ++idx)
				*(bucket + idx) = nullptr; 
		}
	};
}