
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
	/// ���̃g�[�N�����X�g���ێ����邷�ׂẴg�[�N������������̎Q�Ƃ�؂�A�����������ɃR�s�[����B
	/// ���̌�A�\�[�X�o�b�t�@�͊J�����邱�Ƃ��ł���B
	void CloneTokenStrings()
	{
		LN_FOREACH(TokenT& t, m_list)
		{
			t.CloneTokenStrings();
		}
	}

	/// ���ׂẴg�[�N�����e�L�X�g�����ďo�͂���
	void DumpText(Stream* stream)
	{
		LN_FOREACH(TokenT& t, m_list)
		{
			if (!t.IsEOF()) {	// EOF �͏o���Ȃ�
				stream->Write(t.GetTokenBegin(), t.GetLength());
			}
		}
	}

private:
	Array<TokenT>	m_list;
};


} // namespace Parser
} // namespace Lumino

