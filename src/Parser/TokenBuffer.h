
#pragma once
#include "Token.h"
#include "CacheBuffer.h"

LN_NAMESPACE_BEGIN
namespace parser
{

/**
	
*/
class TokenBuffer
	: public RefObject
{
public:
	TokenBuffer();
	virtual ~TokenBuffer();

	Token CreateToken(CommonTokenType commonType, const TokenChar* begin, const TokenChar* end, int langTokenType = 0);

private:
	CacheBuffer<TokenChar>	m_stringCache;
};

} // namespace Parser
LN_NAMESPACE_END


