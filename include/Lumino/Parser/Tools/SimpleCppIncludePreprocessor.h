
#pragma once

#include "../../../../external/Lumino.Core/include/Lumino/IO/PathName.h"
#include "../CppLexer.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class SimpleCppIncludePreprocessor
{
public:
	typename typedef BasicString<TChar>			StringT;
	typename typedef Token<TChar>				TokenT;
	typename typedef TokenList<TChar>			TokenListT;
	typename typedef RefPtr<TokenListT>			TokenListPtr;
	typename typedef TokenListT::const_iterator Position;
	typename typedef BasicPathName<TChar>		PathNameT;

public:
	struct SettingData
	{
		PathName		CurrentDirectory;
		ErrorManager*	ErrorManager;
	};

public:
	void Analyze(const PathName& fileFullPath, ErrorManager* errorManager);
	TokenListT* GetTokenList() { return m_tokenList; }


private:
	//void Analyze(TokenListT* tokenList, const SettingData& settingData);


	/// (settingData.CurrentDirectory は無視されます)
	//TokenListT* AnalyzeFileToTokenList(const PathNameT& filePath);

	//static StringT AnalyzeStringToString(const StringT& text, const SettingData& settingData);

private:
	//struct IncludeFileInfo
	//{
	//	PathNameT FileName;
	//	PathNameT CurrentDirectory;
	//};

	bool CheckLoadedIncludeFile(const PathNameT& fileFullPath) const { return m_loadedIncludeFileNames.Contains(fileFullPath); }

	bool LoadIncludeFile(const PathNameT& fileFullPath, int includeNest, TokenListPtr* outTokens);

	void AnalyzeTokenList(TokenListT* tokenList, const PathNameT& currentDirecotry, int includeNest);	// 解析メイン。再起呼び出し

private:
	bool ParseIncludeLine(Position posSharp, Position* outLineHead, Position* outLineEnd, Position* outFilePath);
	Position SkipGenericSpace(Position pos);
	Position GetNextGenericToken(Position pos);
	bool IsGenericSpace(const TokenT& token);

private:
	ErrorManager*		m_errorManager;
	PathNameT			m_currentDirectory;
	//int					m_lineNumber;
	Array<PathNameT>	m_loadedIncludeFileNames;	///< auto pragma once マーク済み (読み込み済み) include ファイル名リスト
	RefPtr<TokenListT>	m_tokenList;
};

} // namespace Parser
} // namespace Lumino
