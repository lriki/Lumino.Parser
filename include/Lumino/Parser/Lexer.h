
#pragma once

#include <Lumino/Base/RefBuffer.h>
#include "TokenList.h"

namespace Lumino
{
namespace Parser
{
// �����̎��
enum AlphaNumType
{
	AlphaNumType_MBC = 0,			///< �}���`�o�C�g�����ɔz�u�����R�[�h
	AlphaNumType_Control = 1,		///< ���䕶��
	AlphaNumType_OpChar = 2,		///< �I�y���[�V�����R�[�h
	AlphaNumType_Number = 3,		///< ����
	AlphaNumType_Alphabet = 4,		///< �ʏ핶��
	AlphaNumType_HexAlpha = 5,		///< 16�i�����Ƃ��Ă��g���镶��
};

class ErrorManager;

template<typename TChar>
class Lexer
{
public:
	typename typedef TokenList<TChar> TokenListT;


public:
	Lexer();
	virtual ~Lexer(){}

	void Analyze(RefBuffer* buffer, ErrorManager* errorManager);
	TokenListT& GetTokenList() { return m_tokenList; }


private:
	// 
	int		ReadToken(const TChar* buffer);

	// 
	int		ReadSpaceSequence(const TChar* buffer);				// ���p�X�y�[�X, TAB, �S�p�X�y�[�X�� (�󔒂̘A���͉�ŉ��)
	int		ReadNewLine(const TChar* buffer);
	int		ReadIdentifier(const TChar* buffer);
	int		ReadKeyword(const TChar* buffer);
	int		ReadNumericLiteral(const TChar* buffer);		// 10, 0x08, 1.5e-9, .10 ��
	int		ReadCharOrStringLiteral(const TChar* buffer);	// 'a', "aaa", L"aaa", @"aaa"
	int		ReadOperator(const TChar* buffer);
	int		ReadComment(const TChar* buffer);
	//int		ReadPrePro(const TChar* buffer);
	int		ReadEscNewLine(const TChar* buffer);			// �s�� \ (�s�R�����g�̉�͂ɉe�����邽�߁Alexer ���x���ŕK�v)
	int		ReadMBSSequence(const TChar* buffer);

	// 
	//int		ReadHexLiteralStart(const TChar* buffer);		// 0x (BASIC���Ȃ�Ă��Ɩ��͂��Ǝv������ &h �Ƃ� $ �Ƃ�)

protected:
	AlphaNumType GetAlphaNumType(TChar ch);
	const TChar* GetBufferEnd() const { return m_bufferEnd; }

	// �ȉ����I�[�o�[���C�h����ꍇ�A�I�[�m�F�ɂ� GetBufferEnd() ���g�p����
	virtual int CheckSpaceChar(const TChar* buffer) = 0;
	virtual int CheckCommentStart(const TChar* buffer) = 0;
	virtual int CheckCommentEnd(const TChar* buffer) = 0;
	virtual int CheckCommentLine(const TChar* buffer) = 0;
	virtual int CheckIdentStart(const TChar* buffer) = 0;
	virtual int CheckIdentLetter(const TChar* buffer) = 0;
	virtual int CheckKeyword(const TChar* buffer, int* langTokenType) = 0;
	virtual int CheckHexLiteralStart(const TChar* buffer) = 0;
	virtual int CheckIntegerSuffix(const TChar* buffer) = 0;
	virtual int CheckRealSuffix(const TChar* buffer) = 0;
	virtual int CheckExponentStart(const TChar* buffer) = 0;
	virtual int CheckStringStart(const TChar* buffer) = 0;
	virtual int CheckStringEnd(const TChar* buffer, const TChar* start) = 0;
	virtual int CheckStringEscape(const TChar* buffer, const TChar* start) = 0;
	virtual int CheckOperator(const TChar* buffer) = 0;
	virtual int CheckEscNewLine(const TChar* buffer) = 0;
	virtual bool CheckCaseSensitive() = 0;

	// �g�[�N�����m�肷�邽�тɌĂяo�����
	virtual void PollingToken(Token<TChar>& token) {}

private:
	TChar*			m_cursor;
	TChar*			m_bufferEnd;	// ���̓o�b�t�@�̏I�[ (�Ō�̕����̎�(EOF)���w��)
	TokenListT		m_tokenList;
	ErrorManager*	m_errorManager;
};

} // namespace Parser
} // namespace Lumino
