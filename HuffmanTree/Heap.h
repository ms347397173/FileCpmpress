#pragma once


#include<vector>
using namespace std;

//升序
template<class TYPE>
class AS_CMP
{
public:
	bool operator() (TYPE & child, TYPE & root)   //条件为true则交换，相当于谓词
	{
		return child > root ? true : false;
	}
};

//降序
template<class TYPE>
class DE_CMP
{
public:
	bool operator() (TYPE & child, TYPE & root)
	{
		return child < root ? true : false;
	}
};


//堆
template<class T, class CMP = DE_CMP>
class Heap
{
public:
	//默认构造  构造一个空的堆  小根堆
	Heap()
	{}
	//用数组构造一个堆
	Heap(const T *  arr, size_t size)
	{
		for (int i = 0; i < size; ++i)
		{
			_array.push_back(arr[i]);
		}
		int root = (_array.size() - 2) / 2;
		while (root >= 0)
		{
			AdjustDown(root--);
		}

	}

	void Pop()   //删除堆顶
	{
		if (_array.size() == 0)
		{
			return;
		}
		if (_array.size() == 1)
		{
			_array.pop_back();
		}
		else
		{
			swap(_array[0], _array[_array.size() - 1]);
			_array.pop_back();
			AdjustDown(0);
		}
	}

	const T & Top() throw(exception &)  // 取堆顶（最小的）
	{
		if (_array.size() > 0)
			return _array[0];
		else
			throw exception("取堆顶失败 堆顶为空");
	}

	void Push(T & t)
	{
		_array.push_back(t);
		AdjustUp(_array.size() - 1);
	}

	int Size()
	{
		return _array.size();
	}


protected:
	void AdjustDown(int root)
	{
		CMP cmp;
		if (_array.size() <= 1)
			return;
		while (root <= (_array.size() - 2) / 2)  //只要root保证为非终端结点
		{
			int child = root * 2 + 1;  //左孩子肯定存在 判断下右孩子
			if (child + 1 < _array.size())  //存在右孩子
			{
				child = (cmp(_array[child], _array[child+1] ))? child  : child+1;  //那个小返回那一个
			}
			if (cmp(_array[child],_array[root]))
			{
				swap(_array[child], _array[root]); //交换
			}
			root = child;
		}
	}

	void AdjustUp(int child)
	{
		//正确的前提是child是合法的，是调用者的责任
		int root;
		CMP cmp;
		while (child > 0)
		{
			root= (child - 1 )/ 2;
			if (cmp(_array[child], _array[root]))
			{
				swap(_array[root], _array[child]);
			}
			child = root;
		}
		
	}

protected:
	vector<T> _array;
};