#pragma once
#ifndef _VECTOR_DETAIL_H
#define _VECTOR_DETAIL_H
namespace STL {
	//*********************************构造、析构**************************************

	template<class T, class Alloc>
	vector<T, Alloc>::~vector() {
		deallocate();
	}

	template<class T, class Alloc>
	vector<T, Alloc>::vector(const size_type n) {
		allocate_and_fill_n(n, value_type());
	}
	template<class T, class Alloc>
	vector<T, Alloc>::vector(size_type n, const value_type& value) {
		allocate_and_fill_n(n, value);
	}

	//********************************空间配置器相关*********************

	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::reallocateAndCopy(iterator position, InputIterator first, InputIterator last) {

		difference_type newCapacity = getNewCapacitySize(last - first);
		iterator newStart = data_Allocator::allocate(newCapacity);
		iterator newEndOfStorage = newStart + newCapacity;

		iterator NewFinish = STL::uninitialized_copy(begin(), position, newStart);
		NewFinish = STL::uninitialized_copy(first, last, position);
		NewFinish = STL::uninitialized_copy(position, end(), NewFinish);

		deallocate();

		start_ = newStart;
		finish_ = NewFinish;
		end_of_storage_ = newEndOfStorage;
		
	}
	template<class T, class Alloc>
	void vector<T, Alloc>::reallocateAndFillN(iterator position
		, const size_type& n
		, const value_type& value) {
		difference_type newCapacity = getNewCapacitySize(last - first);
		iterator newStart = data_Allocator::allocate(newCapacity);
		iterator newEndOfStorage = newStart + newCapacity;

		iterator NewFinish = STL::uninitialized_copy(begin(), position, newStart);
		NewFinish = STL::uninitialized_fill_n(NewFinish, n, value);
		NewFinish = STL::uninitialized_copy(position, end(), NewFinish);

		deallocate();

		start_ = newStart;
		finish_ = NewFinish;
		end_of_storage_ = newEndOfStorage;
	}
	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::__insert(iterator position
		, InputIterator first
		, InputIterator last) {
		difference_type leftSize = end_of_storage_ - finish_;  //容器中剩余空间
		difference_type needSize = distance(first, last);// last - first
		if (leftSize >= needSize) {
			if (finish_ - position > needSize) {
				STL::uninitialized_copy(fininsh_ - needSize, finish_, finish_);
				std::copy_backward(position, finish_ - needSize, finish_);
				std::copy(first, last, position);
			}
			else {
				iterator temp = STL::uninitialized_copy(first + (finish_ - position), last, finish_);
				STL::uninitialized_copy(position, finish_, temp);
				std::copy(first, first + (finish_ - position), position);		
			}
			finish_ += needSize;
		}
		else {
			reallocateAndCopy();
		}
	}
	
	template<class T, class Alloc>
	void vector<T, Alloc>::__insert(iterator position
		, size_type n
		, const value_type& value) {
		assert(n != 0);
		difference_type LeftSize = end_of_storage_ - finish_;
		difference_type needSize = n;
		
		if (LeftSize >= needSize) {
			iterator temp = end() - 1;
			for (; temp - position != 0; ++temp) {
				STL::construct(temp + needSize, *temp);
			}
			STL::uninitialized_fill_n(position, n, value);
			finish_ += n;
		}
		else {
			reallocateAndFillN(position, n, value);
		}
	}


	template<class T, class Alloc>
	void vector<T, Alloc>::fill_initialize(size_t n, const value_type& value) {
		allocate_and_fill_n(n, value);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::allocate_and_fill_n(size_type n, const T& value) {
		start_ = data_Allocater::allocate(n);//配置n个元素的空间
		STL::uninitialized_fill_n(start_, start_ + n, value);
		finish_ = end_of_storage_ = start_ + n;
	}
	template<class T, class Alloc>
	void vector<T, Alloc>::deallocate() {
		if (start_) {//存在被配置过的空间
			data_Allocator::destroy(start_,finish_);
			data_Allocator::deallocate(start_, capacity());
		}
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::clear() {
		data_Allocator::destroy(start_, finish_);
		finish_ = start_;
	}
	template<class T, class Alloc>
	void vector<T, Alloc>::pop_back() {
		--finish_;
		data_Allocator::destroy(finish_);
	}

	//*********************************容器比较函数*****************************

	template<class T, class Alloc>
	bool vector<T, Alloc>::operator ==(const vector& rhs) {
		if (rhs.size() != this->size()) {
			return false;
		}
		auto ptrLhs = this->start_;
		auto ptrRhs = rhs.start_;
		for (; ptrLhs != this->finish_ && ptrRhs != rhs.finish_; ++ptrLhs, ++ptrRhs) {
			if (*ptrLhs != *ptrRhs)
				return false;
		}
		return true;
	}
	template<class T, class Alloc>
	bool vector<T, Alloc>::operator !=(const vector& v) {
		return !(*this == v);
	}

	template<class T, class Alloc>
	bool operator == (const vector<T, Alloc>& v1, const vector<T, Alloc>& v2) {

		return v1.operator==(v2);
	}
	template<class T, class Alloc>
	bool operator != (const vector<T, Alloc>& v1, const vector<T, Alloc>& v2) {

		return !(v1 == v2);
	}

	//*******************************对容器内部元素进行修改的函数******************

	template<class T, class Alloc>
	void  vector<T, Alloc>::push_back(const value_type & value) {
		insert(end(), value);
	}
	template<class T, class Alloc>
	void vector<T, Alloc>::insert(iterator position
		, const size_type& n
		, const value_type& value) {

		__insert(position, n, value);
	}
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::insert
	(iterator position, const value_type& value) {
		const auto index = position - begin();//index防止空间不足造成的迭代器position失效
		insert(position, 1, value);
		return begin() + index;
	}
	template<class T, class Alloc>
	template <class InputIterator>
	void vector<T, Alloc>::insert(iterator position
		, InputIterator first
		, InputIterator last) {
		__insert(position, first, last);
	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position) {
		return erase(position, position + 1);
	}
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase
	(iterator first, iterator last) {
		difference_type tailLeftItemsNum = end() - last;//尾部剩余的元素
		difference_type removedItemsNum = last - first;//需要移出的元素
		finish_ = finish_ - removedItemsNum;
		for (; tailLeftItemsNum != 0; --tailLeftItemsNum) {
			auto temp = (last - removedItemsNum);
			*temp = *(last--);
		}
		return first;
	}
	//*******************************容器容量相关的操作函数********************


	//杂项
	template<class T, class Alloc>
	typename vector<T, Alloc>::size_type vector<T, Alloc>::getNewCapacitySize(size_type n)const {
		size_type oldCapacity= end_of_storage_ - start_;
		auto res = STL::max(oldCapacity, n);
		size_type newCapacity = (oldCapacity != 0 ? (oldCapacity + res) : n);
		return newCapacity;
	}
}
#endif