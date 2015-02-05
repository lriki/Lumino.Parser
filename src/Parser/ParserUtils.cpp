
#include "ParserUtils.h"

namespace Lumino
{
namespace Parser
{


//=============================================================================
// ParserUtils
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
typename ParserUtils<TChar>::Position ParserUtils<TChar>::GetNextGenericToken(Position pos)
{
	while (!pos->IsEOF())
	{
		++pos;
		if (pos->IsGenericToken()) {
			return pos;
		}
	}
	return pos;
}

// テンプレートのインスタンス化
template class ParserUtils<char>;
template class ParserUtils<wchar_t>;

} // namespace Parser
} // namespace Lumino

