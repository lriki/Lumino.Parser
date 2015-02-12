
#include <Lumino/IO/FileUtils.h>
#include <Lumino/IO/MemoryStream.h>
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
void SimpleCppIncludePreprocessor<TChar>::Analyze(TokenListT* tokenList, const SettingData& settingData)
{
	m_currentDirectory = settingData.CurrentDirectory.GetCStr();
	m_errorManager = settingData.ErrorManager;

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
				AnalyzeFileToTokenList(settingData);
				//PathNameT filePath(m_currentDirectory, headerNameToken->GetStringValue());
				//RefPtr<RefBuffer> fileData(FileUtils::ReadAllBytes(filePath));

				//CppLexer<TChar> lexer;
				//lexer.Analyze(fileData, m_errorManager);
				//TokenListT* tokens = lexer.GetTokenList();
				//tokens->CloneTokenStrings();		// fileData �̎Q�Ƃ�؂�
				//SimpleCppIncludePreprocessor<TChar> prepro;
				//SimpleCppIncludePreprocessor<TChar>::SettingData preproSetting;
				//preproSetting.CurrentDirectory = filePath.GetParent();
				//preproSetting.ErrorManager = m_errorManager;
				//prepro.Analyze(tokens, preproSetting);

				// lineHead �� '#'�AlineEnd �� '\n' ���w���Ă���B
				// lineEnd �̑O�܂ł��폜����B�� \n ���c�邱�ƂɂȂ�Apos �� '\n' ���w���B
				// erase ����������� lineHead�AlineEnd �͖����ȃC�e���[�^�ɂȂ�̂Œ��ӁB
				pos = tokenList->erase(lineHead, lineEnd);

				// ���̎��_�� pos �� NewLine �̎����w���Ă���B
				// �߂�l�͐V���ɑ}�����ꂽ�ŏ��̗v�f���w���C�e���[�^
				pos = tokenList->insert(pos, tokens->begin(), tokens->end() - 1);	// �I�[�ɂ͕K�� EOF ������̂� end() -1
				pos += tokens->size() - 1;		// �T�C�Y���i�߂邱�ƂŁA���̃g�[�N���ʒu�ɍs�� (-1 �̓��C�����[�v�̍Ō�� ++pos �̕�)

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
typename SimpleCppIncludePreprocessor<TChar>::TokenListT* SimpleCppIncludePreprocessor<TChar>::AnalyzeFileToTokenList(const PathNameT& filePath, const SettingData& settingData)
{
	/* settingData.CurrentDirectory �͖�������B
	 * ����̓t�@�C�p�X���󂯎��Ȃ� Analyze() �p�ɗp�ӂ������̂ŁA
	 * �t�@�C���p�X���󂯎��邱�̊֐��ł͕K�v�Ȃ����́B
	 * �l�����Ă��܂��Ƃ������č����̌��ɂȂ�B
	 */


	//PathNameT filePath(m_currentDirectory, headerNameToken->GetStringValue());
	RefPtr<RefBuffer> fileData(FileUtils::ReadAllBytes(filePath));

	CppLexer<TChar> lexer;
	lexer.Analyze(fileData, m_errorManager);
	TokenListT* tokens = lexer.GetTokenList();
	tokens->CloneTokenStrings();		// fileData �̎Q�Ƃ�؂�
	SimpleCppIncludePreprocessor<TChar> prepro;
	SimpleCppIncludePreprocessor<TChar>::SettingData preproSetting;
	preproSetting.CurrentDirectory = filePath.GetParent();
	preproSetting.ErrorManager = m_errorManager;
	prepro.Analyze(tokens, preproSetting);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename SimpleCppIncludePreprocessor<TChar>::StringT SimpleCppIncludePreprocessor<TChar>::AnalyzeStringToString(const StringT& text, const SettingData& settingData)
{
	RefBuffer buffer;
	buffer.Reserve((byte_t*)text.GetCStr(), text.GetByteCount());

	CppLexer<TChar> lexer;
	lexer.Analyze(&buffer, settingData.ErrorManager);

	SimpleCppIncludePreprocessor<TChar> prepro;
	prepro.Analyze(lexer.GetTokenList(), settingData);

	MemoryStream stream;
	lexer.GetTokenList()->DumpText(&stream);

	return StringT((TChar*)stream.GetBuffer(), stream.GetSize() / sizeof(TChar));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool SimpleCppIncludePreprocessor<TChar>::ParseIncludeLine(Position posLineHead, Position* outLineHead, Position* outLineEnd, Position* outFilePath)
{
	// #
	Position pos = SkipGenericSpace(posLineHead);
	if (!pos->EqualChar('#')) return false;
	*outLineHead = posLineHead;

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
	*outLineEnd = pos;

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename SimpleCppIncludePreprocessor<TChar>::Position SimpleCppIncludePreprocessor<TChar>::SkipGenericSpace(Position pos)
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

