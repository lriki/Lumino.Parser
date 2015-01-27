
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

} // namespace Parser
} // namespace Lumino
