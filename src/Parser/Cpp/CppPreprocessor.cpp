
/*
	�� �u���v�f�̓\�[�X�� TokenList �֎��ۂɓW�J����B
		�������g�p�ʂ͑����Ȃ邪�A�����Q�Ƃɂ���Ɗ֐��`���}�N���̓W�J�����ɕ��G�ɂȂ�B
		�Ƃ������A�֐��`���� # ## ���Z�q�͐V�����g�[�N�������K�v������̂ŁA���� new ���瓦���邱�Ƃ͂ł��Ȃ��B
*/
#include <Lumino/IO/FileUtils.h>
#include <Lumino/IO/MemoryStream.h>
#include <Lumino/Text/EncodingDetector.h>
#include "../ParserUtils.h"
#include "../../../include/Lumino/Parser/Cpp/CppPreprocessor.h"

namespace Lumino
{
namespace Parser
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void CppPreprocessor<TChar>::Analyze(TokenListT& tokenList, ErrorManager* errorManager)
{
	m_errorInfo = errorManager;
	Position pos = tokenList.begin();
	Position end = tokenList.end();
	bool justSawNewLine = true;	// ���s���ォ�ǂ����B�R�[�h�擪�͉��s����Ƃ���B
	int lineNumber = 0;

	while (pos != end)
	{
		// ���s����������
		if (pos->GetTokenType() == TokenType_NewLine)
		{
			justSawNewLine = true;	// ���s�����Ԃɂ���
			lineNumber++;
		}
		// ���s���ゾ�����ꍇ�� # ��T��
		else if (justSawNewLine)
		{
			Position lineEnd;
			if (AnalyzeLine(pos, &lineEnd))
			{
				pos = lineEnd;
			}
			else
			{
				justSawNewLine = false;
			}
		}
		// ���s�ł��v���v���s���ł��Ȃ�
		else {
		}

		++pos;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool CppPreprocessor<TChar>::AnalyzeLine(Position lineHead, Position* outLineEnd)
{
	// �s�� # �`�F�b�N
	Position pos = SkipGenericSpace(lineHead);
	if (!pos->EqualChar('#')) return false;

	// �L�[���[�h�`�F�b�N
	pos = GetNextGenericToken(pos);

	if (pos->EqualString(LN_T(TChar, "if"), 2)) {

	}
	else if (pos->EqualString(LN_T(TChar, "ifdef"), 5)) {

	}
	else if (pos->EqualString(LN_T(TChar, "ifndef"), 6)) {

	}
	else if (pos->EqualString(LN_T(TChar, "elif"), 4)) {

	}
	else if (pos->EqualString(LN_T(TChar, "endif"), 5)) {

	}
	else if (pos->EqualString(LN_T(TChar, "include"), 7)) {

	}
	else if (pos->EqualString(LN_T(TChar, "define"), 6)) {
		return AnalyzeDefine(pos, outLineEnd);
	}
	else if (pos->EqualString(LN_T(TChar, "undef"), 5)) {

	}
	else if (pos->EqualString(LN_T(TChar, "line"), 4)) {

	}
	else if (pos->EqualString(LN_T(TChar, "error"), 5)) {

	}
	else if (pos->EqualString(LN_T(TChar, "pragma"), 5)) {

	}
	else if (pos->IsLineEnd()) {
		// ��� # �s (����d�l�Ƃ��đ��݂���)
	}
	else {
		// TODO: �s���ȃv���v���Z�b�T
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool CppPreprocessor<TChar>::AnalyzeDefine(Position pos, Position* outLineEnd)
{
	// pos �� "define" ���w���Ă���

	// ���ʎq
	pos = GetNextGenericToken(pos);
	if (pos->GetTokenType() != TokenType_Identifier) {
		// TODO: ���ʎq���K�v�ł�
		return false;
	}

	CppMacro<TChar> macro;

	// '(' ������Ί֐��`���}�N�� (���ʎq�Ƃ̊Ԃɋ󔒂͋����Ȃ�)
	++pos;
	if (pos->GetLangTokenType() == TT_CppOP_LeftParen)
	{
		while (true)
		{
			pos = GetNextGenericToken(pos);
			if (pos->GetTokenType() == TokenType_Identifier) {
				// ��������
				macro.AddParam(*pos);
			}
			else if (pos->GetLangTokenType() == TT_CppOP_Ellipsis) {
				// ...
				macro.AddParam(*pos);
			}
			else if (pos->GetLangTokenType() == TT_CppOP_Comma) {
				// ,
			}
			else if (pos->GetLangTokenType() == TT_CppOP_RightParen) {
				// )
				++pos;
				break;	// ���������т̏I�[
			}
			else {
				// TODO: �Ȃ񂩕ςȕ�����������
				return false;
			}
		}
	}

	// ���̎��_�� pos �� ���ʎq�܂��� ) �̎� (���ʂ͋󔒕���) ���w���Ă���

	// �s���܂œǂ�
	pos = SkipGenericSpace(pos);	// ���� pos �����s�������炻�̂܂܂ɂȂ�
	Position lineEnd = pos;
	bool spaceOnly = true;
	while (!lineEnd->IsLineEnd())
	{ 
		++lineEnd;
		if (!lineEnd->IsGenericSpace()) {
			spaceOnly = false;
		}
	}

	// �󔒈ȊO�̕���������Βu���v�f�Ƃ��āA���s�̂ЂƂO�܂ł�ێ����Ă���
	if (!spaceOnly) {
		macro.SetReplacementList(pos, lineEnd);
	}

	*outLineEnd = lineEnd;
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename CppPreprocessor<TChar>::Position CppPreprocessor<TChar>::SkipGenericSpace(Position pos)
{
	while (!pos->IsEOF())
	{
		if (!IsGenericSpace(*pos)) {
			return pos;
		}
		++pos;
	}
	return pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename CppPreprocessor<TChar>::Position CppPreprocessor<TChar>::GetNextGenericToken(Position pos)
{
	while (!pos->IsEOF())
	{
		++pos;
		if (!IsGenericSpace(*pos)) {
			return pos;
		}
	}
	return pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool CppPreprocessor<TChar>::IsGenericSpace(const TokenT& token)
{
	return (
		token.GetTokenType() == TokenType_SpaceSequence ||
		token.GetTokenType() == TokenType_Comment ||
		/*token.GetTokenType() == TokenType_NewLine ||*/	// �v���v���Z�b�T�ł͉��s�͋�ʂ���ׂ�(�󔒈������Ȃ�)�g�[�N���ł���
		token.GetTokenType() == TokenType_EscNewLine);
}

// �e���v���[�g�̃C���X�^���X��
template class CppPreprocessor<char>;
template class CppPreprocessor<wchar_t>;

} // namespace Parser
} // namespace Lumino
