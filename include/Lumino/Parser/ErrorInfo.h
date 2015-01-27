
#pragma once

#include <Lumino/Base/RefBuffer.h>
#include <Lumino/Base/Array.h>
#include "Token.h"

namespace Lumino
{
namespace Parser
{

enum ErrorCode
{
	ErrorCode_Unknown = 0,	///< 不明なエラー

	ErrorCode_Error_UnknownToken,	///< 不明なトークンが見つかった


};

class ErrorItem
{
public:
	ErrorItem(ErrorCode errorCode)
	{
		m_errorCode = errorCode;
	}

private:
	ErrorCode	m_errorCode;
};

class ErrorManager
{
public:
	void AddError(ErrorCode errorCode);

private:
	Array<ErrorItem> m_errorList;
};

} // namespace Parser
} // namespace Lumino
