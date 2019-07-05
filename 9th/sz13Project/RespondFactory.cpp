#include "RespondFactory.h"


RespondFactory::RespondFactory()
{
}

RespondFactory::~RespondFactory()
{
}

//负责响应报文编解码对象创建
Codec * RespondFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	//创建解码对象
	if (NULL == arg)
	{
		codec = new RespondCodec;
	}
	else
	{
		codec = new RespondCodec(static_cast<RespondMsg*>(arg));
	}

	return codec;
}
