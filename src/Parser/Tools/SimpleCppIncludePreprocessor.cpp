
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

	// ファイルを開く
	RefPtr<ByteBuffer> fileData(FileUtils::ReadAllBytes(fileFullPath));

	// 文字コード判別 (BOM を消す意味でも文字コード自動判別は必須)
	Text::EncodingDetector detector;
	Text::EncodingType charCode = detector.Detect(fileData->GetData(), fileData->GetSize());
	Text::Encoding* enc = Text::Encoding::GetEncoding(charCode);

	// TChar へ文字コード変換
	Text::EncodingConversionResult result;
	RefPtr<ByteBuffer> code(
		Text::Encoding::Convert(fileData->GetData(), fileData->GetSize(), enc, Text::Encoding::GetEncodingTemplate<TChar>(), &result));

	// lex
	CppLexer<TChar> lexer;
	lexer.Analyze(code, m_errorManager);
	TokenListPtr tokenList(lexer.GetTokenList(), true);
	tokenList->CloneTokenStrings();		// code の参照を切る

	// 解析開始
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
	// ファイルを開く
	// TODO:ちゃんと解析するなら、ここで -I のリストと結合しつつファイル検索する。
	if (!FileUtils::Exists(fileFullPath)) {
		return false;
	}
	RefPtr<ByteBuffer> fileData(FileUtils::ReadAllBytes(fileFullPath));

	// 文字コード判別 (BOM を消す意味でも文字コード自動判別は必須)
	Text::EncodingDetector detector;
	Text::EncodingType charCode = detector.Detect(fileData->GetData(), fileData->GetSize());
	Text::Encoding* enc = Text::Encoding::GetEncoding(charCode);

	// TChar へ文字コード変換
	Text::EncodingConversionResult result;
	RefPtr<ByteBuffer> code(
		Text::Encoding::Convert(fileData->GetData(), fileData->GetSize(), enc, Text::Encoding::GetEncodingTemplate<TChar>(), &result));

	// lex
	CppLexer<TChar> lexer;
	lexer.Analyze(code, m_errorManager);
	TokenListPtr tokens(lexer.GetTokenList(), true);
	tokens->CloneTokenStrings();		// code の参照を切る

	// 再帰で解析
	AnalyzeTokenList(tokens, fileFullPath.GetParent(), includeNest + 1);

	// 読み込み完了。pragma once チェック用リストへ
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

	bool justSawNewLine = true;	// 改行直後かどうか。コード先頭は改行直後とする。
	int lineNumber = 0;

	while (pos != end)
	{
		// 改行が見つかった
		if (pos->GetTokenType() == TokenType_NewLine)
		{
			justSawNewLine = true;
			lineNumber++;
		}
		// 改行直後だった場合は #include を探す
		else if (justSawNewLine)
		{
			Position lineHead;
			Position lineEnd;
			Position headerNameToken;
			if (ParseIncludeLine(pos, &lineHead, &lineEnd, &headerNameToken))
			{
				// #includeが見つかったので、include ファイルを読み込み、トークンリストを取得する
				PathNameT fileFullPath(currentDirecotry, headerNameToken->GetStringValue());
				fileFullPath = fileFullPath.CanonicalizePath();
				printf("include : %s\n", fileFullPath.GetCStr());

				if (CheckLoadedIncludeFile(fileFullPath))
				{
					// すでに読み込み済みの include ファイルである。
					// '#' ～ '\n' の前 までを削除する
					pos = tokenList->erase(lineHead, lineEnd);
					// 終端イテレータも更新する
					end = tokenList->end();
				}
				else
				{
					TokenListPtr includeTokens;
					if (LoadIncludeFile(fileFullPath, includeNest + 1, &includeTokens))	// lineNumber はエラー出力用
					{
						// lineHead は '#'、lineEnd は '\n' を指している。
						// lineEnd の前までを削除する。→ \n が残ることになり、pos は '\n' を指す。
						// erase が完了すると lineHead、lineEnd は無効なイテレータになるので注意。
						pos = tokenList->erase(lineHead, lineEnd);

						// この時点で pos は NewLine の次を指している。
						// 戻り値は新たに挿入された最初の要素を指すイテレータ
						pos = tokenList->insert(pos, includeTokens->begin(), includeTokens->end() - 1);	// 終端には必ず EOF があるので end() -1
						pos += includeTokens->size() - 1;		// サイズ分進めることで、次のトークン位置に行く (-1 はメインループの最後の ++pos の分)

						// 終端イテレータも更新する
						end = tokenList->end();
					}
					else {
						// include ファイルが見つからなければ置換しないで続行する
						//m_errorManager->AddError(ErrorCode_Warning_FileNotFound, lineNumber);
						printf("not found --->>> %s\n", fileFullPath.GetCStr());
					}
				}
			}

			justSawNewLine = true;	// 改行直後状態維持
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

	// 文字列
	pos = GetNextGenericToken(pos);
	if (pos->GetTokenType() != TokenType_StringLiteral) return false;
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

