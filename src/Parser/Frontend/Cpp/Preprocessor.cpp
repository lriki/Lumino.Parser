
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
	defined ( 識別子 )

	__STDC__


	-----------------------------------
	JISX3014 プログラム言語C++

	16.1
	# ～キーワード前の間は 空白と水平タブのみ可、だけどとりあえず「空白類」としている。

	16.1注(136)
	制御定数式内の全ての識別子はマクロ名であるか否かのいずれかとなる。
	すなわち、キーワード、列挙定数などはまだ存在しない。
	↓
	キャスト式不可能。
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
	if (m_macroMap.Find(name.GetBegin(), name.GetEnd(), &e, CaseSensitivity::CaseSensitive))	// TODO: 大文字小文字
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

		// プリプロディレクティブ処理中でなければ、トークンの valid は現在の状態に従い、処理中は全て無効扱いにする
		if (m_seqDirective == DirectiveSec::Idle ||
			m_seqDirective == DirectiveSec::LineHead)
		{
			token.SetValid(IsInValidSection());
		}
		else
		{
			token.SetValid(false);
		}

		// 何もしていない。改行を探す。
		if (m_seqDirective == DirectiveSec::Idle)
		{
			if (token.GetCommonType() == CommonTokenType::NewLine)
			{
				m_seqDirective = DirectiveSec::LineHead;		// 改行が見つかった。行頭状態へ
			}
		}
		// 行頭にいる。# を探す。
		else if (m_seqDirective == DirectiveSec::LineHead)
		{
			if (token.GetCommonType() == CommonTokenType::Operator &&
				token.EqualString("#", 1))
			{
				m_seqDirective = DirectiveSec::FindIdent;	// "#" を見つけた。識別子を探す
				token.SetValid(false);						// ディレクティブの解析が始まるタイミングなので、ここからトークンを無効にし始める
			}
			else {
				m_seqDirective = DirectiveSec::Idle;		// "#" 以外のトークンだった。Idle へ。
			}
		}
		// 識別子を探している。
		else if (m_seqDirective == DirectiveSec::FindIdent)
		{
			if (token.IsSpaceOrComment())
			{
				// 続行
			}
			else if (
				token.GetCommonType() == CommonTokenType::Identifier ||
				token.GetCommonType() == CommonTokenType::Keyword)		// #else はキーワードで飛んでくる
			{
				// 見つけた
				m_preproLineHead = &token;
				m_seqDirective = DirectiveSec::FindLineEnd;	// 行末を探しに行く
			}
			else
			{
				// Error: # の次になんか変なトークンが来た
				m_diag->Report(DiagnosticsCode::Preprocessor_UnexpectedDirectiveToken, token.ToString());
				return ResultState::Error;
			}
		}
		// 行末を探している。
		else if (m_seqDirective == DirectiveSec::FindLineEnd)
		{
			if (token.GetCommonType() == CommonTokenType::NewLine ||
				token.GetCommonType() == CommonTokenType::Eof)
			{
				LN_RESULT_CALL(PollingDirectiveLine(m_preproLineHead, &token));

				if (token.GetCommonType() == CommonTokenType::NewLine) {
					m_seqDirective = DirectiveSec::LineHead;	// 改行なので行頭状態へ
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
// lineBegin は 識別子を指している。#include なら include。
// lineEnd は NewLine か Eof を指している。
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
		// 次の識別子まで進める
		lineBegin = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);
		if (lineBegin->GetCommonType() != CommonTokenType::Identifier)
		{
			// Error: 識別子ではなかった
			m_diag->Report(DiagnosticsCode::Preprocessor_SyntaxError);
			return ResultState::Error;
		}

		// 識別子を取り出しておく
		Token* ident = lineBegin;

		// スペースを飛ばす
		lineBegin = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);

		// end はスペースがではなくなるまで戻す
		lineEnd = ParserUtils::SkipPrevSpaceOrComment(lineBegin, lineEnd);

		// マクロ登録
		// TODO: マクロの上書き確認
		m_fileCache->outputMacroMap.Insert(*ident, lineBegin->GetBegin(), lineEnd->GetEnd());
	}
	//---------------------------------------------------------
	// #if
	//		:: # if constant-expression new-line groupopt
	else if (lineBegin->EqualString("if", 2))
	{
		// 新しいセクションを開始する
		m_conditionalSectionStack.Push(ConditionalSection());

		// スペースを飛ばす
		Token* pos = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);

		// Error: 定数式が無かった
		LN_DIAG_REPORT_ERROR(pos < lineEnd, DiagnosticsCode::Preprocessor_InvalidConstantExpression);

		// 定数式内のマクロを展開して RpnParser に掛けるためのトークンリストを作る
		m_preproExprTokenList.Clear();
		m_preproExprTokenList.Reserve(lineEnd - pos);		// マクロ展開で増えることはあるが、とりあえずこれだけあらかじめ確保しておく
		for (; pos < lineEnd; )
		{
			// defined ならその処理へ
			if (pos->GetCommonType() == CommonTokenType::Identifier &&
				pos->EqualString("defined", 7))
			{
				Token* ident = nullptr;

				// スペースを飛ばす
				pos = ParserUtils::SkipNextSpaceOrComment(pos, lineEnd);
				if (pos->GetCommonType() == CommonTokenType::Identifier)
				{
					// 識別子だった。"#if defined AAA" のような形式。
					ident = pos;
				}
				else if (pos->GetCommonType() == CommonTokenType::Operator && pos->EqualChar('('))
				{
					// ( だった。さらに飛ばすと識別子、もうひとつ飛ばすと ')'
					ident = ParserUtils::SkipNextSpaceOrComment(pos, lineEnd);
					LN_DIAG_REPORT_ERROR(ident->GetCommonType() == CommonTokenType::Identifier, DiagnosticsCode::Preprocessor_ExpectedDefinedId);
					Token* paren = ParserUtils::SkipNextSpaceOrComment(ident, lineEnd);
					LN_DIAG_REPORT_ERROR(paren->GetCommonType() == CommonTokenType::Operator && paren->EqualChar(')'), DiagnosticsCode::Preprocessor_ExpectedDefinedId);
					++pos;
					++pos;
				}
				else
				{
					// Error: defined の後に識別子が必要
					LN_DIAG_REPORT_ERROR(0, DiagnosticsCode::Preprocessor_ExpectedDefinedId);
				}

				// マクロを探す。
				if (m_fileCache->outputMacroMap.IsDefined(*ident)) {
					m_preproExprTokenList.Add(ConstToken_1);	// "1" に展開
				}
				else {
					m_preproExprTokenList.Add(ConstToken_0);	// "0" に展開
				}
				++pos;
			}
			else
			{
				m_preproExprTokenList.Add(*pos);
				++pos;
			}
		}

		// 定数式を評価する
		LN_RESULT_CALL(m_rpnParser.ParseCppConstExpression2(m_preproExprTokenList.cbegin(), m_preproExprTokenList.cend(), m_diag));
		RpnOperand result;
		LN_RESULT_CALL(m_rpnEvaluator.TryEval(m_rpnParser.GetTokenList(), m_diag, &result));

		// 整数型と bool を許可
		if (result.IsIntager() || result.type == RpnOperandType::Boolean)
		{
			if (result.IsFuzzyTrue()) {	// 0 以外または true
				m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Valid;
			}
			else {
				m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Invalid;
			}
		}
		else
		{
			// Error: 整数定数式が必要です
			LN_DIAG_REPORT_ERROR(pos < lineEnd, DiagnosticsCode::Preprocessor_InvalidConstantExpression);
		}
	}
	//---------------------------------------------------------
	// #ifdef
	//		:: # ifdef identifier new-line groupopt
	else if (lineBegin->EqualString("ifdef", 5))
	{
		// 新しいセクションを開始する
		m_conditionalSectionStack.Push(ConditionalSection());

		// 次の識別子まで進める
		lineBegin = ParserUtils::SkipNextSpaceOrComment(lineBegin, lineEnd);
		// Error: 識別子ではなかった
		LN_DIAG_REPORT_ERROR(lineBegin->GetCommonType() == CommonTokenType::Identifier, DiagnosticsCode::Preprocessor_SyntaxError);

		// 現時点でマクロが定義されているかチェック
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
		if (m_conditionalSectionStack.IsEmpty() ||				// #if がない
			m_conditionalSectionStack.GetTop().elseProcessed)	// 既に #else 受領済み
		{
			// Error: 予期しない #else
			m_diag->Report(DiagnosticsCode::Preprocessor_UnexpectedElse);
			return ResultState::Error;
		}

		if (m_conditionalSectionStack.GetTop().state == ConditionalSectionState::Valid)
		{
			// 有効領域のあとの #else なので、後は何があろうと全て無効領域となる
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Skip;
		}
		else if (m_conditionalSectionStack.GetTop().state == ConditionalSectionState::Invalid)
		{
			// いままで無効だったのでここから有効になる
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Valid;
		}
		else
		{
			// Skip のまま維持する
			m_conditionalSectionStack.GetTop().state = ConditionalSectionState::Skip;
		}

		// else を処理した
		m_conditionalSectionStack.GetTop().elseProcessed = true;
	}
	//---------------------------------------------------------
	// #endif
	//		::	# endif new-line
	else if (lineBegin->EqualString("endif", 5))
	{
		if (m_conditionalSectionStack.IsEmpty())
		{
			// Error: 予期しない #endif
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

