
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
	TokenListPtr Tokenize(const char* str, DiagnosticsItemSet* diag);
	TokenListPtr Tokenize(const ByteBuffer& buffer, DiagnosticsItemSet* diag);

	virtual int ReadToken(const Range& buffer, TokenList* list) = 0;

	static AlphaNumTypeFlags GetAlphaNumType(TokenChar ch);

	static int ReadNewLine(const Range& buffer, Token* outToken);
	static int IsNewLine(const Range& buffer);

	static int ReadMBSSequence(const Range& buffer, Token* outToken);

protected:
	DiagnosticsItemSet*	m_diag;
	int		m_currentLineNumber;	// 0Å`
	int		m_currentColumn;
};

} // namespace Parser
LN_NAMESPACE_END

