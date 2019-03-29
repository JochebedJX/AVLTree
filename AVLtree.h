#pragma once

#include<iostream>

using namespace std;

template<class T,class V>
struct AVLTreeNode
{
	AVLTreeNode(const T& key,const V& value)
	: _pLeft(nullptr)
	, _pRight(nullptr)
	, _pParent(nullptr)
	, _key(key)
	, _value(value)
	, _bf(0)
	{}
	AVLTreeNode<T,V>* _pLeft; // �ýڵ������
	AVLTreeNode<T,V>* _pRight; // �ýڵ���Һ���
	AVLTreeNode<T,V>* _pParent; // �ýڵ��˫��

	T _key;
	V _value;

	int _bf; // �ýڵ��ƽ������
};

template<class T,class V>
class AVLTree
{
public:
	typedef AVLTreeNode<T, V> Node;
	AVLTree() :_pRoot(nullptr){}
	bool Insert(const T& key,const V& value)
	{
		// 1. �Ȱ��ն����������Ĺ��򽫽ڵ���뵽AVL����
		if (nullptr == _pRoot)
		{
			_pRoot = new Node(key,value);
			return true;
		}
		// ���ն�����������������key��AVL�еĲ���λ��
		Node* pCur = _pRoot;
		Node* pParent = nullptr;
		while (pCur)
		{
			pParent = pCur;
			if (key < pCur->_key)
				pCur = pCur->_pLeft;
			else if (key > pCur->_key)
				pCur = pCur->_pRight;
			else
				return false; // �ýڵ��ڶ����������д���
		}
		// �����½ڵ㣺�½ڵ�һ��������pParent���������Ҳ�
		pCur = new Node(key, value);
		if (key < pParent->_key)
			pParent->_pLeft = pCur;
		else
			pParent->_pRight = pCur;
		// ����pCur��˫�׽ڵ�
		pCur->_pParent = pParent;

		// 2. �½ڵ�����AVL����ƽ���Կ��ܻ��⵽�ƻ�����ʱ����Ҫ����ƽ�����ӣ�������Ƿ��ƻ���AVL����ƽ����
		/*
		pCur�����pParent��ƽ������һ����Ҫ�������ڲ���֮ǰ��pParent��ƽ�����ӷ�Ϊ���������-1��0, 1, ���������������
		1. ���pCur���뵽pParent����ֻ࣬���pParent��ƽ������-1����
		2. ���pCur���뵽pParent���Ҳֻ࣬���pParent��ƽ������+1����
		��ʱ��pParent��ƽ�����ӿ��������������0������1������2
		1. ���pParent��ƽ������Ϊ0��˵������֮ǰpParent��ƽ������Ϊ����1������󱻵�����0����ʱ����AVL�������ʣ�����ɹ�
		2. ���pParent��ƽ������Ϊ����1��˵������ǰpParent��ƽ������һ��Ϊ0������󱻸��³�����1����ʱ��pParentΪ�������ĸ߶����ӣ���Ҫ�������ϸ���
		3. ���pParent��ƽ������Ϊ����2����pParent��ƽ������Υ��ƽ���������ʣ���Ҫ���������ת����
		*/
		while (pParent)
		{
			// ����˫�׵�ƽ������
			if (pCur == pParent->_pLeft)
				pParent->_bf--;
			else
				pParent->_bf++;
			// ���º���˫�׵�ƽ������
			if (0 == pParent->_bf)
				break;
			else if (1 == pParent->_bf || -1 == pParent->_bf)
			{
				// ����ǰ˫�׵�ƽ��������0�������˫�׵�ƽ����ΪΪ1����-1��˵����˫��Ϊ���Ķ������ĸ߶�������һ�㣬�����Ҫ�������ϵ���
				pCur = pParent;
				pParent = pCur->_pParent;
			}
			else
			{
				// ˫�׵�ƽ������Ϊ����2��Υ����AVL����ƽ���ԣ���Ҫ����pParentΪ������������ת����
				if (2 == pParent->_bf)
				{
					//������������˫��
					if (pParent->_pRight->_bf == 1)
						_RotateL(pParent);
					else
						_RotateRL(pParent);
					break;
				}
				else
				{
					//�ҵ���������˫��
					if (pParent->_pLeft->_bf == -1)
						_RotateR(pParent);
					else
						_RotateLR(pParent);
					break;
				}
			}
		}
		return true;
	}

