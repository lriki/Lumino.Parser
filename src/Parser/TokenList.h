
#pragma once
#include "Token.h"

LN_NAMESPACE_BEGIN
namespace parser
{
	
/**
	@brief	
*/
class TokenList
	: public RefObject
{
public:
	TokenList() {}
	~TokenList() {}

	void Reserve(size_t size) { m_list.Reserve(size); }
	void Resize(size_t size) { m_list.Resize(size); }
	void Add(const Token& token) { m_list.Add(token); }
	Token& GetAt(int index) { return m_list.GetAt(index); }
	int GetCount() const { return m_list.GetCount(); }

private:
	Array<Token>	m_list;
};

} // namespace Parser
LN_NAMESPACE_END

