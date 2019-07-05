#include "SequenceASN1.h"

#include <iostream>

using namespace std;

//构造函数
SequenceASN1::SequenceASN1():BaseASN1()
{
}

//析构函数
SequenceASN1::~SequenceASN1()
{
}

//编码整型 int
int SequenceASN1::writeHeadNode(int iValue)
{
	ITCAST_INT ret = -1;

	//编码整型
	ret = DER_ItAsn1_WriteInteger(iValue, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed.." << endl;
		return -1;
	}


	//永远指向链表的最后一个节点
	m_next = m_header;

	return 0;
}

//编码字符串
int SequenceASN1::writeHeadNode(char * sValue, int len)
{
	ITCAST_INT ret = -1;

	//将char*转化为ANYBUF类型
	ret = DER_ITCAST_String_To_AnyBuf(&m_tmp, (unsigned char*)sValue, len);
	if (0 != ret)
	{
		cout << "DER_ITCAST_String_To_AnyBuf failed..." << endl;
		return -1;
	}

	//编码字符串
	ret = DER_ItAsn1_WritePrintableString(m_tmp, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WritePrintableString failed..." << endl;
		//释放临时节点内存
		DER_ITCAST_FreeQueue(m_tmp);
		m_tmp = NULL;

		return -1;
	}

	//m_next赋值
	m_next = m_header;

	//释放临时节点内存
	DER_ITCAST_FreeQueue(m_tmp);
	m_tmp = NULL;

	return 0;
}

//编码后继节点 
int SequenceASN1::writeNextNode(int iValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_WriteInteger(iValue, &m_next->next);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed.." << endl;
		return -1;
	}

	//指向下一个节点
	m_next = m_next->next;

	return 0;
}

//编码后继节点
int SequenceASN1::writeNextNode(char * sValue, int len)
{
	ITCAST_INT ret = -1;

	ret = EncodeChar(sValue, len, &m_next->next);
	if (0 != ret)
	{
		cout << "EncodeChar failed.." << endl;
		return -1;
	}

	m_next = m_next->next;

	return 0;
}

//解码
int SequenceASN1::readHeadNode(int & iValue)
{
	ITCAST_INT ret = -1;


	ret = DER_ItAsn1_ReadInteger(m_header, (ITCAST_UINT32*)&iValue);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed..." << endl;

		return -1;
	}

	m_next = m_header->next;

	return 0;
}

//解码
int SequenceASN1::readHeadNode(char * sValue)
{
	ITCAST_INT ret = -1;

	ret = DER_ItAsn1_ReadPrintableString(m_header, &m_tmp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed..." << endl;

		return -1;
	}

	//拷贝数据
	memcpy(sValue, m_tmp->pData, m_tmp->dataLen);

	//释放内存
	if (NULL != m_tmp)
	{
		DER_ITCAST_FreeQueue(m_tmp);
		m_tmp = NULL;
	}

	m_next = m_header->next;

	return 0;
}

int SequenceASN1::readNextNode(int & iValue)
{
	ITCAST_INT ret = -1;


	ret = DER_ItAsn1_ReadInteger(m_next, (ITCAST_UINT32*)&iValue);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed..." << endl;

		return -1;
	}

	m_next = m_next->next;

	return 0;
}

int SequenceASN1::readNextNode(char * sValue)
{
	ITCAST_INT ret = -1;

	ret = DER_ItAsn1_ReadPrintableString(m_next, &m_tmp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed..." << endl;

		return -1;
	}

	//拷贝数据
	memcpy(sValue, m_tmp->pData, m_tmp->dataLen);

	//释放内存
	if (NULL != m_tmp)
	{
		DER_ITCAST_FreeQueue(m_tmp);
		m_tmp = NULL;
	}

	m_next = m_next->next;

	return 0;
}

//编码结构体
int SequenceASN1::packSequence(char ** outData, int & outLen)
{
	ITCAST_INT ret = -1;

	ret = DER_ItAsn1_WriteSequence(m_header, &m_tmp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteSequence failed.." << endl;
		return -1;
	}

	*outData = new char[m_tmp->dataLen + 1];
	if (NULL == *outData)
	{
		cout << "new outData failed..." << endl;
		return -1;
	}
	
	//拷贝数据
	memcpy(*outData, m_tmp->pData, m_tmp->dataLen);
	outLen = m_tmp->dataLen;

	//释放内存
	if (NULL != m_tmp)
	{
		DER_ITCAST_FreeQueue(m_tmp);
	}

	return 0;
}

//解码结构体
int SequenceASN1::unpackSequence(char * inData, int inLen)
{
	ITCAST_INT ret = -1;

	//将char*转为ANYBUF类型
	ret = DER_ITCAST_String_To_AnyBuf(&m_tmp, (unsigned char *)inData, inLen);
	if (0 != ret)
	{
		cout << "DER_ITCAST_String_To_AnyBuf failed.." << endl;
		return -1;
	}

	ret = DER_ItAsn1_ReadSequence(m_tmp, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadSequence failed.." << endl;
		return -1;
	}

	//释放临时节点内存
	if (NULL != m_tmp)
	{
		DER_ITCAST_FreeQueue(m_tmp);
	}

	return 0;
}

//释放结构体或者链表 如果node为NULL 就释放链表 否则释放节点
void SequenceASN1::freeSequence(ITCAST_ANYBUF * node)
{
	if (NULL == node)
	{
		DER_ITCAST_FreeQueue(m_header);
	}
	else
	{
		DER_ITCAST_FreeQueue(node);
	}
}


