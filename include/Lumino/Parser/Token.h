
#pragma once

#include <Lumino/Base/Array.h>

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
	Token(TokenType type, const TChar* pBegin, const TChar* pEnd)
		: m_type(type)
		, m_begin(pBegin)
		, m_end(pEnd)
	{
	}

	Token(TokenType type, int lnagTokenType, const TChar* pBegin, const TChar* pEnd)
		: m_type(type)
		, m_langTokenType(lnagTokenType)
		, m_begin(pBegin)
		, m_end(pEnd)
	{
	}

public:
	TokenType	GetTokenType() const { return m_type; }
	const char*	GetTokenBegin() const { return m_begin; }
	const char*	GetTokenEnd() const { return m_end; }
	int			GetLength() const { return m_end - m_begin; }

	bool IsGenericSpace() const
	{
		return
			m_type == TokenType_SpaceSequence ||
			m_type == TokenType_NewLine ||
			m_type == TokenType_Comment;
	}

	bool IsGenericToken() const
	{
		return !IsGenericSpace();
	}

	bool IsEOF() const
	{
		return m_type == TokenType_EOF;
	}

private:
	TokenType		m_type;
	int				m_langTokenType;
	const TChar*	m_begin;
	const TChar*	m_end;
};

} // namespace Parser
} // namespace Lumino

