
#pragma once

#include <Lumino/IO/PathName.h>
#include "../CppLexer.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class CppMacro
{
public:
	typename typedef BasicString<TChar>			StringT;
	typename typedef Token<TChar>				TokenT;
	typename typedef TokenList<TChar>			TokenListT;
	typename typedef TokenListT::const_iterator Position;

public:
	void AddParam(const TokenT& param) { m_params.Add(param); }
	void SetReplacementList(Position begin, Position end) { m_replacementList.Resize(end - begin); std::copy(begin, end, m_replacementList.begin()); }
	
private:
	StringT			m_name;		///< ��`��
	Array<TokenT>	m_params;
	TokenListT		m_replacementList;
};


template<typename TChar>
class CppMacroCollection
{
public:
	typename typedef CppMacro<TChar>	CppMacroT;

public:


private:
	/* �}�N�����Ƃ��Ďg���� ASCII �R�[�h�͈̔͂� 'A'(0x41) �` 'z'0x7A�B
	 * �r���� '[' ���̋L�����኱�܂ނ��A�S�ăJ�o�[�ł���͈͂͂��� 58 �����B
	 */
	static const int CodeTopMax = 58;
	static const int CodeEndMax = 58;
	static const int CodeLengthMax = 128;

	typedef std::vector<CppMacroT>	MacroList;

	MacroList***	m_macroListMap;		///< [�擪����][�I�[����][������] �Ń}�N�����O���[�v�����邽�߂̃e�[�u�� (128 �����܂ł̃}�N���͂��̃e�[�u���Ɋi�[�����)
	MacroList		m_macroListMapLong;	///< 128 �����𒴂��閼�O�������}�N�����X�g
};

//template<typename TChar>
//class CppPreprocessorManager
//{
//public:
//
//private:
//
//};

template<typename TChar>
class CppPreprocessor
{
public:
	typename typedef BasicString<TChar>			StringT;
	typename typedef Token<TChar>				TokenT;
	typename typedef TokenList<TChar>			TokenListT;
	typename typedef RefPtr<TokenListT>			TokenListPtr;
	typename typedef TokenListT::const_iterator Position;
	typename typedef BasicPathName<TChar>		PathNameT;

public:

	void Analyze(TokenListT& tokenList, ErrorManager* errorManager);

private:
	bool AnalyzeLine(Position lineHead, Position* outLineEnd);
	bool AnalyzeDefine(Position pos, Position* outLineEnd);
	Position SkipGenericSpace(Position pos);
	Position GetNextGenericToken(Position pos);
	bool IsGenericSpace(const TokenT& token);

private:
	ErrorManager*	m_errorInfo;
	TokenListT		m_workTokenList;
};

} // namespace Parser
} // namespace Lumino
