
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
	RpnEvaluator_UnexpectedToken			= Severity_Error | RpnEvaluatorGroup | 2,	/**< �\�����Ȃ��g�[�N����������܂����B*/
	RpnEvaluator_InsufficientToken			= Severity_Error | RpnEvaluatorGroup | 3,	/**< ����]�����邽�߂̃g�[�N�����s�����Ă��܂��B(�����������Ă��Ȃ�) */
	RpnEvaluator_InvalidFuncCallArgsCount	= Severity_Error | RpnEvaluatorGroup | 4,	/**< �֐��Ăяo���̈���������܂���B*/
	RpnEvaluator_InvalidOperatorSide		= Severity_Error | RpnEvaluatorGroup | 5,	/**< ���Z�q�̕ӂɑ΂���I�y�����h������܂���B*/
	RpnEvaluator_InvalidNumericType			= Severity_Error | RpnEvaluatorGroup | 6,	/**< �����Ȑ��l�^�̃g�[�N�������͂���܂����B*/
	RpnEvaluator_OperatorInvalidType		= Severity_Error | RpnEvaluatorGroup | 7,	/**< ���Z�q�̌^�ɖ����Ȍ^({0})�̃I�y�����h���w�肳��Ă��܂��B*/
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
	int SetLineNumber(int line) { m_lineNumber = line; }		// 0�`
	int SetColumnNumber(int column) { m_columnNumber = column; }	// 0�`

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
	//PathName	m_filePath;		// �ł��邾�� DiagnosticsManager ���璼�ڑ������悤�ɂ��A�����������L�ł���悤�ɂ���
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
		�t�@�C���p�X�ȂǕ����̃G���[���������܂Ƃ߂����́B
		�t�@�C���p�X�p�̃������� Item ���ƂɎ������Ȃ��悤�ɂ���̂��ړI��1�B
		���񏈗�����Ƃ����A�G���[���|�[�g�̂��тɃ��b�N����̂͂�낵���Ȃ��B
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

