
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
	//if (C# �̎��� UTF-8�S�p�X�y�[�X�Ƃ�)
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
	return 0;	// �R�����g�J�n�ł͂Ȃ�
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
	return 0;	// �R�����g�I�[�ł͂Ȃ�
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
	return 0;	// �s�R�����g�J�n�ł͂Ȃ�
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
	// C# �Ȃ� @" ������
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
		if (pStart[0] == '\'') {		// ' �J�n�̏ꍇ�A
			if (buffer[1] == '\'')		// \' �̓G�X�P�[�v�V�[�P���X
				return 2;
		}
		else if (pStart[0] == '"') {	// " �J�n�̏ꍇ�A
			if (buffer[1] == '"')		// \" �̓G�X�P�[�v�V�[�P���X
				return 2;
		}
	}
	// C# �Ȃ� pStart �� @" �����`�F�b�N�B���̏ꍇ�A�G�X�P�[�v�� "" �����ɂȂ�B
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
		// �㑱�������`�F�b�N
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
			if (buffer[0] == '>' && buffer[1] == '>')	return 2;	// >>	��C#�ł̓g�[�N�������ł͂Ȃ� http://dev.activebasic.com/egtra/2011/08/12/389/

			if (buffer[0] == '<' && buffer[1] == '<' && buffer[3] == '=')	return 2;	// <<
			if (buffer[0] == '>' && buffer[1] == '>' && buffer[3] == '=')	return 2;	// <<
		}

		// ���Ɍ㑱������̂͂Ȃ�
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
