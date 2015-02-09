
#pragma once


namespace Lumino
{
namespace Parser
{

/**
	@brief
	@details	��ʂ̕�����g�[�N���̓��I�������m�ۂ�}���邽�߂Ɏg�p����B
				32 ����(\0�܂�)�܂ł͓��I�m�ۂ����A����𒴂����Ƃ��͓��I�m�ۂ���B
				�v���O��������̉�͂Ȃ炱�ꂾ���ł��قƂ�Ǘ}������͂��B
				���ӓ_�Ƃ��āA32 �����𒴂���g�[�N������ʂɏo�Ă���ꍇ��
				���̐ÓI 32 ���������ʂȗ̈�Ƃ��Ă������̂ŁA�������g�p�ʂ͒��ˏオ�邩������Ȃ��B
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


