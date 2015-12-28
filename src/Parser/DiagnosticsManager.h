
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
	RpnEvaluator_InvalidNumericLiteral		= Severity_Error | RpnEvaluatorGroup | 1,
	RpnEvaluator_UnexpectedToken			= Severity_Error | RpnEvaluatorGroup | 2,	/**< 予期しないトークンが見つかりました。*/
	RpnEvaluator_InsufficientToken			= Severity_Error | RpnEvaluatorGroup | 3,	/**< 式を評価するためのトークンが不足しています。(式が完結していない) */
	RpnEvaluator_InvalidFuncCallArgsCount	= Severity_Error | RpnEvaluatorGroup | 4,	/**< 関数呼び出しの引数が足りません。*/
	RpnEvaluator_InvalidOperatorSide		= Severity_Error | RpnEvaluatorGroup | 5,	/**< 演算子の辺に対するオペランドが足りません。*/
	RpnEvaluator_InvalidNumericType			= Severity_Error | RpnEvaluatorGroup | 6,	/**< 無効な数値型のトークンが入力されました。*/
	RpnEvaluator_OperatorInvalidType		= Severity_Error | RpnEvaluatorGroup | 7,	/**< 演算子の型に無効な型({0})のオペランドが指定されています。*/
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
	const StringArrayA& GetOptions() const { return m_options; }

private:
	friend class DiagnosticsItemSet;
	//DiagnosticsItem();
	//PathName	m_filePath;		// できるだけ DiagnosticsManager から直接代入するようにし、メモリを共有できるようにする
	DiagnosticsCode	m_code;
	int				m_lineNumber;
	int				m_columnNumber;
	StringArrayA	m_options;
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
	void Report(DiagnosticsCode code, StringA option1);

	const Array<DiagnosticsItem>* GetItems() { return &m_items; }

private:
	PathName	m_filePath;
	int			m_currentLineNumber;
	int			m_currentColumnNumber;
	Array<DiagnosticsItem>	m_items;
};


} // namespace Parser
LN_NAMESPACE_END

