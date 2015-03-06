
#pragma once

#include "Token.h"
#include "TokenList.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class ParserObject
{
public:
	typename typedef BasicString<TChar>			StringT;
	typename typedef Token<TChar>				TokenT;
	typename typedef TokenList<TChar>			TokenListT;
	typename typedef RefPtr<TokenListT>			TokenListPtrT;
	typename typedef TokenListT::const_iterator Position;

};

} // namespace Parser
} // namespace Lumino

