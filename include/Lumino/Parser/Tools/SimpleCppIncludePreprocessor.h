
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

	/// (settingData.CurrentDirectory ‚Í–³Ž‹‚³‚ê‚Ü‚·)
	static TokenListT* AnalyzeFileToTokenList(const PathNameT& filePath, const SettingData& settingData);
	static StringT AnalyzeStringToString(const StringT& text, const SettingData& settingData);

private:
	bool ParseIncludeLine(Position posSharp, Position* outLineHead, Position* outLineEnd, Position* outFilePath);
	Position SkipGenericSpace(Position pos);
	Position GetNextGenericToken(Position pos);
	bool IsGenericSpace(const TokenT& token);

private:
	ErrorManager*		m_errorManager;
	PathNameT			m_currentDirectory;
};

} // namespace Parser
} // namespace Lumino
