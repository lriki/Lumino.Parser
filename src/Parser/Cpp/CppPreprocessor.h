
#pragma once

#include "../../../include/Lumino/Parser/Token.h"
#include "../../../include/Lumino/Parser/ErrorInfo.h"


namespace Lumino
{
namespace Parser
{

template<typename TChar>
class CppPreprocessor
{
public:
	typename typedef Array< Token<TChar> > TokenList;
	typename typedef Array< Token<TChar> >::const_iterator Position;

public:

	void Analyze(const TokenList& tokenList, ErrorManager* errorManager);

private:
};

} // namespace Parser
} // namespace Lumino
