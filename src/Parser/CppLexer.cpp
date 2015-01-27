
#include "../../include/Lumino/Parser/CppLexer.h"

namespace Lumino
{
namespace Parser
{

//=============================================================================
// CppLexer
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckSpaceChar(const char* buffer)
{
	if (buffer[0] == ' ' ||
		buffer[0] == '\t' ||
		buffer[0] == '\f' ||
		buffer[0] == '\v')
	{
		return 1;
	}

	//if (isspace(*buffer))
	//	return 1;
	//if (C# の時は UTF-8全角スペースとか)
	//	return 3;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckCommentStart(const char* buffer)
{
	if (buffer[0] == '/' &&
		buffer[1] == '*')
		return 2;
	return 0;	// コメント開始ではない
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckCommentEnd(const char* buffer)
{
	if (buffer[0] == '*' &&
		buffer[1] == '/')
		return 2;
	return 0;	// コメント終端ではない
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckCommentLine(const char* buffer)
{
	if (buffer[0] == '/' &&
		buffer[1] == '/')
		return 2;
	return 0;	// 行コメント開始ではない
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckIdentStart(const char* buffer)
{
	return isalpha(*buffer) || *buffer == '_';
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckIdentLetter(const char* buffer)
{
	return isalnum(*buffer) || *buffer == '_';
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckKeyword(const char* buffer)
{
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckHexLiteralStart(const char* buffer)
{
	if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X'))
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckIntegerSuffix(const char* buffer)
{
	if (buffer[0] == 'u' ||
		buffer[0] == 'U' ||
		buffer[0] == 'l' ||
		buffer[0] == 'L')
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckRealSuffix(const char* buffer)
{
	if (buffer[0] == 'f' ||
		buffer[0] == 'F' ||
		buffer[0] == 'd' ||
		buffer[0] == 'F')
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckExponentStart(const char* buffer)
{
	if (buffer[0] == 'e' ||
		buffer[0] == 'E')
	{
		if (buffer[1] == '+' ||
			buffer[1] == '-') {
			return 2;
		}
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckStringStart(const char* buffer)
{
	if (buffer[0] == '\'' ||
		buffer[0] == '"')
		return 1;
	return 0;
	// C# なら @" もある
	// http://hydrocul.github.io/wiki/programming_languages_diff/string/escape.html
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckStringEnd(const char* buffer, const char* pStart)
{
	if (buffer[0] == pStart[0])
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckStringEscape(const char* buffer, const char* pStart)
{
	if (buffer[0] == '\\') {
		if (pStart[0] == '\'') {		// ' 開始の場合、
			if (buffer[1] == '\'')		// \' はエスケープシーケンス
				return 2;
		}
		else if (pStart[0] == '"') {	// " 開始の場合、
			if (buffer[1] == '"')		// \" はエスケープシーケンス
				return 2;
		}
	}
	// C# なら pStart が @" かもチェック。その場合、エスケープは "" だけになる。
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckOperator(const char* buffer)
{
	if (m_aryAlphaNumericTypeTable[buffer[0]] == LNAIAlphaNumericFlags_OpChar)
	{
		// 後続文字をチェック
		{
			//if (buffer[0] == '?' && buffer[1] == '?')	// ?? (C#)
			//	return 2;
			if (buffer[0] == '+' && buffer[1] == '+')	// ++
			return 2;
			if (buffer[0] == '-' && buffer[1] == '-')	// --
				return 2;
			if (buffer[0] == '&' && buffer[1] == '&')	// &&
				return 2;
			if (buffer[0] == '|' && buffer[1] == '|')	// ||
				return 2;
			if (buffer[0] == '-' && buffer[1] == '>')	// ->
				return 2;
			if (buffer[1] == '=')
			{
				if (buffer[0] == '=') return 2;		// ==
				if (buffer[0] == '!') return 2;		// !=
				if (buffer[0] == '<') return 2;		// <=
				if (buffer[0] == '>') return 2;		// >=
				if (buffer[0] == '+') return 2;		// +=
				if (buffer[0] == '-') return 2;		// -=
				if (buffer[0] == '*') return 2;		// *=
				if (buffer[0] == '/') return 2;		// /=
				if (buffer[0] == '%') return 2;		// %=
				if (buffer[0] == '&') return 2;		// &=
				if (buffer[0] == '|') return 2;		// |=
				if (buffer[0] == '^') return 2;		// ^=

			}
			if (buffer[0] == '<' && buffer[1] == '<')	return 2;	// <<
			if (buffer[0] == '>' && buffer[1] == '>')	return 2;	// >>	※C#ではトークン扱いではない http://dev.activebasic.com/egtra/2011/08/12/389/

			if (buffer[0] == '<' && buffer[1] == '<' && buffer[3] == '=')	return 2;	// <<
			if (buffer[0] == '>' && buffer[1] == '>' && buffer[3] == '=')	return 2;	// <<
		}

		// 特に後続するものはない
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckEscNewLine(const char* buffer)
{
	if (buffer[0] == '\\' && buffer[1] == '\n')
		return 2;
	return 0;
}

} // namespace Parser
} // namespace Lumino
