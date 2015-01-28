
#include "../../../include/Lumino/Parser/Tools/SimpleCppIncludePreprocessor.h"
#include "../ParserUtils.h"

namespace Lumino
{
namespace Parser
{


//=============================================================================
// SimpleCppIncludePreprocessor
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void SimpleCppIncludePreprocessor<TChar>::Analyze(const TokenList& tokenList, ErrorManager* errorManager)
{
	m_errorManager = errorManager;

	Position pos = tokenList.begin();
	Position end = tokenList.end();

	while (pos != end)
	{
		if (pos.GetTokenType() == TokenType_NewLine ||
			pos.GetTokenType() == TokenType_EOF)
		{
			Position lineHead;
			Position lineEnd;
			TChar filePath[LN_MAX_PATH];
			if (ParseIncludeLine(pos, &lineHead, &lineEnd, filePath))
			{
				// #include‚ªŒ©‚Â‚©‚Á‚½
			}
		}

		++pos;
	}
}

template<typename TChar>
bool SimpleCppIncludePreprocessor<TChar>::ParseIncludeLine(Position newLinePos, Position* outLineHead, Position* outLineEnd, TChar* outFilePath)
{
	Position pos = ParserUtils<TChar>::GetNextGenericToken(newLinePos);
	if (pos->GetTokenBegin()[0] != '#') return false;
	// TODO
	return false;
}

} // namespace Parser
} // namespace Lumino

