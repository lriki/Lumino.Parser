
#pragma once
#include "../Common.h"
#include "../TokenList.h"

LN_NAMESPACE_BEGIN
namespace parser
{
	
/**
	@brief
*/
class Lexer
{
public:
	struct Range
	{
		const TokenChar* pos;
		const TokenChar* end;
	};

public:
	TokenListPtr Tokenize(const char* str, DiagnosticsItemSet* diag);	// TODO: ���ꂾ�Əo���オ���� Token ���w���Ă��镶���񂪉��Ă��܂�
	TokenListPtr Tokenize(const ByteBuffer& buffer, DiagnosticsItemSet* diag);

	virtual int ReadToken(const Range& buffer, TokenList* list) = 0;
	virtual void PollingToken(const Token& newToken);

	static AlphaNumTypeFlags GetAlphaNumType(TokenChar ch);

	static int ReadNewLine(const Range& buffer, Token* outToken);
	static int IsNewLine(const Range& buffer);

	static int ReadMBSSequence(const Range& buffer, Token* outToken);

protected:
	DiagnosticsItemSet*	m_diag;
	int		m_currentLineNumber;	// 0�`
	int		m_currentColumn;
};

} // namespace Parser
LN_NAMESPACE_END

