
#include <Lumino/IO/FileUtils.h>
#include <Lumino/IO/MemoryStream.h>
#include <Lumino/Text/EncodingDetector.h>
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
void SimpleCppIncludePreprocessor<TChar>::Analyze(const PathName& fileFullPath, ErrorManager* errorManager)
{
	m_errorManager = errorManager;

	// �t�@�C�����J��
	RefPtr<ByteBuffer> fileData(FileUtils::ReadAllBytes(fileFullPath));

	// �����R�[�h���� (BOM �������Ӗ��ł������R�[�h�������ʂ͕K�{)
	Text::EncodingDetector detector;
	Text::EncodingType charCode = detector.Detect(fileData->GetData(), fileData->GetSize());
	Text::Encoding* enc = Text::Encoding::GetEncoding(charCode);

	// TChar �֕����R�[�h�ϊ�
	Text::EncodingConversionResult result;
	RefPtr<ByteBuffer> code(
		Text::Encoding::Convert(fileData->GetData(), fileData->GetSize(), enc, Text::Encoding::GetEncodingTemplate<TChar>(), &result));

	// lex
	CppLexer<TChar> lexer;
	lexer.Analyze(code, m_errorManager);
	TokenListPtr tokenList(lexer.GetTokenList(), true);
	tokenList->CloneTokenStrings();		// code �̎Q�Ƃ�؂�

	// ��͊J�n
	PathNameT dir(fileFullPath.GetParent());
	AnalyzeTokenList(tokenList, dir, 0);

	m_tokenList = tokenList;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//template<typename TChar>
//void SimpleCppIncludePreprocessor<TChar>::Analyze(TokenListT* tokenList, const SettingData& settingData)
//{
//	m_currentDirectory = settingData.CurrentDirectory.GetCStr();
//	m_errorManager = settingData.ErrorManager;
//
//	AnalyzeTokenList(tokenList, m_currentDirectory, 0);
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename bool SimpleCppIncludePreprocessor<TChar>::LoadIncludeFile(const PathNameT& fileFullPath, int includeNest, TokenListPtr* outTokens)
{
	// �t�@�C�����J��
	// TODO:�����Ɖ�͂���Ȃ�A������ -I �̃��X�g�ƌ������t�@�C����������B
	if (!FileUtils::Exists(fileFullPath)) {
		return false;
	}
	RefPtr<ByteBuffer> fileData(FileUtils::ReadAllBytes(fileFullPath));

	// �����R�[�h���� (BOM �������Ӗ��ł������R�[�h�������ʂ͕K�{)
	Text::EncodingDetector detector;
	Text::EncodingType charCode = detector.Detect(fileData->GetData(), fileData->GetSize());
	Text::Encoding* enc = Text::Encoding::GetEncoding(charCode);

	// TChar �֕����R�[�h�ϊ�
	Text::EncodingConversionResult result;
	RefPtr<ByteBuffer> code(
		Text::Encoding::Convert(fileData->GetData(), fileData->GetSize(), enc, Text::Encoding::GetEncodingTemplate<TChar>(), &result));

	// lex
	CppLexer<TChar> lexer;
	lexer.Analyze(code, m_errorManager);
	TokenListPtr tokens(lexer.GetTokenList(), true);
	tokens->CloneTokenStrings();		// code �̎Q�Ƃ�؂�

	// �ċA�ŉ��
	AnalyzeTokenList(tokens, fileFullPath.GetParent(), includeNest + 1);

	// �ǂݍ��݊����Bpragma once �`�F�b�N�p���X�g��
	m_loadedIncludeFileNames.Add(fileFullPath);

	outTokens->Attach(tokens, true);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void SimpleCppIncludePreprocessor<TChar>::AnalyzeTokenList(TokenListT* tokenList, const PathNameT& currentDirecotry, int includeNest)
{
	Position pos = tokenList->begin();
	Position end = tokenList->end();

	bool justSawNewLine = true;	// ���s���ォ�ǂ����B�R�[�h�擪�͉��s����Ƃ���B
	int lineNumber = 0;

	while (pos != end)
	{
		// ���s����������
		if (pos->GetTokenType() == TokenType_NewLine)
		{
			justSawNewLine = true;
			lineNumber++;
		}
		// ���s���ゾ�����ꍇ�� #include ��T��
		else if (justSawNewLine)
		{
			Position lineHead;
			Position lineEnd;
			Position headerNameToken;
			if (ParseIncludeLine(pos, &lineHead, &lineEnd, &headerNameToken))
			{
				// #include�����������̂ŁAinclude �t�@�C����ǂݍ��݁A�g�[�N�����X�g���擾����
				PathNameT fileFullPath(currentDirecotry, headerNameToken->GetStringValue());
				fileFullPath = fileFullPath.CanonicalizePath();
				printf("include : %s\n", fileFullPath.GetCStr());

				if (CheckLoadedIncludeFile(fileFullPath))
				{
					// ���łɓǂݍ��ݍς݂� include �t�@�C���ł���B
					// '#' �` '\n' �̑O �܂ł��폜����
					pos = tokenList->erase(lineHead, lineEnd);
					// �I�[�C�e���[�^���X�V����
					end = tokenList->end();
				}
				else
				{
					TokenListPtr includeTokens;
					if (LoadIncludeFile(fileFullPath, includeNest + 1, &includeTokens))	// lineNumber �̓G���[�o�͗p
					{
						// lineHead �� '#'�AlineEnd �� '\n' ���w���Ă���B
						// lineEnd �̑O�܂ł��폜����B�� \n ���c�邱�ƂɂȂ�Apos �� '\n' ���w���B
						// erase ����������� lineHead�AlineEnd �͖����ȃC�e���[�^�ɂȂ�̂Œ��ӁB
						pos = tokenList->erase(lineHead, lineEnd);

						// ���̎��_�� pos �� NewLine �̎����w���Ă���B
						// �߂�l�͐V���ɑ}�����ꂽ�ŏ��̗v�f���w���C�e���[�^
						pos = tokenList->insert(pos, includeTokens->begin(), includeTokens->end() - 1);	// �I�[�ɂ͕K�� EOF ������̂� end() -1
						pos += includeTokens->size() - 1;		// �T�C�Y���i�߂邱�ƂŁA���̃g�[�N���ʒu�ɍs�� (-1 �̓��C�����[�v�̍Ō�� ++pos �̕�)

						// �I�[�C�e���[�^���X�V����
						end = tokenList->end();
					}
					else {
						// include �t�@�C����������Ȃ���Βu�����Ȃ��ő��s����
						//m_errorManager->AddError(ErrorCode_Warning_FileNotFound, lineNumber);
						printf("not found --->>> %s\n", fileFullPath.GetCStr());
					}
				}
			}

			justSawNewLine = true;	// ���s�����Ԉێ�
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
//template<typename TChar>
//typename SimpleCppIncludePreprocessor<TChar>::StringT SimpleCppIncludePreprocessor<TChar>::AnalyzeStringToString(const StringT& text, const SettingData& settingData)
//{
//	ByteBuffer buffer;
//	buffer.Reserve((byte_t*)text.GetCStr(), text.GetByteCount());
//
//	CppLexer<TChar> lexer;
//	lexer.Analyze(&buffer, settingData.ErrorManager);
//
//	SimpleCppIncludePreprocessor<TChar> prepro;
//	prepro.Analyze(lexer.GetTokenList(), settingData);
//
//	MemoryStream stream;
//	lexer.GetTokenList()->DumpText(&stream);
//
//	return StringT((TChar*)stream.GetBuffer(), stream.GetSize() / sizeof(TChar));
//}
//
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
	if (pos->GetTokenType() != TokenType_StringLiteral) return false;
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

