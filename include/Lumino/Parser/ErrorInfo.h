
#pragma once

#include <stack>
#include "../../../external/Lumino.Core/include/Lumino/Base/ByteBuffer.h"
#include "../../../external/Lumino.Core/include/Lumino/Base/Array.h"
#include "../../../external/Lumino.Core/include/Lumino/IO/PathName.h"
#include "Token.h"

namespace Lumino
{
namespace Parser
{

enum ErrorCode
{
	ErrorCode_Unknown = 0,	///< 不明なエラー

	ErrorCode_Error_UnknownToken,	///< 不明なトークンが見つかった


	ErrorCode_Separator_Warning,	///< これ以降は警告コード

	ErrorCode_Warning_FileNotFound,	///< ファイルが見つからなかった



	// [C/C++] define の # の後は仮引数でなければならない
};

class ErrorItem
{
public:
	ErrorItem(ErrorCode errorCode)
	{
		m_lineNumber = 0;
		m_errorCode = errorCode;
	}

	ErrorItem(ErrorCode errorCode, const String& file, int lineNumber, const String& message)
	{
		m_errorCode = errorCode;
		m_filePath = file;
		m_lineNumber = 0;
		m_messages = message;
	}

private:
	ErrorCode	m_errorCode;
	String		m_filePath;
	int			m_lineNumber;		///< 0 スタート
	String		m_messages;
};

class ErrorManager
{
public:
	void AddError(ErrorCode errorCode);

	template<typename TChar>
	void AddError(ErrorCode errorCode, const TChar* filePath, int lineNumber, const TChar* message);

private:
	Array<ErrorItem> m_errorList;

	struct FileInfo
	{
		PathName	FilePath;
		int			LineNumber;
	};

	std::stack<FileInfo>	m_errorFileInfoStack;
};

} // namespace Parser
} // namespace Lumino
