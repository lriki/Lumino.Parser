
#include <Lumino/Parser/CppLexer.h>
#include <Lumino/Parser/RPN/RPNParser.h>

namespace Lumino
{
namespace Parser
{

//template<typename TChar>
//class RPNTokenizer
//	: public ParserObject<TChar>
//{
//public:
//	typename typedef RPNElement<TChar>	RPNElementT;
//	typename typedef Array<RPNElementT>	RPNElementListT;
//
//public:
//	static void Tokenize(Position exprBegin, Position exprEnd, RPNElementListT* outElementList);
//};

/*
	C++ の演算子と優先順位
	http://ja.cppreference.com/w/cpp/language/operator_precedence
	1+2+3	→ (1+2)+3	… 左結合 (Left-to-right)
	a=b=1	→ a=(b=1)	… 右結合 (Right-to-left)
*/

/*
	条件演算子の解析

		1 ? 6 ? 7 : 8 : 3 ? 4 : 5

	という式は、Parse では以下のように展開できれば良い。

		idx   >  0  1  2  3  4  5  6  7  8  9 10 11 12
		token >  1  ?  6  ?  7  :  8  :  3  ?  4  :  5
		goto  >     8     6    13    13    12    13

	Eval では、
	・トークンは左から右へ読んでいく。
	・? の goto は、条件式が false だった場合にジャンプする idx を示す。
	・: の goto は、ここにたどり着いたときにジャンプする idx を示す。(一律、式の終端(※)である)
	※「式の終端」とは、')' またはバッファ終端である。

	?: 以下の優先度の演算子は代入系とthrow、, だが、これらを考慮しない場合は
	たとえどれだけ : で区切られようとも : でジャンプするのは式の終端である。

*/


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
ArrayList< RPNToken<TChar> >* RPNParser<TChar>::ParseCppConstExpression(Position exprBegin, Position exprEnd, ErrorManager* errorInfo)
{
	RPNParser<TChar> parser;
	parser.TokenizeCppConst(exprBegin, exprEnd);
	parser.Parse();
	parser.m_rpnTokenList.SafeAddRef();
	return parser.m_rpnTokenList;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::TokenizeCppConst(Position exprBegin, Position exprEnd)
{
	// とりあえず入力トークン数でメモリ確保 (スペースが含まれていれば Tokenize 後の使用量は少なくなるはず)
	m_tokenList.Attach(LN_NEW RPNTokenListT());
	m_tokenList->Reserve(exprEnd - exprBegin);

	Position pos = exprBegin;

	for (; pos != exprEnd; ++pos)
	{
		switch (pos->GetTokenType())
		{
		case TokenType_SpaceSequence:
		case TokenType_NewLine:
		case TokenType_Comment:
		case TokenType_EscNewLine:
			break;	// これらは空白扱い。何もせず次へ
		case TokenType_NumericLiteral:
			RPNTokenT token;
			token.Type = RPN_TT_NumericLiteral;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		case TokenType_Operator:
		{
			static const RPNTokenType CppTypeToRPNType[] =
			{
								/* TT_CppOP_SeparatorBegin		*/	RPN_TT_Unknown,
				/* TT_CppOP_SharpSharp  	##	*/	RPN_TT_Unknown,
				/* TT_CppOP_Sharp			#	*/	RPN_TT_Unknown,
				/* TT_CppOP_ArrowAsterisk	->*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Arrow			->	*/	RPN_TT_Unknown,
				/* TT_CppOP_Comma			,	*/	RPN_TT_Unknown,
				/* TT_CppOP_Increment		++	*/	RPN_TT_Unknown,
				/* TT_CppOP_Decrement		--	*/	RPN_TT_Unknown,
				/* TT_CppOP_LogicalAnd		&&	*/	RPN_TT_OP_LogicalAnd,
				/* TT_CppOP_LogicalOr		||	*/	RPN_TT_OP_LogicalOr,
				/* TT_CppOP_LessThenEqual	<=	*/	RPN_TT_OP_CompLessThanEqual,
				/* TT_CppOP_GreaterThenEqual>=	*/	RPN_TT_OP_CompGreaterThenEqual,
				/* TT_CppOP_CmpEqual		==	*/	RPN_TT_OP_CompEqual,
				/* TT_CppOP_LeftShiftEqual	<<=	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightShiftEqual	>>=	*/	RPN_TT_Unknown,
				/* TT_CppOP_PlusEqual		+=	*/	RPN_TT_Unknown,
				/* TT_CppOP_MinusEqual		-=	*/	RPN_TT_Unknown,
				/* TT_CppOP_MulEqual		*=	*/	RPN_TT_Unknown,
				/* TT_CppOP_DivEqual		/=	*/	RPN_TT_Unknown,
				/* TT_CppOP_ModEqual		%=	*/	RPN_TT_Unknown,
				/* TT_CppOP_AndEqual		&=	*/	RPN_TT_Unknown,
				/* TT_CppOP_OrEqual			|=	*/	RPN_TT_Unknown,
				/* TT_CppOP_NotEqual		!=	*/	RPN_TT_OP_CompNotEqual,
				/* TT_CppOP_Equal			=	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftShift		<<	*/	RPN_TT_OP_LeftShift,
				/* TT_CppOP_RightShift		>>	*/	RPN_TT_OP_RightShift,
				/* TT_CppOP_Plus			+	*/	RPN_TT_OP_BinaryPlus,
				/* TT_CppOP_Minul			-	*/	RPN_TT_OP_BinaryMinus,
				/* TT_CppOP_Asterisk		*	*/	RPN_TT_OP_Multiply,
				/* TT_CppOP_Slash			/	*/	RPN_TT_OP_Divide,
				/* TT_CppOP_Parseint		%	*/	RPN_TT_OP_IntegerDivide,
				/* TT_CppOP_Ampersand		&	*/	RPN_TT_OP_BitwiseAnd,
				/* TT_CppOP_Pipe			|	*/	RPN_TT_OP_BitwiseOr,
				/* TT_CppOP_Tilde			~	*/	RPN_TT_OP_BitwiseNot,
				/* TT_CppOP_Caret,			^	*/	RPN_TT_OP_BitwiseXor,
				/* TT_CppOP_Exclamation		!	*/	RPN_TT_OP_LogicalNot,
				/* TT_CppOP_Ellipsis		...	*/	RPN_TT_Unknown,
				/* TT_CppOP_DotAsterisk		.*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Dot				.	*/	RPN_TT_Unknown,
				/* TT_CppOP_DoubleColon		::	*/	RPN_TT_Unknown,
				/* TT_CppOP_Question		?	*/	RPN_TT_OP_CondTrue,
				/* TT_CppOP_Colon			:	*/	RPN_TT_OP_CondFalse,
				/* TT_CppOP_Semicolon		;	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftBrace		{	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightBrace		}	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftBracket		[	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightBracket	]	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftParen		(	*/	RPN_TT_OP_LeftParen,
				/* TT_CppOP_RightParen		)	*/	RPN_TT_OP_RightParen,
				/* TT_CppOP_LeftAngle		<	*/	RPN_TT_OP_CompLessThan,
				/* TT_CppOP_RightAngle		>	*/	RPN_TT_OP_CompGreaterThen,
			};
			assert(LN_ARRAY_SIZE_OF(CppTypeToRPNType) == (TT_CppOP_SeparatorEnd - TT_CppOP_SeparatorBegin));

			struct CppTokenInfo
			{
				int					Precedence;		///< 優先順位
				OpeatorAssociation	Association;	///< 結合方向
			};
			static const CppTokenInfo TokenInfoTable[] =
			{
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_Unknown,				// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLiteral,		// (Dummy)
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_LeftParen,			// (
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_RightParen,		// )
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryPlus,			// +
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryMinus,		// -
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_LogicalNot,		// !
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_BitwiseNot,		// ~
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_Multiply,			// *
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_Divide,			// /
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_IntegerDivide,		// %
				{ 6,	OpeatorAssociation_Left },	// RPN_TT_OP_BinaryPlus,		// + (Binary)
				{ 6,	OpeatorAssociation_Left },	// RPN_TT_OP_BinaryMinus,		// - (Binary)
				{ 7,	OpeatorAssociation_Left },	// RPN_TT_OP_LeftShift,			// <<
				{ 7,	OpeatorAssociation_Left },	// RPN_TT_OP_RightShift,		// >>
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompLessThan,		// <
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompLessThanEqual,	// <=
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompGreaterThen,	// >
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompGreaterThenEqual,// >=
				{ 9,	OpeatorAssociation_Left },	// RPN_TT_OP_CompEqual,			// ==
				{ 9,	OpeatorAssociation_Left },	// RPN_TT_OP_CompNotEqual,		// !=
				{ 10,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseAnd,		// &
				{ 11,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseXor,		// ^
				{ 12,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseOr,			// |
				{ 13,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalAnd,		// &&
				{ 14,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalOr,			// ||
				{ 15,	OpeatorAssociation_Right },	// RPN_TT_OP_CondTrue,			// ? (条件演算子)
				{ 15,	OpeatorAssociation_Right },	// RPN_TT_OP_CondFalse,			// : (条件演算子)
			};
			assert(LN_ARRAY_SIZE_OF(TokenInfoTable) == RPN_TT_Max);

			RPNTokenT token;
			token.Type = CppTypeToRPNType[pos->GetLangTokenType() - TT_CppOP_SeparatorBegin];

			// + or - の場合は単項演算子であるかをここで確認する。
			// ひとつ前の有効トークンが演算子であれば単項演算子である。
			if (token.Type == RPN_TT_OP_BinaryPlus || token.Type == RPN_TT_OP_BinaryMinus)
			{
				if (!m_tokenList->IsEmpty() && m_tokenList->GetLast().IsOperator()) 
				{
					if (token.Type == RPN_TT_OP_BinaryPlus) {
						token.Type = RPN_TT_OP_UnaryPlus;
					}
					else {	// if (token.Type == RPN_TT_OP_BinaryMinus)
						token.Type = RPN_TT_OP_UnaryMinus;
					}
				}
			}

			token.Precedence = TokenInfoTable[token.Type].Precedence;
			token.Association = TokenInfoTable[token.Type].Association;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		default:
			// TODO:error
			break;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::Parse()
{
	m_rpnTokenList.Attach(LN_NEW RPNTokenListT());
	m_rpnTokenList->Reserve(m_tokenList->GetCount());

	LN_FOREACH(RPNTokenT& token, *m_tokenList)
	{
		// 定数は出力リストへ積んでいく
		if (token.Type == RPN_TT_NumericLiteral)
		{
			m_rpnTokenList->Add(token);
		}
		else if (token.IsOperator())
		{
			switch (token.Type)
			{
			case RPN_TT_OP_LeftParen:
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_RightParen:
				PopOpStackGroupEnd();
				break;
			case RPN_TT_OP_CondTrue:
				//PushOpStack(&token);
				//rpnExpr.Add(oprtr);
				break;
			case RPN_TT_OP_CondFalse:
				break;
			// 通常の演算子
			default:
				PushOpStack(&token);
				break;
			}
		}
	}

	// 演算子用スタックに残っている要素を全て出力リストに移す
	while (!m_opStack.IsEmpty())
	{
		RPNTokenT* top;
		m_opStack.Pop(&top);
		if (top->Type == RPN_TT_OP_LeftParen) {
			//TODO: error括弧が閉じていない
			break;
		}
		else if (top->Type == RPN_TT_OP_CondTrue) {
			//TODO: error条件演算子が閉じていない
			break;
		}
		m_rpnTokenList->Add(*top);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::PushOpStack(RPNTokenT* token)	// Operator または CondTrue だけ PushOpStack される
{
	// スタックにあるものを取り除く作業。
	// これから入れようとしているものより、top の優先度の方が高ければ取り除く。
	// 同じ場合は取り除かない。
	// スタックには優先度の低いものが残り続けることになる。
	while (!m_opStack.IsEmpty())
	{
		RPNTokenT* top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_LeftParen) {
			// '(' は特別扱い。演算子の優先度でどうこうできない。
			// 別途、')' が見つかったとき、対応する '(' までのスタック要素を全てを出力リストへ移す。
			break;
		}

		if (top->Precedence < token->Precedence)	// [+ * ← +] と言う状態なら、*(5) +(6) なので * を取り除く
		{
			// 出力リストへ
			m_rpnTokenList->Add(*top);
			m_opStack.Pop();
		}
		else {
			break;
		}
	}
	m_opStack.Push(token);
}

//-----------------------------------------------------------------------------
// GroupEnd (')') が見つかったら呼ばれる
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::PopOpStackGroupEnd()
{
	// 対応する GroupStart ('(') が見つかるまでスタックの演算子を出力リストへ移していく。
	while (!m_opStack.IsEmpty())
	{
		RPNTokenT* top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_LeftParen) {
			m_opStack.Pop();	// GroupStart は捨てる
			break;
		}

		// 出力リストへ
		m_rpnTokenList->Add(*top);
		m_opStack.Pop();
	}
}

// テンプレートのインスタンス化
template class RPNParser<char>;
template class RPNParser<wchar_t>;

} // namespace Parser
} // namespace Lumino
