
/*
	■ 置換要素はソースの TokenList へ実際に展開する。
		メモリ使用量は多くなるが、もし参照にすると関数形式マクロの展開が非常に複雑になる。
		というか、関数形式の # ## 演算子は新しいトークンを作る必要があるので、結局 new から逃げることはできない。
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
	bool justSawNewLine = true;	// 改行直後かどうか。コード先頭は改行直後とする。
	int lineNumber = 0;

	while (pos != end)
	{
		// 改行が見つかった
		if (pos->GetTokenType() == TokenType_NewLine)
		{
			justSawNewLine = true;	// 改行直後状態にする
			lineNumber++;
		}
		// 改行直後だった場合は # を探す
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
		// 改行でもプリプロ行頭でもない
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
	// 行頭 # チェック
	Position pos = SkipGenericSpace(lineHead);
	if (!pos->EqualChar('#')) return false;

	// キーワードチェック
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
		// 空の # 行 (言語仕様として存在する)
	}
	else {
		// TODO: 不明なプリプロセッサ
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
	// pos は "define" を指している

	// 識別子
	pos = GetNextGenericToken(pos);
	if (pos->GetTokenType() != TokenType_Identifier) {
		// TODO: 識別子が必要です
		return false;
	}

	CppMacro<TChar> macro;

	// '(' があれば関数形式マクロ (識別子との間に空白は許可しない)
	++pos;
	if (pos->GetLangTokenType() == TT_CppOP_LeftParen)
	{
		while (true)
		{
			pos = GetNextGenericToken(pos);
			if (pos->GetTokenType() == TokenType_Identifier) {
				// 仮引数名
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
				break;	// 仮引数並びの終端
			}
			else {
				// TODO: なんか変な文字があった
				return false;
			}
		}
	}

	// この時点で pos は 識別子または ) の次 (普通は空白文字) を指している

	// 行末まで読む
	pos = SkipGenericSpace(pos);	// もじ pos が改行だったらそのままになる
	Position lineEnd = pos;
	bool spaceOnly = true;
	while (!lineEnd->IsLineEnd())
	{ 
		++lineEnd;
		if (!lineEnd->IsGenericSpace()) {
			spaceOnly = false;
		}
	}

	// 空白以外の文字があれば置換要素として、改行のひとつ前までを保持しておく
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
		/*token.GetTokenType() == TokenType_NewLine ||*/	// プリプロセッサでは改行は区別するべき(空白扱いしない)トークンである
		token.GetTokenType() == TokenType_EscNewLine);
}

// テンプレートのインスタンス化
template class CppPreprocessor<char>;
template class CppPreprocessor<wchar_t>;

} // namespace Parser
} // namespace Lumino
