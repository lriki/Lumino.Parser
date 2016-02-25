
#include "Internal.h"
#include "Frontend\Cpp\CppLexer.h"
#include "DiagnosticsManager.h"
#include "UnitFile.h"
#include "Context.h"

LN_NAMESPACE_BEGIN
namespace parser
{

//=============================================================================
// Context
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState Context::LookupPreprocessedIncludeFile(const TokenPathName& basePath, const TokenPathName& filePath, const Array<TokenPathName>* additionalIncludePaths, const MacroMapContainer& parentMacroMap, DiagnosticsItemSet* parentDiag, UnitFile** outFile)
{
	LN_CHECK_ARGS_RETURNV(additionalIncludePaths != nullptr, ResultState::Error);
	//LN_CHECK_ARGS_RETURNV(macroMap != nullptr, ResultState::Error);

	uint64_t key = MakeCacheFileKey(additionalIncludePaths, parentMacroMap.GetConst());

	auto itr = m_codeFileMap.find(key);
	if (itr != m_codeFileMap.end())
	{
		*outFile = itr->second;
		return ResultState::Success;	// TODO: CodeFile に解析結果を持たせたい
	}
	else
	{
		PathName includeFilePath;
		// 絶対パスならそのまま使う
		if (filePath.IsAbsolute())
		{
			includeFilePath = filePath;
		}
		// 親コードファイルからの相対パスに存在するか？
		if (basePath.ExistsFileInDirectory(filePath))
		{
			includeFilePath = PathName(basePath, filePath);
		}
		// -I で指定されたフォルダに存在するか？
		else
		{
			for (auto& path : *additionalIncludePaths)
			{
				if (path.ExistsFileInDirectory(filePath))
				{
					includeFilePath = PathName(path, filePath);
					break;
				}
			}
		}

		// Error: include ファイルが見つからなかった
		if (includeFilePath.IsEmpty())
		{
			parentDiag->Report(DiagnosticsCode::Preprocessor_IncludeFileNotFount, includeFilePath.GetString());
			return ResultState::Error;
		}

		// ファイル内容を一度全て読み込む
		// TODO: 文字コード
		FileStream file(includeFilePath, FileOpenMode::Read);
		ByteBuffer buffer(file.GetLength(), false);
		file.Read(buffer.GetData(), buffer.GetSize());

		auto codeFile = RefPtr<UnitFile>::MakeRef();
		codeFile->Initialize(includeFilePath);

		// トークン分割
		CppLexer lex;
		TokenListPtr tokens = lex.Tokenize(buffer, parentDiag);	// TODO: エラー
		codeFile->SetTokenList(tokens);				// トークリストは全部覚えておく (define の置換要素とエンティティ参照を取り出したら消しても良い)

		// プリプロセス
		Preprocessor preprocessor;
		LN_RESULT_CALL(preprocessor.BuildPreprocessedTokenList(this, tokens, codeFile, additionalIncludePaths, parentMacroMap, parentDiag));
		//codeFile->GetMacroMap()->SetFreeze(true);	// 確定。あとは変化しない

		m_codeFileMap.insert({ key, codeFile });
		*outFile = codeFile;
		return ResultState::Success;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Context::MakeCacheFileKey(const Array<TokenPathName>* additionalIncludePaths, const MacroMap* macroMap)
{
	uint64_t key = 0;
	for (auto& path : *additionalIncludePaths)
	{
		key += Hash::CalcHash(path.c_str(), path.GetLength());
	}
	key += macroMap->GetHashCode();
	return key;
}

} // namespace Parser
LN_NAMESPACE_END

