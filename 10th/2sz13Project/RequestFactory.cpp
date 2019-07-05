#include "RequestFactory.h"


RequestFactory::RequestFactory()
{
}

RequestFactory::~RequestFactory()
{
}

//创建对象方法 编码对象 解码对象
Codec * RequestFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	//创建解码对象
	if (NULL == arg)
	{
		codec = new RequestCodec;
	}
	else
	{
		codec = new RequestCodec(static_cast<RequestMsg*>(arg));
	}

	return codec;
}
