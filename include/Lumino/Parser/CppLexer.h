
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

	// �ȉ����I�[�o�[���C�h����ꍇ�A�I�[�m�F�ɂ� GetBufferEnd() ���g�p����
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
	virtual bool CheckCaseSensitive() { return true; }	// �啶���Ə���������ʂ���

};

} // namespace Parser
} // namespace Lumino
