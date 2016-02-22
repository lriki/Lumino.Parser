
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
	Array<TokenPathName>	m_additionalIncludePaths;	// �ǉ��̃C���N���[�h�f�B���N�g��
	RefPtr<MacroMap>		m_preprocessorDefinitions;	// ��`�ς݃}�N�� (��͊J�n���� MacroMap)
	

	//CompileOptions*	m_parent;
};

class Context
	: public RefObject
{
public:
	// TODO: �R���p�C���I�v�V�����̃C���N���[�h�t�@�C���̌����p�X�̕��т���v���Ă���K�v������B
	/*
		�����t�@�C�����ق����Ƃ��͊�{�I�ɂ��̊֐���ʂ��B
		.c �Ȃǂ̃R���p�C���P�ʂƂȂ�t�@�C���́A�R���p�C���I�v�V�����ŃL�[���w��B
		.h �Ȃǂ̃C���N���[�h�t�@�C����#include���_�̃}�N���ŃL�[���w��B
	*/
	UnitFile* LookupUnitFile(const PathName& fileAbsPath, uint64_t macroMapHash);

	// FontendContext/AnalayzerContext �������ق��������H
};

} // namespace Parser
LN_NAMESPACE_END

