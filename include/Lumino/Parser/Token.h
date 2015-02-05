
#pragma once

#include <Lumino/Base/Array.h>
#include <Lumino/Base/String.h>
#include <Lumino/Base/StringUtils.h>

namespace Lumino
{
namespace Parser
{

// 基本的なトークン種別 (言語間共通)
enum TokenType
{
	TokenType_Unknown = 0,
	TokenType_SpaceSequence,
	TokenType_NewLine,
	TokenType_Identifier,
	TokenType_Keyword,
	TokenType_NumericLiteral,
	TokenType_CharOrStringLiteral,
	TokenType_Operator,
	TokenType_Comment,				///< 行末\ がある場合は含める
	TokenType_EscNewLine,			///< 行末 \ (TokenType_NewLine とは区別する。"\n" = NewLine, "\\\n" = EscNewLine)
	TokenType_MBSSequence,			///< マルチバイト文字並び
	TokenType_EOF,					///< EOF
};

template<typename TChar>
class Token
{
public:
	typename typedef BasicString<TChar> StringT;

public:
	Token(TokenType type, const TChar* pBegin, const TChar* pEnd)
	{
		Init();
		m_type = type;
		m_begin = pBegin;
		m_end = pEnd;
	}

	Token(TokenType type, int lnagTokenType, const TChar* pBegin, const TChar* pEnd)
		: m_type(type)
		, m_langTokenType(lnagTokenType)
		, m_begin(pBegin)
		, m_end(pEnd)
	{
		Init();
		m_type = type;
		m_langTokenType = lnagTokenType;
		m_begin = pBegin;
		m_end = pEnd;
	}

	void Init()
	{
		m_type = TokenType_Unknown;
		m_langTokenType = 0;
		m_begin = NULL;
		m_end = NULL;
		m_beginStringValue = NULL;
		m_endStringValue = NULL;
	}


public:
	TokenType		GetTokenType() const { return m_type; }
	const TChar*	GetTokenBegin() const { return m_begin; }
	const TChar*	GetTokenEnd() const { return m_end; }
	int				GetLength() const { return m_end - m_begin; }

	// ※ちょっと問題あり。プリプロセッサでは \n は空白扱いしてはダメ
	bool IsGenericSpace() const
	{
		return (
			m_type == TokenType_SpaceSequence ||
			m_type == TokenType_Comment ||
			m_type == TokenType_EscNewLine);
		/* TokenType_EscNewLine も空白である。
		* # \
		* include "hoge.h"
		* はコンパイル可能。
		*/
	}

	bool IsGenericToken() const
	{
		return !IsGenericSpace();
	}

	bool IsEOF() const
	{
		return m_type == TokenType_EOF;
	}

	bool IsLineEnd() const
	{
		return (
			m_type == TokenType_NewLine ||
			m_type == TokenType_EOF);
	}

	// 1文字に一致するか。トークン自体の長さも1文字である。
	bool EqualChar(TChar ch) const
	{
		if (m_begin[0] != ch) return false;
		if (GetLength() != 1) return false;
		return true;
	}

	// 文字列が一致するか (strncmp のような部分一致ではない。長さが違えばその時点で false)
	bool EqualString(const TChar* str, int len) const
	{
		if (GetLength() != len) return false;
		return StringUtils::StrNCmp(m_begin, str, len) == 0;
	}

	void SetStringValue(const TChar* beginStringValue, const TChar* endStringValue)
	{
		m_beginStringValue = beginStringValue;
		m_endStringValue = endStringValue;
	}

	StringT GetStringValue() const { return StringT(m_beginStringValue, m_endStringValue - m_beginStringValue); }

private:
	TokenType		m_type;
	int				m_langTokenType;
	const TChar*	m_begin;
	const TChar*	m_end;
	const TChar*	m_beginStringValue;
	const TChar*	m_endStringValue;
};

} // namespace Parser
} // namespace Lumino

