
#include <Lumino/IO/FileUtils.h>
#include "../../../include/Lumino/Parser/Tools/SimpleCppIncludePreprocessor.h"
#include "../ParserUtils.h"

namespace Lumino
{
namespace Parser
{

//=============================================================================
// SimpleCppIncludePreprocessor
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void SimpleCppIncludePreprocessor<TChar>::Analyze(TokenList* tokenList, const SettingData& settingData, ErrorManager* errorManager)
{
	m_currentDirectory = settingData.CurrentDirectory.GetCStr();
	m_errorManager = errorManager;

	Position pos = tokenList->begin();
	Position end = tokenList->end();

	bool justSawNewLine = true;	// ���s���ォ�ǂ����B�R�[�h�擪�͉��s����Ƃ���B

	while (pos != end)
	{
		// ���s����������
		if (pos->GetTokenType() == TokenType_NewLine)
		{
			justSawNewLine = true;
		}
		// ���s���ゾ�����ꍇ�� #include ��T��
		else if (justSawNewLine)
		{
			Position lineHead;
			Position lineEnd;
			Position headerNameToken;
			if (ParseIncludeLine(pos, &lineHead, &lineEnd, &headerNameToken))
			{
				// #include����������
				PathNameT filePath(m_currentDirectory, headerNameToken->GetStringValue());
				RefPtr<RefBuffer> fileData(FileUtils::ReadAllBytes(filePath));

				CppLexer<TChar> lexer;
				lexer.Analyze(fileData, m_errorManager);
				TokenList& tokens = lexer.GetTokenList();

				// ���̎��_�� pos �� NewLine �̎����w���Ă���B
				// �߂�l�͐V���ɑ}�����ꂽ�ŏ��̗v�f���w���C�e���[�^
				pos = tokenList->insert(pos, tokens.begin(), tokens.end() - 1);	// �I�[�ɂ͕K�� EOF ������̂� end() -1
				pos += tokens.size() - 1;		// �T�C�Y���i�߂邱�ƂŁA���̃g�[�N���ʒu�ɍs�� (-1 �̓��C�����[�v�̍Ō�� ++pos �̕�)

				// �I�[�C�e���[�^���X�V����
				end = tokenList->end();
			}

			justSawNewLine = false;
		}
		else {
			justSawNewLine = false;
		}

		++pos;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool SimpleCppIncludePreprocessor<TChar>::ParseIncludeLine(Position posSharp, Position* outLineHead, Position* outLineEnd, Position* outFilePath)
{
	Position pos = posSharp;

	// #
	//pos = GetNextGenericToken(newLinePos);
	if (!pos->EqualChar('#')) return false;

	// include
	pos = GetNextGenericToken(pos);
	if (!pos->EqualString(LN_T(TChar, "include"), 7)) return false;

	// ������
	pos = GetNextGenericToken(pos);
	if (pos->GetTokenType() != TokenType_CharOrStringLiteral) return false;
	*outFilePath = pos;

	// ���s
	pos = GetNextGenericToken(pos);
	if (!pos->IsLineEnd()) return false;

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename SimpleCppIncludePreprocessor<TChar>::Position SimpleCppIncludePreprocessor<TChar>::GetNextGenericToken(Position pos)
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
bool SimpleCppIncludePreprocessor<TChar>::IsGenericSpace(const TokenT& token)
{
	return (
		token.GetTokenType() == TokenType_SpaceSequence ||
		token.GetTokenType() == TokenType_Comment ||
		/*token.GetTokenType() == TokenType_NewLine ||*/	// �v���v���Z�b�T�ł͉��s�͋�ʂ���ׂ�(�󔒈������Ȃ�)�g�[�N���ł���
		token.GetTokenType() == TokenType_EscNewLine);
}

// �e���v���[�g�̃C���X�^���X��
template class SimpleCppIncludePreprocessor<char>;
template class SimpleCppIncludePreprocessor<wchar_t>;

} // namespace Parser
} // namespace Lumino

