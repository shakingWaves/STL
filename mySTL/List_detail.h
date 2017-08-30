#include "List.h"
#ifndef _LIST_DETAIL_H_
#define _LIST_DETAIL_H_
namespace STL {

	namespace Detail {//         Detail namespace
		template<class T>
		list_iterator<T>& list_iterator<T>::operator ++() {
			p = p->next;
			return *this;
		}

		template<class T>
		inline list_iterator<T> list_iterator<T>::operator ++(int) {
			auto res = *this;
			++*this;
			return res;
		}

		template<class T>
		inline list_iterator<T>& list_iterator<T>::operator--() {
			p = p->prev;
			return *this;
		}

		template<class T>
		inline list_iterator<T> list_iterator<T>::operator--(int) {
			auto res = *this;
			--*this;
			return res;
		}
		template<class T>
		bool operator==(const list_iterator<T>& lhs, const list_iterator<T>& rhs) {
			return lhs.p == rhs.p;
		}
		template<class T>
		bool operator!=(const list_iterator<T>& lhs, const list_iterator<T>& rhs) {
			return false;
		}
	}//end of Detail

	//                                    构造函数
	template<class T>
	inline list<T>::list()
	{
		head.p = NewNode();
		tail.p = head.p;
	}
	//__list 泛型的划分
	template<class T>
	inline list<T>::list(size_type n, const value_type & val)
	{
		__list(n, val, std::is_integral<value_type>());
	}
	template<class T>
	template <class InputIterator>
	list<T>::list(InputIterator first, InputIterator last) {
		__list(first, last, std::is_integral<InputIterator>());
	}
	template<class T>
	inline list<T>::list(const list & lis)
	{
		//__list(lis.begin(), lis, end(), std::is_integral<lis.begin()>());
		//不能直接使用

		head.p = newNode();
		tail.p = head.p;
		for (auto node = l.head.p; node != l.tail.p; node = node->next)
			push_back(node->data);
	}

	template<class T>
	inline list & list<T>::operator=(const list & rhs)
	{

	}
	template<class T>
	inline list<T>::~list()
	{
		for (; head != tail;) {
			auto temp = head++;
			nodeAllocator::destroy(temp.p);
			nodeAllocator::deallocate(temp.p);
		}
		nodeAllocator::deallocate(tail.p);
	}


	//                            空间配置器相关
	template<class T>
	template<class InputIterator>
	inline void list<T>::__list(InputIterator first, InputIterator last, std::false_type)
	{
		head.p = NewNode();
		tail.p = NewNode();
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	template<class T>
	inline void list<T>::__list(size_type n, const value_type & val, std::true_type)
	{
		head.p = NewNode();
		tail.p = NewNode();
		while (n--) {
			push_back(val);
		}
	}


	template<class T>
	inline typename list<T>::nodePtr list<T>::newNode(const T& value = T())
	{
		nodePtr node = nodeAllocator::allocate();
		nodeAllocator::construct(node, Detail::__list_node<T>(value, nullptr, nullptr));
		return node;
	}
	template<class T>
	inline void list<T>::deleteNode(nodePtr p)
	{
		p->prev = p->next = nullptr;
		nodeAllocator::destroy(p);
		nodeAllocator::deallocate(p);
	}
	//                         元素操作相关
	template<class T>
	void list<T>::remove(const value_type& val) {
		for (auto it = begin(); it != end();) {
			if (*it == val)
				it = erase(it);
			else
				++it;
		}
	}
	template<class T>
	template<class Func>
	void list<T>::remove_if(Func ifFunc){
		for (auto it = begin(); it != end();) {
			if (ifFunc(*it))
				it = erase(it);
			else
				++it;
		}
	}

	template<class T>
	typename list<T>::iterator list<T>::insert(iterator position, const value_type& value) {
		nodePtr tmp = newNode(value);
		auto prev = position.p->prev;
		
		tmp->next = position.p;
		tmp->prev = prev;

		prev->next = tmp;
		position.p->prev = tmp;
		
		return (iterator)tmp;//类型转换，存在构造函数可以进行转换
	}

	template<class T>
	typename list<T>::iterator list<T>::erase(iterator position) {
		auto prev = position.p->prev;
		prev->next = position.p->next;
		position.p->next->prev = prev;
		deleteNode(position.p);
		return iterator(prev->next);//指向被删除节点的下一个节点
	}
	template<class T>
	typename list<T>::iterator list<T>::erase(iterator first, iterator last) {
		typename list<T>::iterator res;
		for (; first != last;) {
			auto temp = first++;
			res = erase(temp);
		}
		return res;
	}
	template<class T>
	void list<T>::unique()//移出数值相同且连续的元素,直到只剩下单个
	{
		
		iterator first = begin();
		iterator last = end();
		if (first == last) {
			return;
		}
		iterator next = first;
		while (++next != last) {
			if (*first == *next) {//有相同节点，移出一个
				erase(next);
			}
			else {
				first = next;    //调整
			}
			next = first;
		}
	}
	template<class T>
	void list<T>::clear() {
		erase(begin(), end());
	}
	template<class T>
	void list<T>::push_front(const value_type& val) {
		insert(begin(), val);
	}
	template<class T>
	void list<T>::pop_front() {
		erase(begin());
	}
	template<class T>
	void list<T>::push_back(const value_type& val) {
		insert(end(), x);
	}
	template<class T>
	void list<T>::pop_back() {
		iterator tmp = end();
		erase(--tmp);
	}
	//                                容量相关

	template<class T>
	typename list<T>::size_type list<T>::size()const {
		size_type length = 0;
		for (auto h = head; h != tail; ++h)
			++length;
		return length;
	}
	//                               元素访问
	template<class T>
	typename list<T>::iterator list<T>::begin() {
		return head;
	}
	template<class T>
	typename list<T>::iterator list<T>::end() {
		return tail;
	}

	//                              比较函数
	template <class T>
	bool operator== (const list<T>& lhs, const list<T>& rhs) {
		auto nodeL = lhs.head.p, nodeR = rhs.head.p;
		for (; nodeL != lhs.tail.p && nodeR != rhs.tail.p; nodeL = nodeL->next, nodeR = nodeR->next) {
			if (nodeL->data != nodeR->data)
				break;
		}
		if (nodeL == lhs.tail.p && nodeR == rhs.tail.p)
			return true;
		return false;
	}
	template <class T>
	bool operator!= (const list<T>& lhs, const list<T>& rhs) {
		return !(lhs == rhs);
	}
	
}
#endif // !_LIST_DETAIL_H_

