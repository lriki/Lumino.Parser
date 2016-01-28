
#pragma once
#include "../Common.h"
#include "../TokenList.h"
#include "../TokenBuffer.h"

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

		Range()
		{

		}

		Range(const TokenChar* begin, const TokenChar* end_)
		{
			pos = begin;
			end = end_;
		}
	};

public:
	ResultState Tokenize(UnitFile* file, DiagnosticsItemSet* diag);

	TokenListPtr Tokenize(const char* str, DiagnosticsItemSet* diag);	// TODO: これだと出来上がった Token が指している文字列が壊れてしまう
	TokenListPtr Tokenize(const ByteBuffer& buffer, DiagnosticsItemSet* diag);

	virtual int ReadToken(const Range& buffer, TokenList* list) = 0;
	virtual void PollingToken(const Token& newToken);

	static AlphaNumTypeFlags GetAlphaNumType(TokenChar ch);

	int ReadNewLine(const Range& buffer, Token* outToken);
	static int IsNewLine(const Range& buffer);

	int ReadMBSSequence(const Range& buffer, Token* outToken);

protected:
	RefPtr<TokenBuffer>	m_tokenBuffer;
	DiagnosticsItemSet*	m_diag;
	int		m_currentLineNumber;	// 0〜
	int		m_currentColumn;
};

} // namespace Parser
LN_NAMESPACE_END

