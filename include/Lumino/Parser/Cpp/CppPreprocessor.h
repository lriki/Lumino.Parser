
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
	StringT			m_name;		///< 定義名
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
	/* マクロ名として使える ASCII コードの範囲は 'A'(0x41) ～ 'z'0x7A。
	 * 途中に '[' 等の記号も若干含むが、全てカバーできる範囲はこの 58 文字。
	 */
	static const int CodeTopMax = 58;
	static const int CodeEndMax = 58;
	static const int CodeLengthMax = 128;

	typedef std::vector<CppMacroT>	MacroList;

	MacroList***	m_macroListMap;		///< [先頭文字][終端文字][文字数] でマクロをグループ化するためのテーブル (128 文字までのマクロはこのテーブルに格納される)
	MacroList		m_macroListMapLong;	///< 128 文字を超える名前名を持つマクロリスト
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
