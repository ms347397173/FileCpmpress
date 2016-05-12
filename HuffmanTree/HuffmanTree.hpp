//��������
#ifndef _HUFFMAN_TREE_
#define _HUFFMAN_TREE_

#include"Heap.h"

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const T& x)
	:_left(NULL)
	, _right(NULL)
	, _parents(NULL)
	, _weight(x)   //�ⶨ��Ϊ0 ��̫���� �����Ϊ����T���͵�Ĭ�Ϲ��죬������POD���Ͳ�����
	{}

	HuffmanTreeNode<T> * _left;
	HuffmanTreeNode<T> * _right;
	HuffmanTreeNode<T> * _parents;
	T _weight;
};

template<class T>
class HuffmanTree
{
public:
	template<class TYPE>
	struct Compare
	{
		bool operator()(const TYPE & a, const TYPE & b)
		{
			return a->_weight < b->_weight ? true : false;
		}
	};

	
public:
	HuffmanTree()
		:_root(NULL)
	{}

	~HuffmanTree()
	{}

	//��ҪT����֧�� != �� <
	void CreateHuffmanTree(const T* arr, size_t size,const T& invalid)
	{
		//Heap<T> hp(arr,size);

		//while (hp.Top() == invalid)
		//{
		//	hp.Pop();  //ȥ��ȨֵΪ0��
		//}

		if (!arr || !size)
			return;

		Heap<HuffmanTreeNode<T>*, Compare<HuffmanTreeNode<T>*>> minHeap;

		for (int i = 0; i<size; ++i)
		{
			if (arr[i] != invalid)
			{
				HuffmanTreeNode<T> * node = new HuffmanTreeNode<T>(arr[i]);
				minHeap.Push(node);
			}
		}

		//������

		if (minHeap.Size() == 1)
		{
			_root = new HuffmanTreeNode<T>(arr[0]);
			_root->_left = minHeap.Top();
			minHeap.Pop();
			_root->_right = NULL;
			
			return;

		}

		if (minHeap.Size() == 0)
		{
			return;
		}

		while (minHeap.Size() > 1)
		{
			//�ҳ���С������
			HuffmanTreeNode<T> * left = minHeap.Top();
			minHeap.Pop();
			HuffmanTreeNode<T> * right = minHeap.Top();
			minHeap.Pop();

			//T������Ҫ֧�� operator+
			HuffmanTreeNode<T> * node = new HuffmanTreeNode<T>(left->_weight + right->_weight);

			node->_left = left;
			node->_right = right;
			left->_parents = node;
			right->_parents = node;

			minHeap.Push(node);

		}
		_root = minHeap.Top();  //�����ڵ㸳ֵ
		minHeap.Pop();
		
	
	}

	void _CountCode(HuffmanTreeNode<T> * root,string code,T * arr)
	{
		if (root->_left == NULL && root->_right == NULL)
		{
			root->_weight._code = code;
			arr[root->_weight._ch]._code = code;

			return;
		}
		if (root->_left)
			_CountCode(root->_left, code + "0",arr);
		if (root->_right)
			_CountCode(root->_right, code + "1",arr);
		

	}

	void CountCode(T * arr)
	{
		string code;
		_CountCode(_root, code, arr);
	}

	HuffmanTreeNode<T> * GetRoot()
	{
		return _root;
	}


protected:
	HuffmanTreeNode<T> * _root;
};


#endif
