#pragma once
#include <cassert>

class StackAllocator
{
public:
	StackAllocator(size_t capacity=65536);
	template<typename T>
	
	T* New()
	{
		size_t size = sizeof(T);
		size_t spaceSoFar = head - buffer;
		if (spaceSoFar + size > capacity)
		{
			//System MessageBox("Ran out of mem)
			//assert(false);
			return nullptr;
		}
		char* oldHead = head;
		head += size;

		new (oldHead) T();
		return (T*)oldHead;
	}

	template <typename T>
	T* StartArray();

	template <typename T>
	T* PushArray();

	void Clear();

	//void* Data
	//{
		//return buffer;
//	}

	void PopTo(void* object);

private:
	char* buffer;
	size_t capacity;
	char* head;
};

StackAllocator::StackAllocator(size_t capacity) :
	buffer(new char[capacity]),
	capacity(capacity),
	head(buffer)
{

}

void StackAllocator::Clear()
{
	head = buffer;
}

template <typename T>
T* StackAllocator::StartArray()
{
	return (T*)head;
}

template <typename T>
T* StackAllocator::PushArray()
{
	return New<T>();
}