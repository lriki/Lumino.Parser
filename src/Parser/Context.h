
#pragma once
#include "Common.h"
#include "Frontend\Cpp\Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

class CompileOptions
{
public:


private:
	Array<TokenPathName>	m_additionalIncludePaths;	// 追加のインクルードディレクトリ
	RefPtr<MacroMap>		m_preprocessorDefinitions;	// 定義済みマクロ (解析開始時の MacroMap)
	

	//CompileOptions*	m_parent;
};

class Context
	: public RefObject
{
public:
	// TODO: コンパイラオプションのインクルードファイルの検索パスの並びも一致している必要がある。
	/*
		何かファイルがほしいときは基本的にこの関数を通す。
		.c などのコンパイル単位となるファイルは、コンパイルオプションでキーを指定。
		.h などのインクルードファイルは#include時点のマクロでキーを指定。
	*/
	UnitFile* LookupUnitFile(const PathName& fileAbsPath, uint64_t macroMapHash);

	// FontendContext/AnalayzerContext 分けたほうがいい？
};

} // namespace Parser
LN_NAMESPACE_END

