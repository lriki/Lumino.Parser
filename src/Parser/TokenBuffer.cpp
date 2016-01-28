
#include "Internal.h"
#include "TokenBuffer.h"

LN_NAMESPACE_BEGIN
namespace parser
{

//=============================================================================
// TokenBuffer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TokenBuffer::TokenBuffer()
{
	m_stringCache.Reserve(8192);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TokenBuffer::~TokenBuffer()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Token TokenBuffer::CreateToken(CommonTokenType commonType, const TokenChar* begin, const TokenChar* end, int langTokenType)
{
	int len = end - begin;
	LN_CHECK_ARGS(len > 0);

	Token t;
	t.m_commonType = commonType;
	t.m_langTokenType = langTokenType;
	t.m_locBegin = m_stringCache.Append(begin, len);
	t.m_locEnd = t.m_locBegin + len;
	return t;
}

} // namespace Parser
LN_NAMESPACE_END

