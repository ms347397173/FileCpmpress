#pragma once


#include<vector>
using namespace std;

//����
template<class TYPE>
class AS_CMP
{
public:
	bool operator() (TYPE & child, TYPE & root)   //����Ϊtrue�򽻻����൱��ν��
	{
		return child > root ? true : false;
	}
};

//����
template<class TYPE>
class DE_CMP
{
public:
	bool operator() (TYPE & child, TYPE & root)
	{
		return child < root ? true : false;
	}
};


//��
template<class T, class CMP = DE_CMP>
class Heap
{
public:
	//Ĭ�Ϲ���  ����һ���յĶ�  С����
	Heap()
	{}
	//�����鹹��һ����
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

	void Pop()   //ɾ���Ѷ�
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

	const T & Top() throw(exception &)  // ȡ�Ѷ�����С�ģ�
	{
		if (_array.size() > 0)
			return _array[0];
		else
			throw exception("ȡ�Ѷ�ʧ�� �Ѷ�Ϊ��");
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
		while (root <= (_array.size() - 2) / 2)  //ֻҪroot��֤Ϊ���ն˽��
		{
			int child = root * 2 + 1;  //���ӿ϶����� �ж����Һ���
			if (child + 1 < _array.size())  //�����Һ���
			{
				child = (cmp(_array[child], _array[child+1] ))? child  : child+1;  //�Ǹ�С������һ��
			}
			if (cmp(_array[child],_array[root]))
			{
				swap(_array[child], _array[root]); //����
			}
			root = child;
		}
	}

	void AdjustUp(int child)
	{
		//��ȷ��ǰ����child�ǺϷ��ģ��ǵ����ߵ�����
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