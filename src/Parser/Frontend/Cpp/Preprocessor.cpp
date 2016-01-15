
/*
	preprocessing-file:
		groupopt
	group:
		group-part
		group group-part
	group-part:
		if-section
		control-line
		text-line
		# non-directive
	if-section:
		if-group elif-groupsopt else-groupopt endif-line
	if-group:
		# if constant-expression new-line groupopt
		# ifdef identifier new-line groupopt
		# ifndef identifier new-line groupopt
	elif-groups:
		elif-group
		elif-groups elif-group
	elif-group:
		# elif constant-expression new-line groupopt
	else-group:
		# else new-line groupopt
	endif-line:
		# endif new-line
	control-line:
		# include pp-tokens new-line
		# define identifier replacement-list new-line
		# define identifier lparen identifier-listopt) replacement-list new-line
		# define identifier lparen ... ) replacement-list new-line
		# define identifier lparen identifier-list, ... ) replacement-list new-line
		# undef identifier new-line
		# line pp-tokens new-line
		# error pp-tokensopt new-line
		# pragma pp-tokensopt new-line
		# new-line
	text-line:
		pp-tokensopt new-line

	non-directive:
		pp-tokens new-line
	lparen:
		a ( character not immediately preceded by white-space
	identifier-list:
		identifier
		identifier-list , identifier
	replacement-list:
		pp-tokensopt
	pp-tokens:
		preprocessing-token
		pp-tokens preprocessing-token
	new-line:
		the new-line character
*/
#include "../../Internal.h"
#include "../../DiagnosticsManager.h"
#include "Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

//=============================================================================
// MacroMap
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
MacroEntity* MacroMap::Insert(const Token& name, const TokenChar* replacementBegin, const TokenChar* replacementEnd)
{
	MacroEntity macro;
	macro.name = name.ToString();
	macro.replacementContentString = TokenString(replacementBegin, replacementEnd - replacementBegin);

	m_allMacroList.Add(macro);
	MacroEntity* m = &m_allMacroList.GetLast();
	m_macroMap.Insert(name.GetBegin(), name.GetEnd(), m);
	return m;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
MacroEntity* MacroMap::Find(const Token& name)
{

}

//=============================================================================
// Preprocessor
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Preprocessor::Preprocessor()
	: m_tokenList(nullptr)
	, m_fileCache(nullptr)
	, m_seqDirective(DirectiveSec::Idle)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState Preprocessor::BuildPreprocessedTokenList(TokenList* tokenList, PreprocessedFileCacheItem* outFileCache, DiagnosticsItemSet* diag)
{
	m_tokenList = tokenList;
	m_fileCache = outFileCache;
	m_diag = diag;
	m_seqDirective = DirectiveSec::LineHead;

	int tokenCount = m_tokenList->GetCount();
	for (int iToken = 0; iToken < tokenCount; ++iToken)
	{
		Token& token = m_tokenList->GetAt(iToken);

		// �������Ă��Ȃ��B���s��T���B
		if (m_seqDirective == DirectiveSec::Idle)
		{
			if (token.GetCommonType() == CommonTokenType::NewLine)
			{
				m_seqDirective = DirectiveSec::LineHead;		// ���s�����������B�s����Ԃ�
			}
		}
		// �s���ɂ���B# ��T���B
		else if (m_seqDirective == DirectiveSec::LineHead)
		{
			if (token.GetCommonType() == CommonTokenType::Operator &&
				token.EqualString("#", 1))
			{
				m_seqDirective = DirectiveSec::FindIdent;	// "#" ���������B���ʎq��T��
			}
			else {
				m_seqDirective = DirectiveSec::Idle;		// "#" �ȊO�̃g�[�N���������BIdle �ցB
			}
		}
		// ���ʎq��T���Ă���B
		else if (m_seqDirective == DirectiveSec::FindIdent)
		{
			if (token.IsSpaceOrComment())
			{
				// ���s
			}
			else if (token.GetCommonType() == CommonTokenType::Identifier)
			{
				// ������
				m_preproLineHead = &token;
				m_seqDirective = DirectiveSec::FindLineEnd;	// �s����T���ɍs��
			}
			else
			{
				// Error: # �̎��ɂȂ񂩕ςȃg�[�N��������
				m_diag->Report(DiagnosticsCode::Preprocessor_UnexpectedDirectiveToken, token.ToString());
				return ResultState::Error;
			}
		}
		// �s����T���Ă���B
		else if (m_seqDirective == DirectiveSec::FindLineEnd)
		{
			if (token.GetCommonType() == CommonTokenType::NewLine ||
				token.GetCommonType() == CommonTokenType::Eof)
			{
				LN_RESULT_CALL(PollingDirectiveLine(m_preproLineHead, &token));
				m_seqDirective = DirectiveSec::Idle;
			}
		}
	}
	return ResultState::Success;
}

//-----------------------------------------------------------------------------
// lineBegin �� ���ʎq���w���Ă���B#include �Ȃ� include�B
// lineEnd �� NewLine �� Eof ���w���Ă���B
//-----------------------------------------------------------------------------
ResultState Preprocessor::PollingDirectiveLine(Token* lineBegin, Token* lineEnd)
{
	if (lineBegin->EqualString("define", 6))
	{
		// ���̎��ʎq�܂Ői�߂�
		++lineBegin;
		while (lineBegin < lineEnd && lineBegin->IsSpaceOrComment()) ++lineBegin;	// �X�y�[�X���΂�
		if (lineBegin->GetCommonType() != CommonTokenType::Identifier)
		{
			// Error: ���ʎq�ł͂Ȃ�����
			m_diag->Report(DiagnosticsCode::Preprocessor_SyntaxError);
			return ResultState::Error;
		}

		// ���ʎq�����o���Ă���
		Token* ident = lineBegin;

		// �X�y�[�X���΂�
		++lineBegin;
		while (lineBegin < lineEnd && lineBegin->IsSpaceOrComment()) ++lineBegin;

		// end �̓X�y�[�X���ł͂Ȃ��Ȃ�܂Ŗ߂�
		--lineEnd;
		while (lineBegin < lineEnd && lineEnd->IsSpaceOrComment()) --lineEnd;

		// �}�N���o�^
		m_fileCache->outputMacroMap.Insert(*ident, lineBegin->GetBegin(), lineEnd->GetEnd());
	}
	return ResultState::Success;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//TokenList::iterator Preprocessor::GetNextGenericToken(TokenList::iterator pos)
//{
//	while (!pos->IsEof())
//	{
//		++pos;
//		if (pos->GetCommonType() != CommonTokenType::SpaceSequence &&
//			pos->GetCommonType() != CommonTokenType::Comment)
//		{
//			return pos;
//		}
//	}
//	return pos;
//}

} // namespace Parser
LN_NAMESPACE_END