	/*
	��ͼ�ڲ���ǰ��AVL����ƽ��ģ��½ڵ���뵽30��������(ע�⣺�˴���������)�У�30������������һ�㣬������60Ϊ���Ķ�������ƽ�⣬Ҫ��60ƽ�⣬ֻ�ܽ�60�������ĸ߶ȼ���һ�㣬����������һ�㣬
	���������������ᣬ����60ת��������Ϊ60��30��ֻ�ܽ������30���������������30��������������������ֵһ������30��С��60��ֻ�ܽ������60������������ת��ɺ󣬸��½ڵ��ƽ�����Ӽ��ɡ�
	����ת�����У������¼��������Ҫ���ǣ�
	1. 30�ڵ���Һ��ӿ��ܴ��ڣ�Ҳ���ܲ�����
	2. 60�����Ǹ��ڵ㣬Ҳ����������
	����Ǹ��ڵ㣬��ת��ɺ�Ҫ���¸��ڵ�
	�����������������ĳ���ڵ����������Ҳ������������
	*/
	void _RotateR(Node* pParent)
	{
		// pSubL: pParent������
		// pSubLR: pParent���ӵ��Һ��ӣ�ע�⣺��
		Node* pSubL = pParent->_pLeft;
		Node* pSubLR = pSubL->_pRight;
		// ��ת���֮��30���Һ�����Ϊ˫�׵�����
		pParent->_pLeft = pSubLR;
		// ���30�����ӵ��Һ��Ӵ��ڣ�������˫��
		if (pSubLR)
			pSubLR->_pParent = pParent;
		// 60 ��Ϊ 30���Һ���
		pSubL->_pRight = pParent;

		// ��Ϊ60�����ǿ�����������ڸ�����˫��ǰ�����ȱ���60��˫��
		Node* pPParent = pParent->_pParent;

		// ����60��˫��
		pParent->_pParent = pSubL;

		// ����30��˫��
		pSubL->_pParent = pPParent;
		// ���60�Ǹ��ڵ㣬����ָ����ڵ��ָ��
		if (NULL == pPParent)
		{
			_pRoot = pSubL;
			pSubL->_pParent = NULL;
		}
		else
		{
			// ���60����������������˫�׵���������Ҳ������������
			if (pPParent->_pLeft == pParent)
				pPParent->_pLeft = pSubL;
			else
				pPParent->_pRight = pSubL;
		}
		// ���ݵ�����Ľṹ���²��ֽڵ��ƽ������
		pParent->_bf = pSubL->_bf = 0;
	}

	void _RotateL(Node* pParent)
	{
		// pSubL: pParent���Һ���
		// pSubLR: pParent�Һ��ӵ�����
		Node* pSubR = pParent->_pRight;
		Node* pSubRL = pSubR->_pLeft;
		// ��ת���֮��30��������Ϊ˫�׵��Һ���
		pParent->_pRight = pSubRL;
		// ���30���Һ��ӵ����Ӵ��ڣ�������˫��
		if (pSubRL)
			pSubRL->_pParent = pParent;
		// 60 ��Ϊ 30������
		pSubR->_pLeft = pParent;

		// ��Ϊ60�����ǿ�����������ڸ�����˫��ǰ�����ȱ���60��˫��
		Node* pPParent = pParent->_pParent;

		// ����60��˫��
		pParent->_pParent = pSubR;

		// ����30��˫��
		pSubR->_pParent = pPParent;
		// ���60�Ǹ��ڵ㣬����ָ����ڵ��ָ��
		if (NULL == pPParent)
		{
			_pRoot = pSubR;
			pSubR->_pParent = NULL;
		}
		else
		{
			// ���60����������������˫�׵���������Ҳ������������
			if (pPParent->_pRight == pParent)
				pPParent->_pRight = pSubR;
			else
				pPParent->_pLeft = pSubR;
		}
		// ���ݵ�����Ľṹ���²��ֽڵ��ƽ������
		pParent->_bf = pSubR->_bf = 0;
	}

