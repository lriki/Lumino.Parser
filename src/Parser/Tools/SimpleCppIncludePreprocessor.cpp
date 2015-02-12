
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

	bool justSawNewLine = true;	// 改行直後かどうか。コード先頭は改行直後とする。

	while (pos != end)
	{
		// 改行が見つかった
		if (pos->GetTokenType() == TokenType_NewLine)
		{
			justSawNewLine = true;
		}
		// 改行直後だった場合は #include を探す
		else if (justSawNewLine)
		{
			Position lineHead;
			Position lineEnd;
			Position headerNameToken;
			if (ParseIncludeLine(pos, &lineHead, &lineEnd, &headerNameToken))
			{
				// #includeが見つかった
				AnalyzeFileToTokenList(settingData);
				//PathNameT filePath(m_currentDirectory, headerNameToken->GetStringValue());
				//RefPtr<RefBuffer> fileData(FileUtils::ReadAllBytes(filePath));

				//CppLexer<TChar> lexer;
				//lexer.Analyze(fileData, m_errorManager);
				//TokenListT* tokens = lexer.GetTokenList();
				//tokens->CloneTokenStrings();		// fileData の参照を切る
				//SimpleCppIncludePreprocessor<TChar> prepro;
				//SimpleCppIncludePreprocessor<TChar>::SettingData preproSetting;
				//preproSetting.CurrentDirectory = filePath.GetParent();
				//preproSetting.ErrorManager = m_errorManager;
				//prepro.Analyze(tokens, preproSetting);

				// lineHead は '#'、lineEnd は '\n' を指している。
				// lineEnd の前までを削除する。→ \n が残ることになり、pos は '\n' を指す。
				// erase が完了すると lineHead、lineEnd は無効なイテレータになるので注意。
				pos = tokenList->erase(lineHead, lineEnd);

				// この時点で pos は NewLine の次を指している。
				// 戻り値は新たに挿入された最初の要素を指すイテレータ
				pos = tokenList->insert(pos, tokens->begin(), tokens->end() - 1);	// 終端には必ず EOF があるので end() -1
				pos += tokens->size() - 1;		// サイズ分進めることで、次のトークン位置に行く (-1 はメインループの最後の ++pos の分)

				// 終端イテレータも更新する
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
	/* settingData.CurrentDirectory は無視する。
	 * これはファイパスを受け取らない Analyze() 用に用意したもので、
	 * ファイルパスを受け取れるこの関数では必要ないもの。
	 * 考慮してしまうとかえって混乱の元になる。
	 */


	//PathNameT filePath(m_currentDirectory, headerNameToken->GetStringValue());
	RefPtr<RefBuffer> fileData(FileUtils::ReadAllBytes(filePath));

	CppLexer<TChar> lexer;
	lexer.Analyze(fileData, m_errorManager);
	TokenListT* tokens = lexer.GetTokenList();
	tokens->CloneTokenStrings();		// fileData の参照を切る
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

	// 文字列
	pos = GetNextGenericToken(pos);
	if (pos->GetTokenType() != TokenType_CharOrStringLiteral) return false;
	*outFilePath = pos;

	// 改行
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
		/*token.GetTokenType() == TokenType_NewLine ||*/	// プリプロセッサでは改行は区別するべき(空白扱いしない)トークンである
		token.GetTokenType() == TokenType_EscNewLine);
}

// テンプレートのインスタンス化
template class SimpleCppIncludePreprocessor<char>;
template class SimpleCppIncludePreprocessor<wchar_t>;

} // namespace Parser
} // namespace Lumino

