
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN
namespace parser
{
enum class DiagnosticsCode
{
	Severity_Note		= 0x00000000,
	Severity_Warning	= 0x10000000,
	Severity_Error		= 0x20000000,
	Severity_Fatal		= 0x30000000,

	CppLexerGroup = 0x00010000,
	//CppLexer_InvalidToken = Severity_Error | CppLexerGroup | 0x0001,
	UnexpectedEOFInBlockComment = Severity_Error | CppLexerGroup | 0x0001,


	RpnEvaluatorGroup = 0x00020000,
	RpnEvaluator_InvalidNumericLiteral	= Severity_Error | RpnEvaluatorGroup | 1,
	RpnEvaluator_UnexpectedToken		= Severity_Error | RpnEvaluatorGroup | 2,	/**< 予期しないトークンが見つかった。*/
	RpnEvaluator_InsufficientToken		= Severity_Error | RpnEvaluatorGroup | 3,	/**< 式を評価するためのトークンが不足している。(式が完結していない) */

};

enum class SeverityLevel
{
	Note,
	Warning,
	Error,
	Fatal,
};
	
/**
	@brief
*/
class DiagnosticsManager
{
public:
	//void SetFilePath(const PathName& filePath) { m_filePath = filePath; }
	int SetLineNumber(int line) { m_lineNumber = line; }		// 0～
	int SetColumnNumber(int column) { m_columnNumber = column; }	// 0～

	void Report();

private:
	int			m_lineNumber;
	int			m_columnNumber;
};

/**
	@brief
*/
class DiagnosticsItem
{
public:
	DiagnosticsCode GetCode() const { return m_code; }

private:
	friend class DiagnosticsItemSet;
	//DiagnosticsItem();
	//PathName	m_filePath;		// できるだけ DiagnosticsManager から直接代入するようにし、メモリを共有できるようにする
	DiagnosticsCode	m_code;
	int				m_lineNumber;
	int				m_columnNumber;
};

/**
	@brief
*/
class DiagnosticsItemSet
{
public:
	/*
		ファイルパスなど複数のエラーが持つ情報をまとめたもの。
		ファイルパス用のメモリを Item ごとに持たせないようにするのも目的の1つ。
		並列処理するときも、エラーレポートのたびにロックするのはよろしくない。
	*/

	int SetCurrentLineNumber(int line) { m_currentLineNumber = line; }
	int SetCurrentColumnNumber(int column) { m_currentColumnNumber = column; }

	void Report(DiagnosticsCode code);

	const Array<DiagnosticsItem>* GetItems() { return &m_items; }

private:
	PathName	m_filePath;
	int			m_currentLineNumber;
	int			m_currentColumnNumber;
	Array<DiagnosticsItem>	m_items;
};


} // namespace Parser
LN_NAMESPACE_END