	// ��ת֮ǰ��60��ƽ�����ӿ�����-1/0/1����ת���֮�󣬸�������������ڵ��ƽ�����ӽ��е���
	void _RotateLR(Node* pParent)
	{
		Node* pSubL = pParent->_pLeft;
		Node* pSubLR = pSubL->_pRight;

		// ��ת֮ǰ������pSubLR��ƽ�����ӣ���ת���֮����Ҫ���ݸ�ƽ�����������������ڵ��ƽ������
			int bf = pSubLR->_bf;

		// �ȶ�30��������
		_RotateL(pParent->_pLeft);

		// �ٶ�90�����ҵ���
		_RotateR(pParent);
		if (1 == bf)
			pSubL->_bf = -1;
		else if (-1 == bf)
			pParent->_bf = 1;
	}

	// ��ת֮ǰ��60��ƽ�����ӿ�����-1/0/1����ת���֮�󣬸�������������ڵ��ƽ�����ӽ��е���
	void _RotateRL(Node* pParent)
	{
		Node* pSubR = pParent->_pRight;
		Node* pSubRL = pSubR->_pLeft;

		// ��ת֮ǰ������pSubLR��ƽ�����ӣ���ת���֮����Ҫ���ݸ�ƽ�����������������ڵ��ƽ������
		int bf = pSubRL->_bf;

		// �Ƚ����ҵ���
		_RotateR(pParent->_pRight);

		// �ٽ�������
		_RotateL(pParent);
		if (1 == bf)
			pSubR->_bf = 0;
		else if (-1 == bf)
			pParent->_bf = 0;
	}

	int Height()
	{
		int height = _Height(_pRoot);
		return height;
	}

	int _Height(Node* pRoot)
	{
		if (nullptr == pRoot)
			return 0;
		// ����pRoot���������ĸ߶�
		int leftHeight = _Height(pRoot->_pLeft);
		int rightHeight = _Height(pRoot->_pRight);
		// �������������нϸߵ������߶�+1
		return (leftHeight > rightHeight)?(leftHeight + 1) : (rightHeight + 1);
	}

	bool IsBalanceTree()
	{
		return _IsBalanceTree(_pRoot);
	}

	bool _IsBalanceTree(Node* pRoot)
	{
		// ����Ҳ��AVL��
		if (nullptr == pRoot)
			return true;
		// ����pRoot�ڵ��ƽ�����ӣ���pRoot���������ĸ߶Ȳ�
		int leftHeight = _Height(pRoot->_pLeft);
		int rightHeight = _Height(pRoot->_pRight);
		int diff = rightHeight - leftHeight;
		// ����������ƽ��������pRoot��ƽ�����Ӳ���ȣ�����
		// pRootƽ�����ӵľ���ֵ����1����һ������AVL��
		if (diff != pRoot->_bf || (diff > 1 || diff < -1))
			return false;
		// pRoot��������������AVL���������һ����AVL��
		return _IsBalanceTree(pRoot->_pLeft) && _IsBalanceTree(pRoot->_pRight);
	}

private:
	Node * _pRoot;
};

void TestAVLTree()
{
	int arr[] = { 16, 3, 7, 11, 9, 26, 18, 14, 15 }; 
	AVLTree<int, int> avl;
	for (int i = 0; i < sizeof(arr) / sizeof(int);i++)
	{
		avl.Insert(arr[i],1);
	}
	cout << "height:"<<avl.Height() << endl;
	cout << "avl is balancetree:" << avl.IsBalanceTree() << endl;
}