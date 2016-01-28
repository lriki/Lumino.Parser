
#pragma once
#include "Common.h"
#include "TokenList.h"
#include "Frontend\Cpp\Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

// .cpp �� .h�A1�̉�͒P�ʂƂȂ�t�@�C��
class UnitFile
	: public RefObject
{
public:

	void Initialize(const TokenPathName& filePath);

	const TokenPathName& GetFilePath() const { return m_filePath; }


public:
	SourceRange SaveMacroReplacementTokens(const Token* begin, const Token* end);	// �L���b�V���ɕۑ�����ƍĔz�u�̉\��������̂ŁA�i�[���ꂽ�ꏊ�̓|�C���^�ł͂Ȃ��C���f�b�N�X�ŕԂ�
	void GetMacroReplacementTokens(const SourceRange& range, const Token** outBegin, const Token** outEnd) const;


LN_INTERNAL_ACCESS:
	TokenPathName		m_filePath;
	TokenListPtr		m_tokenList;

	uint64_t			m_inputMacroMapHash;
	RefPtr<MacroMap>	m_macroMap;		// ���̃t�@�C���̒��Œ�`���ꂽ�}�N���Bundef �� undef �Ƃ��ċL�^����
	Array<Token>		m_tokensCache;	// TODO: TokenList �ł������H
	TokenBuffer			m_tokenBuffer;

	// �}�N���̒u���v�f�͂����ƕۑ����Ă����Ȃ���΂Ȃ�Ȃ��B�u���ꏊ�͂��̃N���X����ԃC���[�W���₷���Ǝv���B
	// ����A���̃g�[�N�����X�g�͕ۑ�����K�v�͖����B�ǂ�Ȓ�`������̂�����������΂����B(�֐����͕K�v������ () �Ȃ񂩂͂���Ȃ�)
};

} // namespace Parser
LN_NAMESPACE_END

