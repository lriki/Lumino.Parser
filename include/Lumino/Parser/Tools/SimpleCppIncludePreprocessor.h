
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
	typename typedef Token<TChar>				TokenT;
	typename typedef TokenList<TChar>			TokenListT;
	typename typedef TokenListT::const_iterator Position;
	typename typedef BasicPathName<TChar>		PathNameT;

public:
	struct SettingData
	{
		PathName	CurrentDirectory;
	};

public:
	void Analyze(TokenListT* tokenList, const SettingData& settingData, ErrorManager* errorManager);

private:
	bool ParseIncludeLine(Position posSharp, Position* outLineHead, Position* outLineEnd, Position* outFilePath);
	Position GetNextGenericToken(Position pos);
	bool IsGenericSpace(const TokenT& token);

private:
	ErrorManager*		m_errorManager;
	PathNameT			m_currentDirectory;
};

} // namespace Parser
} // namespace Lumino
