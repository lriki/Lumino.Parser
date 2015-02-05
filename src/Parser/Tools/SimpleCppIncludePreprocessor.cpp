
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
				PathNameT filePath(m_currentDirectory, headerNameToken->GetStringValue());
				RefPtr<RefBuffer> fileData(FileUtils::ReadAllBytes(filePath));

				CppLexer<TChar> lexer;
				lexer.Analyze(fileData, m_errorManager);
				TokenList& tokens = lexer.GetTokenList();

				// この時点で pos は NewLine の次を指している。
				// 戻り値は新たに挿入された最初の要素を指すイテレータ
				pos = tokenList->insert(pos, tokens.begin(), tokens.end() - 1);	// 終端には必ず EOF があるので end() -1
				pos += tokens.size() - 1;		// サイズ分進めることで、次のトークン位置に行く (-1 はメインループの最後の ++pos の分)

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
bool SimpleCppIncludePreprocessor<TChar>::ParseIncludeLine(Position posSharp, Position* outLineHead, Position* outLineEnd, Position* outFilePath)
{
	Position pos = posSharp;

	// #
	//pos = GetNextGenericToken(newLinePos);
	if (!pos->EqualChar('#')) return false;

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
		/*token.GetTokenType() == TokenType_NewLine ||*/	// プリプロセッサでは改行は区別するべき(空白扱いしない)トークンである
		token.GetTokenType() == TokenType_EscNewLine);
}

// テンプレートのインスタンス化
template class SimpleCppIncludePreprocessor<char>;
template class SimpleCppIncludePreprocessor<wchar_t>;

} // namespace Parser
} // namespace Lumino

