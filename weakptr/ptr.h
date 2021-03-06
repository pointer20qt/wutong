#pragma once

namespace T
{

	template<class T> class T_autoPtr
	{
		T* obj = nullptr;
	public:
		T_autoPtr(T* ptr) :obj(ptr)
		{

		}
		T_autoPtr() = default;
		~T_autoPtr()
		{
			delete obj;
		}
		T_autoPtr(T_autoPtr<T>& ptr) :obj(ptr.release())
		{

		}
		T_autoPtr& operator=(T_autoPtr<T>& ptr)
		{
			obj = ptr.release();
			return *this;
		}
		void reset(T* newPtr = nullptr)
		{
			delete obj;
			obj = newPtr;
		}
		T* release()
		{
			auto temP = obj;
			obj = nullptr;
			return temP;
		}
		T& operator* ()
		{
			return *obj;
		}
		T* operator-> ()
		{
			return obj;
		}
		T* get()
		{
			return obj;
		}
	};

	template<class T> class T_uniquePtr{
		T* obj = nullptr;
	public:
		T_uniquePtr(T* ptr) :obj(ptr)
		{

		}
		T_uniquePtr(const T&) = delete;
		T_uniquePtr() = default;
		~T_uniquePtr()
		{
			delete obj;
		}
		T_uniquePtr& operator=(const T_uniquePtr<T>&) = delete;
		T_uniquePtr(T_uniquePtr<T>&& ptr) :obj(ptr.release())
		{

		}
		T_uniquePtr& operator=(T_uniquePtr<T>&& ptr)
		{
			obj = ptr.release();
			return *this;
		}
		void reset(T* newPtr = nullptr)
		{
			delete obj;
			obj = newPtr;
		}
		T* release()
		{
			auto temP = obj;
			obj = nullptr;
			return temP;
		}
		T& operator*()
		{
			return *obj;
		}
		T* operator->()
		{
			return obj;
		}
		T* get()
		{
			return obj;
		}
	};

	template<class T> class Ref
	{
		int r_count = 0;
		int w_count = 0;
		T* object = nullptr;
	public:
		Ref(T* target) :object(target)
		{
			++r_count;
		}
		inline void increase()
		{
			r_count++;
		}
		inline void increase_w()
		{
			w_count++;
		}
		inline void reduce()
		{
			r_count--;
			if (r_count == 0)
			{
				delete object;
				object = nullptr;
			}
			if (w_count == 0)
			{
				delete this;
			}
		}
		inline void reduce_w()
		{
			w_count--;
			if (r_count == 0 && w_count == 0)
			{
				delete this;
			}
		}
		T* get()
		{
			return object;
		}
		int getCount()
		{
			return r_count;
		}
	};
	template<class T> class TWeakPtr;
	template<class T> class TSharedPtr
	{
		friend class TWeakPtr<T>;
		Ref<T>* ref = nullptr;
	public:
		TSharedPtr() = default;
		~TSharedPtr()
		{
			if (ref)
			{
				ref->reduce();
			}
		}
		TSharedPtr(Ref<T>* newRef)
		{
			ref = newRef;
			if (ref)
			{
				ref->increase();
			}
		}
		TSharedPtr(T* newP)
		{
			ref = new Ref<T>(newP);
		}
		TSharedPtr(const TSharedPtr<T>& other)
		{
			ref = other.ref;
			if (ref)
			{
				ref->increase();
			}
		}
		TSharedPtr(TSharedPtr<T>&& other)
		{
			swap(ref, other.ref);
		}
		TSharedPtr& operator=(const TSharedPtr<T>& other)
		{
			if (ref)
			{
				ref->reduce();
			}
			ref = other.ref;
			if (ref)
			{
				ref->increase();
			}
			return *this;
		}
		TSharedPtr& operator=(TSharedPtr<T>&& other)
		{
			if (ref)
			{
				ref->reduce();
			}
			ref = other.ref;
			other.ref = nullptr;
			return *this;
		}
		void swap(TSharedPtr<T>& other)
		{
			swap(ref, other.ref);
		}
		void reset(T* target = nullptr)
		{
			if (ref)
			{
				ref->reduce();
			}
			if (target != nullptr)
			{
				ref = new Ref<T>(target);
			}
			else
			{
				ref = nullptr;
			}
		}
		T& operator*()
		{
			if (ref)
			{
				return *ref->get();
			}
			else
			{
				return *(T*)nullptr;
			}
		}
		T* operator->()
		{
			if (ref)
			{
				return ref->get();
			}
			else
			{
				return (T*)nullptr;
			}
		}
		T* get()
		{
			if (ref)
			{
				return ref->get();
			}
			else
			{
				return (T*)nullptr;
			}
		}
		int use_count()
		{
			if (ref)
			{
				return ref->getCount();
			}
			else
			{
				return 0;
			}
		}
		bool unique()
		{
			if (ref)
			{
				return ref->getCount() == 1;
			}
			else
			{
				return false;
			}
		}
		operator bool()
		{
			return ref != nullptr;
		}
	};

	template<class T> class TWeakPtr
	{
		Ref<T>* ref = nullptr;
	public:
		TWeakPtr() = default;
		TWeakPtr(TSharedPtr<T>& other) :ref(other.ref)
		{
			cout << "使用shared构造weak的拷贝赋值运算符" << endl;
			if (ref)
			{
				ref->increase_w();
			}
			
		}
		TWeakPtr(const TWeakPtr<T>& other) :ref(other.ref)
		{
			if (ref)
			{
				ref->increase_w();
			}
		}
		TWeakPtr(TWeakPtr<T>&& other)
		{
			swap(ref, other.ref);
		}
		TWeakPtr<T>& operator = (const TWeakPtr<T>& other)
		{
			cout <<"调用了weak的拷贝赋值运算符" << endl;
			if (ref)
			{
				ref->reduce_w();
			}
			ref = other.ref;
			if (ref)
			{
				ref->increase_w();
			}
			return *this;
		}
		TWeakPtr<T>& operator=(TWeakPtr<T>&& other)
		{
			cout << "调用了weak的移动赋值运算符" << endl;
			if (ref)
			{
				ref->reduce_w();
			}
			ref = other.ref;
			other.ref = nullptr;
			return *this;
		}
		int use_count()
		{
			if (ref)
			{
				return ref->getCount();
			}
			else
			{
				return 0;
			}
		}
		bool expired()
		{
			if (ref)
			{
				return ref->getCount() > 0;
			}
			else
			{
				return false;
			}
		}
		TSharedPtr<T> lock()
		{
			TSharedPtr<T> tem(ref);
			return tem;
		}
		void swap(TWeakPtr<T>& other)
		{
			swap(ref, other.ref);
		}
		void reset()
		{
			ref->reduce_w();
			ref = nullptr;
		}
	};
}