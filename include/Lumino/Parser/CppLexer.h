
#pragma once

#include "Lexer.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class CppLexer : public Lexer<TChar>
{
public:
	CppLexer() {}
	virtual ~CppLexer() {}

protected:

	// 以下をオーバーライドする場合、終端確認には GetBufferEnd() を使用する
	virtual int CheckSpaceChar(const char* buffer);
	virtual int CheckCommentStart(const char* buffer);
	virtual int CheckCommentEnd(const char* buffer);
	virtual int CheckCommentLine(const char* buffer);
	virtual int CheckIdentStart(const char* buffer);
	virtual int CheckIdentLetter(const char* buffer);
	virtual int CheckKeyword(const char* buffer);
	virtual int CheckHexLiteralStart(const char* buffer);
	virtual int CheckIntegerSuffix(const char* buffer);
	virtual int CheckRealSuffix(const char* buffer);
	virtual int CheckExponentStart(const char* buffer);
	virtual int CheckStringStart(const char* buffer);
	virtual int CheckStringEnd(const char* buffer, const char* start);
	virtual int CheckStringEscape(const char* buffer, const char* start);
	virtual int CheckOperator(const char* buffer);
	virtual int CheckEscNewLine(const char* buffer);
	virtual bool CheckCaseSensitive() { return true; }	// 大文字と小文字を区別する

};

} // namespace Parser
} // namespace Lumino
