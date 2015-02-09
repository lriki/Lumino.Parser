
#pragma once

#include "../../include/Lumino/Parser/Token.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class ParserUtils
{
public:
	typename typedef Array< Token<TChar> > TokenList;
	typename typedef Array< Token<TChar> >::const_iterator Position;

public:
	static Position GetNextGenericToken(Position pos);

};

} // namespace Parser
} // namespace Lumino
