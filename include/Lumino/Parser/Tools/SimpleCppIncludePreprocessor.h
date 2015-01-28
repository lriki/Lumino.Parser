
#pragma once

#include "../CppLexer.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class SimpleCppIncludePreprocessor
{
public:
	typename typedef Array< Token<TChar> > TokenList;
	typename typedef Array< Token<TChar> >::const_iterator Position;

public:
	void Analyze(const TokenList& tokenList, ErrorManager* errorManager);

private:
	bool ParseIncludeLine(Position posNLOrEOF, Position* outLineHead, Position* outLineEnd, TChar* outFilePath);

private:
	ErrorManager*	m_errorManager;
};

} // namespace Parser
} // namespace Lumino
