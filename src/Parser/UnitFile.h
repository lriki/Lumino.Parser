
#pragma once
#include "Common.h"
#include "TokenList.h"
#include "Frontend\Cpp\Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

// .cpp や .h、1つの解析単位となるファイル
class UnitFile
	: public RefObject
{
public:

	void Initialize(const TokenPathName& filePath);

	const TokenPathName& GetFilePath() const { return m_filePath; }


public:
	SourceRange SaveMacroReplacementTokens(const Token* begin, const Token* end);	// キャッシュに保存すると再配置の可能性があるので、格納された場所はポインタではなくインデックスで返す
	void GetMacroReplacementTokens(const SourceRange& range, const Token** outBegin, const Token** outEnd) const;


LN_INTERNAL_ACCESS:
	TokenPathName		m_filePath;
	TokenListPtr		m_tokenList;

	uint64_t			m_inputMacroMapHash;
	RefPtr<MacroMap>	m_macroMap;		// このファイルの中で定義されたマクロ。undef は undef として記録する
	Array<Token>		m_tokensCache;	// TODO: TokenList でもいい？
	TokenBuffer			m_tokenBuffer;

	// マクロの置換要素はずっと保存しておかなければならない。置き場所はこのクラスが一番イメージしやすいと思う。
	// 一方、元のトークンリストは保存する必要は無い。どんな定義があるのかだけ分かればいい。(関数名は必要だけど () なんかはいらない)
};

} // namespace Parser
LN_NAMESPACE_END

