
#include <Lumino/Text/UnicodeUtils.h>
#include "../../include/Lumino/Parser/Lexer.h"
#include "../../include/Lumino/Parser/ErrorInfo.h"

namespace Lumino
{
namespace Parser
{

static int g_alphaNumTypeTable[256] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
	2, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2,
	2, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

//=============================================================================
// Lexer
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Lexer<TChar>::Lexer()
: m_cursor(NULL)
, m_bufferEnd(NULL)
, m_errorManager(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void Lexer<TChar>::Analyze(RefBuffer* buffer, ErrorManager* errorManager)
{
	m_errorManager = errorManager;
	m_tokenList.Attach(LN_NEW TokenListT());

	// �ň��̃p�^�[���ŗe�ʊm��
	m_tokenList->Reserve(buffer->GetSize());

	m_cursor = (TChar*)buffer->GetPointer();
	m_bufferEnd = m_cursor + buffer->GetSize();

	while (m_cursor < m_bufferEnd)
	{
		int nLen = ReadToken(m_cursor);
		if (nLen == 0) {
			// �G���[�͌Ăяo����ŏ�����
			break;
		}

		// �V�����ǂݎ��ꂽ�g�[�N����ʒm����
		PollingToken(m_tokenList->GetLast());
		m_cursor += nLen;
	}

	// �Ō�� EOF �����Ă���
	m_tokenList->Add(Token<TChar>(TokenType_EOF, m_bufferEnd, m_bufferEnd + 1));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadToken(const TChar* buffer)
{
	int count;

	// �󔒕���
	count = ReadSpaceSequence(buffer);
	if (count > 0) { return count; }
	// ���s
	count = ReadNewLine(buffer);
	if (count > 0) { return count; }
	// �\���
	count = ReadKeyword(buffer);
	if (count > 0) { return count; }
	// ���ʎq
	count = ReadIdentifier(buffer);
	if (count > 0) { return count; }
	// ���l���e����
	count = ReadNumericLiteral(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_NumericLiteral, buffer, buffer + count));
		return count;
	}
	// �����񃊃e����
	count = ReadCharOrStringLiteral(buffer);
	if (count > 0) {
		return count;
	}
	// �R�����g (���Z�q�����D�悷��)
	count = ReadComment(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_Comment, buffer, buffer + count));
		return count;
	}
	// ���Z�q
	count = ReadOperator(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_Operator, buffer, buffer + count));
		return count;
	}
	// �v���v���Z�b�T�f�B���N�e�B�u
	//count = ReadPrePro(buffer);
	//if (count > 0) {
	//	m_tokenList.push_back(Token<TChar>(TokenType_PrePro, buffer, buffer + count));
	//	return count;
	//}
	// �s�� \ 
	count = ReadEscNewLine(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_EscNewLine, buffer, buffer + count));
		return count;
	}
	// �}���`�o�C�g��������
	count = ReadMBSSequence(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_MBSSequence, buffer, buffer + count));
		return count;
	}
	// �s���ȃg�[�N������������
	m_errorManager->AddError(ErrorCode_Error_UnknownToken);
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadSpaceSequence(const TChar* buffer)
{
	// �A������X�y�[�X�����̐���Ԃ�
	// (�S�p�X�y�[�X�����e����ꍇ�͂���S�̂̕��������J�E���g)
	const TChar* pos = buffer;
	while (pos < m_bufferEnd)
	{
		int count = CheckSpaceChar(pos);
		if (count == 0) {
			break;
		}
		pos += count;
	}

	// �g�[�N���쐬
	if (buffer < pos) {
		m_tokenList->Add(Token<TChar>(TokenType_SpaceSequence, buffer, pos));
	}
	return pos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadNewLine(const TChar* buffer)
{
	int count = 0;
	if (buffer[0] == '\r' &&
		buffer[1] == '\n')
	{
		// �g�[�N���쐬
		m_tokenList->Add(Token<TChar>(TokenType_NewLine, buffer, buffer + 2));
		return 2;
	}
	if (buffer[0] == '\n' ||
		buffer[0] == '\n')
	{
		// �g�[�N���쐬
		m_tokenList->Add(Token<TChar>(TokenType_NewLine, buffer, buffer + 1));
		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadIdentifier(const TChar* buffer)
{
	// ���ʎq�̐擪�������H
	int count = CheckIdentStart(buffer);
	if (count == 0) {
		return 0;	// ���ʎq�ł͂Ȃ�
	}

	// ��ɑ��������͐��������H
	const TChar* pPos = buffer + count;
	while (pPos < m_bufferEnd)
	{
		count = CheckIdentLetter(pPos);
		if (count == 0)
			break;
		pPos += count;
	}

	// �g�[�N���쐬
	m_tokenList->Add(Token<TChar>(TokenType_Identifier, buffer, pPos));
	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadKeyword(const TChar* buffer)
{
	int lnagTokenType = 0;
	int count = CheckKeyword(buffer, &lnagTokenType);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_Keyword, lnagTokenType, buffer, buffer + count));
	}
	return count;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadNumericLiteral(const TChar* buffer)
{
	/* 123	10�i�����e����
	* 012	8�i�����e����
	*
	*/


	// TODO: ���̓��e�� LangDef �Ɏ����Ă����ׂ������H

	bool	bExistPeriod = false;		/* �����_���o�ꂵ����TRUE */
	bool	bExistExp = false;		/* �w�����o�ꂵ����TRUE */
	bool	bExistSuffix = false;		/* �ڔ��ꂪ�o�ꂵ����TRUE */
	bool	bHexMode = false;		/* 16�i���ŗL��ꍇ��TRUE */

	// 16�i���̐擪�`�F�b�N
	const TChar* pPos = buffer;
	int count = CheckHexLiteralStart(buffer);
	if (count > 0) {
		pPos += count;
		bHexMode = true;	// 16 �i���ł���
	}

	int nStep = 0;

	bool bEnd = false;
	while (pPos < m_bufferEnd && !bEnd)
	{
		if (*pPos > 255) {	// wchar_t �͈̔̓`�F�b�N
			break;
		}
		AlphaNumType charType = (AlphaNumType)g_alphaNumTypeTable[*pPos];

		// �}���`�o�C�g�R�[�h�܂��͐��䕶���܂��� . �ȊO�̉��Z�q�ł���ΏI��
		if (charType == AlphaNumType_MBC ||
			charType == AlphaNumType_Control ||
			(charType == AlphaNumType_OpChar && *pPos != '.')){
			break;	// �I��
		}

		// �T�t�B�b�N�X�̃`�F�b�N (�T�t�B�b�N�X�͈�ԍŌゾ���A���낢��ȂƂ���ɏ�����B1f 1.0f .1f 1.0-e2f)
		if (charType == AlphaNumType_Alphabet || charType == AlphaNumType_HexAlpha){
			// �����^
			count = CheckIntegerSuffix(pPos);
			if (count > 0) {
				pPos += count;
				bExistSuffix = true;	// �T�t�B�b�N�X��������
				nStep = 10;
				continue;
			}
			// �����^
			count = CheckRealSuffix(pPos);
			if (count > 0) {
				pPos += count;
				bExistSuffix = true;	// �T�t�B�b�N�X��������
				nStep = 10;
				continue;
			}
			// �� 50LL �̂悤�ɏd���̉\�������邽�߁A�����������Ă��G���[�ɂ��Ȃ�
		}

		if (bExistSuffix) {
			return 0;	// �T�t�B�b�N�X�̌�ɁA�T�t�B�b�N�X�ł͂Ȃ������������B��������
		}

		switch (nStep)
		{
		case 0:		// ��������
		{
						if (bHexMode && charType == AlphaNumType_HexAlpha) {
							pPos++;		// 16 �i���Ȃ� HexAlpha �������Čp��
						}
						else if (charType == AlphaNumType_Number) {
							pPos++;		// ���ʂ̐��l�� OK
						}
						else if (charType == AlphaNumType_OpChar && *pPos == '.') {
							nStep = 1;	// . �����������B���������ֈڍs
							pPos++;
						}
						else {
							return 0;	// 10G ���A�ςȃT�t�B�b�N�X�Ƃ��A���̎��ʎq�Ƃ̊ԂɃX�y�[�X�������Ƃ�
						}
						break;
		}
		case 1:		// ��������
		{
						if (bHexMode && charType == AlphaNumType_HexAlpha) {
							pPos++;		// 16 �i���Ȃ� HexAlpha �������Čp��
						}
						else if (charType == AlphaNumType_Number) {
							pPos++;		// ���ʂ̐��l�� OK
						}
						else
						{
							count = CheckExponentStart(pPos);
							if (count > 0) {	// e E + -
								nStep = 2;		// e- �Ƃ������������B�w�������ֈڍs
								pPos += count;
							}
							else {
								return 0;		// 10.5G ���A�ςȃT�t�B�b�N�X�Ƃ��A���̎��ʎq�Ƃ̊ԂɃX�y�[�X�������Ƃ�
							}
						}
						break;
		}
		case 2:		// �w������
		{
						if (charType == AlphaNumType_Number) {
							pPos++;		// ���ʂ̐��l�� OK
						}
						else {
							return 0;	// ����ȊO�͎��s
						}
						break;
		}
		}
	}

	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadCharOrStringLiteral(const TChar* buffer)
{
	// ������̊J�n�`�F�b�N
	int startCount = CheckStringStart(buffer);
	if (startCount == 0) {
		return 0;
	}

	const TChar* pStart = buffer;
	const TChar* pPos = buffer + 1;
	int endCount = 0;
	while (pPos < m_bufferEnd)
	{
		// �G�X�P�[�v�V�[�P���X�̃`�F�b�N
		int count = CheckStringEscape(pPos, pStart);
		if (count > 0) {
			pPos += count;
			continue;	// ���̌�̉�͂ɂ͉񂳂Ȃ��Ŏ��̕�����
		}

		// ������̏I���`�F�b�N
		endCount = CheckStringEnd(pPos, pStart);
		if (endCount > 0) {
			pPos += endCount;
			break;		// ������I��
		}

		pPos++;
	}

	Token<TChar> token(TokenType_CharOrStringLiteral, buffer, pPos);
	token.SetStringValue(buffer + startCount, pPos - endCount);
	m_tokenList->Add(token);

	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadOperator(const TChar* buffer)
{
	return CheckOperator(buffer);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadComment(const TChar* buffer)
{
	// �͈̓R�����g�̊J�n�`�F�b�N
	int count = CheckCommentStart(buffer);
	if (count != 0)
	{
		const TChar* pStart = buffer;
		const TChar* pPos = buffer + 1;
		while (pPos < m_bufferEnd)
		{
			// �I���`�F�b�N
			count = CheckCommentEnd(pPos);
			if (count > 0) {
				pPos += count;
				break;		// �I��
			}

			pPos++;
		}
		return pPos - buffer;
	}

	// �s�R�����g�̊J�n�`�F�b�N
	count = CheckCommentLine(buffer);
	if (count != 0)
	{
		const TChar* pStart = buffer;
		const TChar* pPos = buffer + 1;
		while (pPos < m_bufferEnd)
		{
			// �s�� \ �`�F�b�N
			count = ReadEscNewLine(pPos);
			if (count > 0) {
				pPos += count;
				continue;		// �p��
			}

			// �I���`�F�b�N
			count = CheckCommentEnd(pPos);
			if (*pPos == '\n') {
				//pPos += 1;	// \n �̓R�����g�Ɋ܂܂Ȃ�
				break;			// �I��
			}

			pPos++;
		}
		return pPos - buffer;
	}

	return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//template<typename TChar>
//int Lexer<TChar>::ReadPrePro(const TChar* buffer)
//{
//	return 0;
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadEscNewLine(const TChar* buffer)
{
	return CheckEscNewLine(buffer);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadMBSSequence(const TChar* buffer)
{
	// �}���`�o�C�g��������

	const TChar* pPos = buffer;
	while (pPos < m_bufferEnd)
	{
		int extraCount = 0;
		Text::UnicodeUtils::CheckUTF8TrailingBytes((const Text::UnicodeUtils::UTF8*)pPos, (const Text::UnicodeUtils::UTF8*)m_bufferEnd, false, &extraCount);

		// �ǉ��o�C�g������ = �V���O���o�C�g�����������ꍇ�͏I��
		if (extraCount == 0) {
			break;
		}

		// ���񒲂ׂ������ƁA�ǉ��������i�߂�
		pPos += (1 + extraCount);
	};

	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
AlphaNumType Lexer<TChar>::GetAlphaNumType(TChar ch)
{
	if (ch > 255) {
		return AlphaNumType_MBC;	// wchar_t �^�̑��o�C�g�R�[�h
	}
	return (AlphaNumType)g_alphaNumTypeTable[ch];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//template<typename TChar>
//int Lexer<TChar>::ReadHexLiteralStart(const TChar* buffer)
//{
//	return CheckHexLiteralStart(buffer);
//}

// �e���v���[�g�̃C���X�^���X��
template class Lexer<char>;
template class Lexer<wchar_t>;

} // namespace Parser
} // namespace Lumino
