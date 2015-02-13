
#pragma once

#include <Lumino/IO/PathName.h>
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
	void Analyze(TokenListT* tokenList, const SettingData& settingData);


	/// (settingData.CurrentDirectory �͖�������܂�)
	//TokenListT* AnalyzeFileToTokenList(const PathNameT& filePath);

	static StringT AnalyzeStringToString(const StringT& text, const SettingData& settingData);

private:
	//struct IncludeFileInfo
	//{
	//	PathNameT FileName;
	//	PathNameT CurrentDirectory;
	//};

	bool LoadIncludeFile(const PathNameT& filePath, int includeNest, TokenListPtr* outTokens);

	void AnalyzeTokenList(TokenListT* tokenList, const PathNameT& currentDirecotry, int includeNest);	// ��̓��C���B�ċN�Ăяo��

private:
	bool ParseIncludeLine(Position posSharp, Position* outLineHead, Position* outLineEnd, Position* outFilePath);
	Position SkipGenericSpace(Position pos);
	Position GetNextGenericToken(Position pos);
	bool IsGenericSpace(const TokenT& token);

private:
	ErrorManager*		m_errorManager;
	PathNameT			m_currentDirectory;
	//int					m_lineNumber;
};

} // namespace Parser
} // namespace Lumino
