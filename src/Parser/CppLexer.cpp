
#include <Lumino/Base/String.h>
#include <Lumino/Base/StringUtils.h>
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
CppLexer<TChar>::CppLexer()
	: m_seqPreProInclude(PreProIncludeSeq_LineHead)
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckSpaceChar(const TChar* buffer)
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
int CppLexer<TChar>::CheckCommentStart(const TChar* buffer)
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
int CppLexer<TChar>::CheckCommentEnd(const TChar* buffer)
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
int CppLexer<TChar>::CheckCommentLine(const TChar* buffer)
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
int CppLexer<TChar>::CheckIdentStart(const TChar* buffer)
{
	return isalpha(*buffer) || *buffer == '_';
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckIdentLetter(const TChar* buffer)
{
	return isalnum(*buffer) || *buffer == '_';
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckKeyword(const TChar* buffer, int* langTokenType)
{
	static const WordData wordList[] =
	{
		{ LN_T(TChar, "asm"),					3,	TT_CppKW_asm },
		{ LN_T(TChar, "auto"),					4,	TT_CppKW_auto },
		{ LN_T(TChar, "bool"),					4,	TT_CppKW_bool },
		{ LN_T(TChar, "break"),					5,	TT_CppKW_break },
		{ LN_T(TChar, "case"),					4,	TT_CppKW_case },
		{ LN_T(TChar, "catch"),					5,	TT_CppKW_catch },
		{ LN_T(TChar, "char"),					4,	TT_CppKW_char },
		{ LN_T(TChar, "class"),					5,	TT_CppKW_class },
		{ LN_T(TChar, "const"),					5,	TT_CppKW_const },
		{ LN_T(TChar, "const_cast"),			10,	TT_CppKW_const_cast },
		{ LN_T(TChar, "continue"),				8,	TT_CppKW_continue },
		{ LN_T(TChar, "default"),				7,	TT_CppKW_default },
		{ LN_T(TChar, "delete"),				6,	TT_CppKW_delete },
		{ LN_T(TChar, "do"),					2,	TT_CppKW_do },
		{ LN_T(TChar, "double"),				6,	TT_CppKW_double },
		{ LN_T(TChar, "dynamic_cast"),			12,	TT_CppKW_dynamic_cast },
		{ LN_T(TChar, "else"),					4,	TT_CppKW_else },
		{ LN_T(TChar, "enum"),					4,	TT_CppKW_enum },
		{ LN_T(TChar, "explicit"),				8,	TT_CppKW_explicit },
		{ LN_T(TChar, "export"),				6,	TT_CppKW_export },
		{ LN_T(TChar, "extern"),				6,	TT_CppKW_extern },
		{ LN_T(TChar, "false"),					5,	TT_CppKW_false },
		{ LN_T(TChar, "float"),					5,	TT_CppKW_float },
		{ LN_T(TChar, "for"),					3,	TT_CppKW_for },
		{ LN_T(TChar, "friend"),				6,	TT_CppKW_friend },
		{ LN_T(TChar, "goto"),					4,	TT_CppKW_goto },
		{ LN_T(TChar, "if"),					2,	TT_CppKW_if },
		{ LN_T(TChar, "inline"),				6,	TT_CppKW_inline },
		{ LN_T(TChar, "int"),					3,	TT_CppKW_int },
		{ LN_T(TChar, "long"),					4,	TT_CppKW_long },
		{ LN_T(TChar, "mutable"),				7,	TT_CppKW_mutable },
		{ LN_T(TChar, "namespace"),				9,	TT_CppKW_namespace },
		{ LN_T(TChar, "new"),					3,	TT_CppKW_new },
		{ LN_T(TChar, "operator"),				8,	TT_CppKW_operator },
		{ LN_T(TChar, "private"),				7,	TT_CppKW_private },
		{ LN_T(TChar, "protected"),				9,	TT_CppKW_protected },
		{ LN_T(TChar, "public"),				6,	TT_CppKW_public },
		{ LN_T(TChar, "register"),				8,	TT_CppKW_register },
		{ LN_T(TChar, "reinterpret_cast"),		16,	TT_CppKW_reinterpret_cast },
		{ LN_T(TChar, "return"),				6,	TT_CppKW_return },
		{ LN_T(TChar, "short"),					5,	TT_CppKW_short },
		{ LN_T(TChar, "signed"),				6,	TT_CppKW_signed },
		{ LN_T(TChar, "sizeof"),				6,	TT_CppKW_sizeof },
		{ LN_T(TChar, "static"),				6,	TT_CppKW_static },
		{ LN_T(TChar, "static_cast"),			11,	TT_CppKW_static_cast },
		{ LN_T(TChar, "struct"),				6,	TT_CppKW_struct },
		{ LN_T(TChar, "switch"),				6,	TT_CppKW_switch },
		{ LN_T(TChar, "template"),				8,	TT_CppKW_template },
		{ LN_T(TChar, "this"),					4,	TT_CppKW_this },
		{ LN_T(TChar, "throw"),					5,	TT_CppKW_throw },
		{ LN_T(TChar, "true"),					4,	TT_CppKW_true },
		{ LN_T(TChar, "try"),					3,	TT_CppKW_try },
		{ LN_T(TChar, "typedef"),				7,	TT_CppKW_typedef },
		{ LN_T(TChar, "typeid"),				6,	TT_CppKW_typeid },
		{ LN_T(TChar, "typename"),				8,	TT_CppKW_typename },
		{ LN_T(TChar, "union"),					5,	TT_CppKW_union },
		{ LN_T(TChar, "unsigned"),				8,	TT_CppKW_unsigned },
		{ LN_T(TChar, "using"),					5,	TT_CppKW_using },
		{ LN_T(TChar, "virtual"),				7,	TT_CppKW_virtual },
		{ LN_T(TChar, "void"),					4,	TT_CppKW_void },
		{ LN_T(TChar, "volatile"),				8,	TT_CppKW_volatile },
		{ LN_T(TChar, "wchar_t"),				7,	TT_CppKW_wchar_t },
		{ LN_T(TChar, "while"),					5,	TT_CppKW_while },
	};

	const int count = LN_ARRAY_SIZE_OF(wordList);
	for (int i = 0; i < count; ++i) 
	{
		if (wordList[i].Word[0] == buffer[0] &&		// まずは先頭文字を調べて
			StringUtils::StrNCmp(wordList[i].Word, buffer, wordList[i].Length) == 0)	// 先頭が一致したら残りを調べる
		{
			*langTokenType = (int)wordList[i].Type;
			return wordList[i].Length;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckHexLiteralStart(const TChar* buffer)
{
	if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X'))
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckIntegerSuffix(const TChar* buffer)
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
int CppLexer<TChar>::CheckRealSuffix(const TChar* buffer)
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
int CppLexer<TChar>::CheckExponentStart(const TChar* buffer)
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
int CppLexer<TChar>::CheckStringStart(const TChar* buffer)
{
	if (buffer[0] == '\'' || buffer[0] == '"') {
		return 1;		// 普通の文字列リテラルの開始
	}
	if (buffer[0] == 'L') {
		if (buffer[1] == '\'' || buffer[1] == '"') {
			return 2;	// L プレフィックス付き文字列リテラル
		}
	}
	// include ディレクティブ内であれば <> も文字列扱いする。
	if (m_seqPreProInclude == PreProIncludeSeq_FoundInclude && buffer[0] == '<') {
		return 1;

	}
	return 0;
	// C# なら @" もある
	// http://hydrocul.github.io/wiki/programming_languages_diff/string/escape.html
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckStringEnd(const TChar* buffer, const TChar* start, bool* outIsChar)
{
	*outIsChar = false;

	// L プレフィックス付き文字列リテラル
	if (start[0] == 'L') {
		if (buffer[0] == start[1]) {	// クォーテーションが L の次のクォーテーションと等しい
			if (buffer[0] == '\'') { *outIsChar = true; }	// ' は文字リテラル
			return 1;
		}
	}
	// 普通の文字列リテラルの開始
	if (buffer[0] == start[0]) {
		if (buffer[0] == '\'') { *outIsChar = true; }		// ' は文字リテラル
		return 1;
	}
	// include ディレクティブ内であれば <> も文字列扱いする。
	if (m_seqPreProInclude == PreProIncludeSeq_FoundInclude && buffer[0] == '>') {
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckStringEscape(const TChar* buffer, const TChar* pStart)
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
int CppLexer<TChar>::CheckOperator(const TChar* buffer, int* langTokenType)
{
	if (GetAlphaNumType(buffer[0]) == AlphaNumType_OpChar)
	{
		// ++ と + のように、1文字目が同じものは文字数の多い方が先に検索されるようにする
		static const WordData wordList[] =
		{
			{ LN_T(TChar, "##"),	2,	TT_CppOP_SharpSharp },
			{ LN_T(TChar, "#"),		1,	TT_CppOP_Sharp },
			{ LN_T(TChar, "->*"),	3,	TT_CppOP_ArrowAsterisk },
			{ LN_T(TChar, "->"),	2,	TT_CppOP_Arrow },
			{ LN_T(TChar, ","),		1,	TT_CppOP_Comma },
			{ LN_T(TChar, "++"),	2,	TT_CppOP_Increment },
			{ LN_T(TChar, "--"),	2,	TT_CppOP_Decrement },
			{ LN_T(TChar, "&&"),	2,	TT_CppOP_LogicalAnd },
			{ LN_T(TChar, "||"),	2,	TT_CppOP_LogicalOr },
			{ LN_T(TChar, "<="),	2,	TT_CppOP_LessThenEqual },
			{ LN_T(TChar, ">="),	2,	TT_CppOP_GreaterThenEqual },
			{ LN_T(TChar, "=="),	2,	TT_CppOP_CmpEqual },
			{ LN_T(TChar, "<<="),	3,	TT_CppOP_LeftShiftEqual },
			{ LN_T(TChar, ">>="),	3,	TT_CppOP_RightShiftEqual },
			{ LN_T(TChar, "+="),	2,	TT_CppOP_PlusEqual },
			{ LN_T(TChar, "-="),	2,	TT_CppOP_MinusEqual },
			{ LN_T(TChar, "*="),	2,	TT_CppOP_MulEqual },
			{ LN_T(TChar, "/="),	2,	TT_CppOP_DivEqual },
			{ LN_T(TChar, "%="),	2,	TT_CppOP_ModEqual },
			{ LN_T(TChar, "&="),	2,	TT_CppOP_AndEqual },
			{ LN_T(TChar, "|="),	2,	TT_CppOP_OrEqual },
			{ LN_T(TChar, "!="),	2,	TT_CppOP_NotEqual },
			{ LN_T(TChar, "="),		1,	TT_CppOP_Equal },
			{ LN_T(TChar, "<<"),	2,	TT_CppOP_LeftShift },
			{ LN_T(TChar, ">>"),	2,	TT_CppOP_RightShift },
			{ LN_T(TChar, "+"),		1,	TT_CppOP_Plus },
			{ LN_T(TChar, "-"),		1,	TT_CppOP_Minul },
			{ LN_T(TChar, "*"),		1,	TT_CppOP_Asterisk },
			{ LN_T(TChar, "/"),		1,	TT_CppOP_Slash },
			{ LN_T(TChar, "%"),		1,	TT_CppOP_Parseint },
			{ LN_T(TChar, "&"),		1,	TT_CppOP_Ampersand },
			{ LN_T(TChar, "|"),		1,	TT_CppOP_Pipe },
			{ LN_T(TChar, "~"),		1,	TT_CppOP_Tilde },
			{ LN_T(TChar, "^"),		1,	TT_CppOP_Caret },
			{ LN_T(TChar, "!"),		1,	TT_CppOP_Exclamation },
			{ LN_T(TChar, "..."),	3,	TT_CppOP_Ellipsis },
			{ LN_T(TChar, ".*"),	2,	TT_CppOP_DotAsterisk },
			{ LN_T(TChar, "."),		1,	TT_CppOP_Dot },
			{ LN_T(TChar, "::"),	2,	TT_CppOP_DoubleColon },
			{ LN_T(TChar, "?"),		1,	TT_CppOP_Question },
			{ LN_T(TChar, ":"),		1,	TT_CppOP_Colon },
			{ LN_T(TChar, ";"),		1,	TT_CppOP_Semicolon },
			{ LN_T(TChar, "{"),		1,	TT_CppOP_LeftBrace },
			{ LN_T(TChar, "}"),		1,	TT_CppOP_RightBrace },
			{ LN_T(TChar, "["),		1,	TT_CppOP_LeftBracket },
			{ LN_T(TChar, "]"),		1,	TT_CppOP_RightBracket },
			{ LN_T(TChar, "("),		1,	TT_CppOP_LeftParen },
			{ LN_T(TChar, ")"),		1,	TT_CppOP_RightParen },
			{ LN_T(TChar, "<"),		1,	TT_CppOP_LeftAngle },
			{ LN_T(TChar, ">"),		1,	TT_CppOP_RightAngle },
		};

		const int count = LN_ARRAY_SIZE_OF(wordList);
		for (int i = 0; i < count; ++i)
		{
			if (wordList[i].Word[0] == buffer[0] &&		// まずは先頭文字を調べて
				StringUtils::StrNCmp(wordList[i].Word, buffer, wordList[i].Length) == 0)	// 先頭が一致したら残りを調べる
			{
				*langTokenType = (int)wordList[i].Type;
				return wordList[i].Length;
			}
		}
		return 0;
	//	// 後続文字をチェック
	//	{
	//		//if (buffer[0] == '?' && buffer[1] == '?')	// ?? (C#)
	//		//	return 2;
	//		if (buffer[0] == '+' && buffer[1] == '+')	// ++
	//			return 2;
	//		if (buffer[0] == '-' && buffer[1] == '-')	// --
	//			return 2;
	//		if (buffer[0] == '&' && buffer[1] == '&')	// &&
	//			return 2;
	//		if (buffer[0] == '|' && buffer[1] == '|')	// ||
	//			return 2;
	//		if (buffer[0] == '-' && buffer[1] == '>')	// ->
	//			return 2;
	//		if (buffer[1] == '=')
	//		{
	//			if (buffer[0] == '=') return 2;		// ==
	//			if (buffer[0] == '!') return 2;		// !=
	//			if (buffer[0] == '<') return 2;		// <=
	//			if (buffer[0] == '>') return 2;		// >=
	//			if (buffer[0] == '+') return 2;		// +=
	//			if (buffer[0] == '-') return 2;		// -=
	//			if (buffer[0] == '*') return 2;		// *=
	//			if (buffer[0] == '/') return 2;		// /=
	//			if (buffer[0] == '%') return 2;		// %=
	//			if (buffer[0] == '&') return 2;		// &=
	//			if (buffer[0] == '|') return 2;		// |=
	//			if (buffer[0] == '^') return 2;		// ^=

	//		}
	//		if (buffer[0] == '<' && buffer[1] == '<')	return 2;	// <<
	//		if (buffer[0] == '>' && buffer[1] == '>')	return 2;	// >>	※C#ではトークン扱いではない http://dev.activebasic.com/egtra/2011/08/12/389/

	//		if (buffer[0] == '<' && buffer[1] == '<' && buffer[3] == '=')	return 2;	// <<
	//		if (buffer[0] == '>' && buffer[1] == '>' && buffer[3] == '=')	return 2;	// <<
	//	}

	//	// 特に後続するものはない
	//	return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int CppLexer<TChar>::CheckEscNewLine(const TChar* buffer)
{
	if (buffer[0] == '\\' && buffer[1] == '\n')
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void CppLexer<TChar>::PollingToken(Token<TChar>& token)
{
	if (token.IsGenericSpace()) {
		// ホワイトスペースやコメントである。なにもしない
		return;
	}

	// 何もしていない。改行を探す。
	if (m_seqPreProInclude == PreProIncludeSeq_Idle)
	{
		if (token.GetTokenType() == TokenType_NewLine) {
			m_seqPreProInclude = PreProIncludeSeq_LineHead;		// 改行が見つかった。行頭状態へ
		}
	}
	// 行頭にいる。# を探す。
	else if (m_seqPreProInclude == PreProIncludeSeq_LineHead)
	{
		if (token.GetTokenType() == TokenType_Operator && *token.GetTokenBegin() == '#') {
			m_seqPreProInclude = PreProIncludeSeq_FoundSharp;	// "#" を見つけた
		}
		else {
			m_seqPreProInclude = PreProIncludeSeq_Idle;		// "#" 以外のトークンだった。Idle へ。
		}
	}
	// # まで見つけている。次の "include" を探す。
	else if (m_seqPreProInclude == PreProIncludeSeq_FoundSharp)
	{
		if (token.EqualString(LN_T(TChar, "include"), 7)) {
			m_seqPreProInclude = PreProIncludeSeq_FoundInclude;	// "include" を見つけた
		}
		else {
			m_seqPreProInclude = PreProIncludeSeq_Idle;		// #" 以外のトークンだった。"include" 以外のプリプロディレクティブ。
		}
	}
	// include ～ 行末
	else if (m_seqPreProInclude == PreProIncludeSeq_FoundInclude)
	{
		if (token.GetTokenType() == TokenType_NewLine) {
			m_seqPreProInclude = PreProIncludeSeq_LineHead;		// 改行が見つかった。行頭状態へ
		}
	}
}

// テンプレートのインスタンス化
template class CppLexer<char>;
template class CppLexer<wchar_t>;

} // namespace Parser
} // namespace Lumino
