
#pragma once

#include <Lumino/Base/RefObject.h>
#include <Lumino/IO/Stream.h>
#include "Token.h"

namespace Lumino
{
namespace Parser
{

template<typename TChar>
class TokenList : public RefObject
{
public:
	typedef typename Token<TChar>	TokenT;
	typedef typename Array< Token<TChar> >::iterator iterator;
	typedef typename Array< Token<TChar> >::const_iterator const_iterator;

public:
	TokenList() {}
	~TokenList() {}

public:
	void Reserve(size_t size) { m_list.reserve(size); }
	void Add(const Token<TChar>& token) { m_list.push_back(token); }
	TokenT& GetLast() { return *(m_list.rbegin()); }
	const TokenT& GetLast() const { return *(m_list.rbegin()); }

	size_t size() { return m_list.size(); }
	iterator begin() { return m_list.begin(); }
	iterator end() { return m_list.end(); }
	iterator insert(iterator pos, iterator begin, iterator end) { return m_list.insert(pos, begin, end); }
	iterator insert(const_iterator pos, const_iterator begin, const_iterator end) { return m_list.insert(pos, begin, end); }
	iterator erase(iterator begin, iterator end) { return m_list.erase(begin, end); }
	iterator erase(const_iterator begin, const_iterator end) { return m_list.erase(begin, end); }

public:
	/// このトークンリストが保持するすべてのトークンが持つ文字列の参照を切り、内部メモリにコピーする。
	/// この後、ソースバッファは開放することができる。
	void CloneTokenStrings()
	{
		LN_FOREACH(TokenT& t, m_list)
		{
			t.CloneTokenStrings();
		}
	}

	/// すべてのトークンをテキスト化して出力する
	void DumpText(Stream* stream)
	{
		LN_FOREACH(TokenT& t, m_list)
		{
			if (!t.IsEOF()) {	// EOF は出さない
				stream->Write(t.GetTokenBegin(), t.GetLength());
			}
		}
	}

private:
	Array<TokenT>	m_list;
};


} // namespace Parser
} // namespace Lumino

