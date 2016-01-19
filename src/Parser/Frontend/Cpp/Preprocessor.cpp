
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

	......
	sizeof
	alignof
	defined ( ���ʎq )

	__STDC__


	-----------------------------------
	JISX3014 �v���O��������C++

	16.1
	# �`�L�[���[�h�O�̊Ԃ� �󔒂Ɛ����^�u�̂݉A�����ǂƂ肠�����u�󔒗ށv�Ƃ��Ă���B

	16.1��(136)
	����萔�����̑S�Ă̎��ʎq�̓}�N�����ł��邩�ۂ��̂����ꂩ�ƂȂ�B
	���Ȃ킿�A�L�[���[�h�A�񋓒萔�Ȃǂ͂܂����݂��Ȃ��B
	��
	�L���X�g���s�\�B
*/
#include "../../Internal.h"
#include "../../DiagnosticsManager.h"
#include "../../ParserUtils.h"
#include "CppLexer.h"
#include "Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

static const TokenChar* ConstToken_0_Buf = "0";
static const Token ConstToken_0(CommonTokenType::ArithmeticLiteral, ConstToken_0_Buf, ConstToken_0_Buf + 1, TT_NumericLitaralType_Int32);

static const TokenChar* ConstToken_1_Buf = "1";
static const Token ConstToken_1(CommonTokenType::ArithmeticLiteral, ConstToken_1_Buf, ConstToken_1_Buf + 1, TT_NumericLitaralType_Int32);

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
	MacroEntity* e;
	if (m_macroMap.Find(name.GetBegin(), name.GetEnd(), &e, CaseSensitivity::CaseSensitive))	// TODO: �啶��������
		return e;
	else
		return nullptr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool MacroMap::IsDefined(const Token& name)
{
	MacroEntity* e = Find(name);
	if (e != nullptr)
	{
		return !e->undef;
	}
	return false;
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
	m_conditionalSectionStack.Clear();

	int tokenCount = m_tokenList->GetCount();
	for (int iToken = 0; iToken < tokenCount; ++iToken)
	{
		Token& token = m_tokenList->GetAt(iToken);

		// �v���v���f�B���N�e�B�u�������łȂ���΁A�g�[�N���� valid �͌��݂̏�Ԃɏ]���A�������͑S�Ė��������ɂ���
		if (m_seqDirective == DirectiveSec::Idle ||
			m_seqDirective == DirectiveSec::LineHead)
		{
			token.SetValid(IsInValidSection());
		}
		else
		{
			token.SetValid(false);
		}

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
				token.SetValid(false);						// �f�B���N�e�B�u�̉�͂��n�܂�^�C�~���O�Ȃ̂ŁA��������g�[�N���𖳌��ɂ��n�߂�
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
			else if (
				token.GetCommonType() == CommonTokenType::Identifier ||
				token.GetCommonType() == CommonTokenType::Keyword)		// #else �̓L�[���[�h�Ŕ��ł���
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

				if (token.GetCommonType() == CommonTokenType::NewLine) {
					m_seqDirective = DirectiveSec::LineHead;	// ���s�Ȃ̂ōs����Ԃ�
				}
				else {
					m_seqDirective = DirectiveSec::Idle;
				}
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
	//---------------------------------------------------------
	// #define
	//		:: # define identifier replacement-list new-line
	//		:: # define identifier lparen identifier-listopt) replacement-list new-line
	//		:: # define identifier lparen ... ) replacement-list new-line
	//		:: # define identifier lparen identifier-list, ... ) replacement-list new-line
	if (lineBegin->EqualString("define", 6))
	{
		// ���̎��ʎq�܂Ői�߂�
		lineBegin = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);
		if (lineBegin->GetCommonType() != CommonTokenType::Identifier)
		{
			// Error: ���ʎq�ł͂Ȃ�����
			m_diag->Report(DiagnosticsCode::Preprocessor_SyntaxError);
			return ResultState::Error;
		}

		// ���ʎq�����o���Ă���
		Token* ident = lineBegin;

		// �X�y�[�X���΂�
		lineBegin = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);

		// end �̓X�y�[�X���ł͂Ȃ��Ȃ�܂Ŗ߂�
		lineEnd = ParserUtils::SkipPrevSpaceOrComment(lineBegin, lineEnd);

		// �}�N���o�^
		// TODO: �}�N���̏㏑���m�F
		m_fileCache->outputMacroMap.Insert(*ident, lineBegin->GetBegin(), lineEnd->GetEnd());
	}
	//---------------------------------------------------------
	// #if
	//		:: # if constant-expression new-line groupopt
	else if (lineBegin->EqualString("if", 2))
	{
		// �V�����Z�N�V�������J�n����
		m_conditionalSectionStack.Push(ConditionalSection());

		// �X�y�[�X���΂�
		Token* pos = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);

		// Error: �萔������������
		LN_DIAG_REPORT_ERROR(pos < lineEnd, DiagnosticsCode::Preprocessor_InvalidConstantExpression);

		// �萔�����̃}�N����W�J���� RpnParser �Ɋ|���邽�߂̃g�[�N�����X�g�����
		m_preproExprTokenList.Clear();
		m_preproExprTokenList.Reserve(lineEnd - pos);		// �}�N���W�J�ő����邱�Ƃ͂��邪�A�Ƃ肠�������ꂾ�����炩���ߊm�ۂ��Ă���
		for (; pos < lineEnd; )
		{
			// defined �Ȃ炻�̏�����
			if (pos->GetCommonType() == CommonTokenType::Identifier &&
				pos->EqualString("defined", 7))
			{
				Token* ident = nullptr;

				// �X�y�[�X���΂�
				pos = ParserUtils::SkipNextSpaceOrComment(pos, lineEnd);
				if (pos->GetCommonType() == CommonTokenType::Identifier)
				{
					// ���ʎq�������B"#if defined AAA" �̂悤�Ȍ`���B
					ident = pos;
				}
				else if (pos->GetCommonType() == CommonTokenType::Operator && pos->EqualChar('('))
				{
					// ( �������B����ɔ�΂��Ǝ��ʎq�A�����ЂƂ�΂��� ')'
					ident = ParserUtils::SkipNextSpaceOrComment(pos, lineEnd);
					LN_DIAG_REPORT_ERROR(ident->GetCommonType() == CommonTokenType::Identifier, DiagnosticsCode::Preprocessor_ExpectedDefinedId);
					Token* paren = ParserUtils::SkipNextSpaceOrComment(ident, lineEnd);
					LN_DIAG_REPORT_ERROR(paren->GetCommonType() == CommonTokenType::Operator && paren->EqualChar(')'), DiagnosticsCode::Preprocessor_ExpectedDefinedId);
					++pos;
					++pos;
				}
				else
				{
					// Error: defined �̌�Ɏ��ʎq���K�v
					LN_DIAG_REPORT_ERROR(0, DiagnosticsCode::Preprocessor_ExpectedDefinedId);
				}

				// �}�N����T���B
				if (m_fileCache->outputMacroMap.IsDefined(*ident)) {
					m_preproExprTokenList.Add(ConstToken_1);	// "1" �ɓW�J
				}
				else {
					m_preproExprTokenList.Add(ConstToken_0);	// "0" �ɓW�J
				}
				++pos;
			}
			else
			{
				m_preproExprTokenList.Add(*pos);
				++pos;
			}
		}

		// �萔����]������
		LN_RESULT_CALL(m_rpnParser.ParseCppConstExpression2(m_preproExprTokenList.cbegin(), m_preproExprTokenList.cend(), m_diag));
		RpnOperand result;
		LN_RESULT_CALL(m_rpnEvaluator.TryEval(m_rpnParser.GetTokenList(), m_diag, &result));

		// �����^�� bool ������
		if (result.IsIntager() || result.type == RpnOperandType::Boolean)
		{
			if (result.IsFuzzyTrue()) {	// 0 �ȊO�܂��� true
				m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Valid;
			}
			else {
				m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Invalid;
			}
		}
		else
		{
			// Error: �����萔�����K�v�ł�
			LN_DIAG_REPORT_ERROR(pos < lineEnd, DiagnosticsCode::Preprocessor_InvalidConstantExpression);
		}
	}
	//---------------------------------------------------------
	// #ifdef
	//		:: # ifdef identifier new-line groupopt
	else if (lineBegin->EqualString("ifdef", 5))
	{
		// �V�����Z�N�V�������J�n����
		m_conditionalSectionStack.Push(ConditionalSection());

		// ���̎��ʎq�܂Ői�߂�
		lineBegin = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);
		// Error: ���ʎq�ł͂Ȃ�����
		LN_DIAG_REPORT_ERROR(lineBegin->GetCommonType() == CommonTokenType::Identifier, DiagnosticsCode::Preprocessor_SyntaxError);

		// �����_�Ń}�N������`����Ă��邩�`�F�b�N
		if (m_fileCache->outputMacroMap.IsDefined(*lineBegin))
		{
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Valid;
		}
		else
		{
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Invalid;
		}
	}
	//---------------------------------------------------------
	// #else
	//		::	# else new-line groupopt
	else if (lineBegin->EqualString("else", 4))
	{
		if (m_conditionalSectionStack.IsEmpty() ||				// #if ���Ȃ�
			m_conditionalSectionStack.GetTop().elseProcessed)	// ���� #else ��̍ς�
		{
			// Error: �\�����Ȃ� #else
			m_diag->Report(DiagnosticsCode::Preprocessor_UnexpectedElse);
			return ResultState::Error;
		}

		if (m_conditionalSectionStack.GetTop().state == ConditionalSectionState::Valid)
		{
			// �L���̈�̂��Ƃ� #else �Ȃ̂ŁA��͉������낤�ƑS�Ė����̈�ƂȂ�
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Skip;
		}
		else if (m_conditionalSectionStack.GetTop().state == ConditionalSectionState::Invalid)
		{
			// ���܂܂Ŗ����������̂ł�������L���ɂȂ�
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Valid;
		}
		else
		{
			// Skip �̂܂܈ێ�����
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Skip;
		}

		// else ����������
		m_conditionalSectionStack.GetTop().elseProcessed = true;
	}
	//---------------------------------------------------------
	// #endif
	//		::	# endif new-line
	else if (lineBegin->EqualString("endif", 5))
	{
		if (m_conditionalSectionStack.IsEmpty())
		{
			// Error: �\�����Ȃ� #endif
			m_diag->Report(DiagnosticsCode::Preprocessor_UnexpectedEndif);
			return ResultState::Error;
		}
		m_conditionalSectionStack.Pop();
	}

	return ResultState::Success;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Preprocessor::IsInValidSection() const
{
	if (m_conditionalSectionStack.IsEmpty()) {
		return true;
	}
	else if (m_conditionalSectionStack.GetTop().state == ConditionalSectionState::Valid) {
		return true;
	}
	return false;
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

