
#pragma once

#include "../../include/Lumino/Parser/ErrorInfo.h"

namespace Lumino
{
namespace Parser
{

//=============================================================================
// ErrorManager
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ErrorManager::AddError(ErrorCode errorCode)
{
	m_errorList.Add(ErrorItem(errorCode));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void ErrorManager::AddError(ErrorCode errorCode, const TChar* filePath, int lineNumber, const TChar* message)
{
	m_errorList.Add(ErrorItem(errorCode, String(filePath), lineNumber, String(message)));
}

} // namespace Parser
} // namespace Lumino
