
#include "../Internal.h"
#include "Lexer.h"

LN_NAMESPACE_BEGIN
namespace parser
{
	
static int g_alphaNumTypeTable[256] =
{
	0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
	0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
	0x0001, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
	0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
	0x0002, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
	0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
	0x0002, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
	0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

//=============================================================================
// Lexer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TokenListPtr Lexer::Tokenize(const char* str, DiagnosticsItemSet* diag)
{
	ByteBuffer buf(str, strlen(str));
	return Tokenize(buf, diag);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TokenListPtr Lexer::Tokenize(const ByteBuffer& buffer, DiagnosticsItemSet* diag)
{
	m_diag = diag;
	m_currentLineNumber = 0;
	m_currentColumn = 0;

	TokenListPtr list(LN_NEW TokenList(), false);

	// 最悪のパターンで容量確保
	list->Reserve(buffer.GetSize());

	Range r;
	r.pos = (const TokenChar*)buffer.GetConstData();
	r.end = r.pos + (buffer.GetSize() / sizeof(TokenChar));
	while (r.pos < r.end)
	{
		int len = ReadToken(r, list);
		if (len == 0) {
			break;
		}
		r.pos += len;
		m_currentColumn += len;

		// 改行が見つかったら行番号を増やす
		if (list->GetAt(list->GetCount() - 1).GetCommonType() == CommonTokenType::NewLine)
		{
			++m_currentLineNumber;
			m_currentColumn = 0;
		}
	}

	// 最後に EOF を入れておく
	list->Add(Token(CommonTokenType::Eof, nullptr, nullptr));

	return list;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
AlphaNumTypeFlags Lexer::GetAlphaNumType(TokenChar ch)
{
	if (ch > 255) {
		return AlphaNumTypeFlags::MBC;	// wchar_t 型の多バイトコード
	}
	return (AlphaNumTypeFlags::enum_type)g_alphaNumTypeTable[ch];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Lexer::ReadNewLine(const Range& buffer, Token* outToken)
{
	int len = IsNewLine(buffer);
	if (len > 0) {
		*outToken = Token(CommonTokenType::NewLine, buffer.pos, buffer.pos + len);
		return len;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Lexer::IsNewLine(const Range& buffer)
{
	if (buffer.pos[0] == '\r' &&
		buffer.pos[1] == '\n')
	{
		return 2;
	}
	if (buffer.pos[0] == '\r' ||
		buffer.pos[0] == '\n')
	{
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Lexer::ReadMBSSequence(const Range& buffer, Token* outToken)
{
	Range r = buffer;
	while (r.pos < r.end)
	{
		int extraCount = 0;
		UnicodeUtils::CheckUTF8TrailingBytes((const UnicodeUtils::UTF8*)r.pos, (const UnicodeUtils::UTF8*)r.end, false, &extraCount);

		// 追加バイトが無い = シングルバイト文字だった場合は終了
		if (extraCount == 0) {
			break;
		}

		// 今回調べた文字と、追加文字分進める
		r.pos += (1 + extraCount);
	};

	int len = r.pos - buffer.pos;
	if (len > 0) {
		*outToken = Token(CommonTokenType::MbsSequence, buffer.pos, r.pos);
	}
	return len;
}

} // namespace Parser
LN_NAMESPACE_END

