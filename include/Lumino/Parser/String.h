
#pragma once


namespace Lumino
{
namespace Parser
{

/**
	@brief
	@details	大量の文字列トークンの動的メモリ確保を抑えるために使用する。
				32 文字(\0含む)までは動的確保せず、それを超えたときは動的確保する。
				プログラム言語の解析ならこれだけでもほとんど抑えられるはず。
				注意点として、32 文字を超えるトークンが大量に出てくる場合は
				この静的 32 文字が無駄な領域としてくっつくので、メモリ使用量は跳ね上がるかもしれない。
*/
template<typename TChar>
class String
{
public:
	static const int NormalLength = 32;

public:


private:
	TChar*	m_stringBegin;
	TChar*	m_stringEnd;
	TChar	m_staticBuffer[NormalLength];
};

} // namespace Parser
} // namespace Lumino


