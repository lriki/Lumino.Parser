
#if 0
#include <Lumino/Parser/CppLexer.h>
#include <Lumino/Parser/RPN/RPNParser.h>

LN_NAMESPACE_BEGIN
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
	■ 条件演算子の解析

		1 ? 6 ? 7 : 8 : 3 ? 4 : 5		分かりやすく括弧を付けると→ 1 ? (6 ? 7 : 8) : (3 ? 4 : 5)

	という式は、Parse では以下のように展開できれば良い。

		idx   >  0  1  2  3  4  5  6  7  8  9 10 11 12
		token >  1  ?  6  ?  7  :  8  :  3  ?  4  :  5
		goto  >     8     6    13    13    12    13

	Eval では、
	・トークンは左から右へ読んでいく。
	・? の goto は、条件式(オペランstack top)が false だった場合にジャンプする idx を示す。
	・: の goto は、ここにたどり着いたときにジャンプする idx を示す。(一律、式の終端(※)である)
	※「式の終端」とは、')' またはバッファ終端である。

	?: 以下の優先度の演算子は代入系とthrow、, だが、これらを考慮しない場合は
	たとえどれだけ : で区切られようとも : でジャンプするのは必ず式の終端である。

*/

/*
	■ 関数呼び出しの解析

	"1+2" のような関数呼び出しの無い単純な式でも、一番外側にはダミーの関数呼び出しがあるものとして解析する。
	イメージとしては "_(1+2)"。

	式の終端を ) であると考えることで、条件演算子や , 演算子との兼ね合いも少し楽になる。かも。
	↓
	, は全て、引数の区切りと考える。カンマ演算子も。
	そして、ダミー関数は一番後ろの引数の評価結果を返す関数とする。

	"A(1+1,2)" のパース結果は、
	
		1 1 + 2 A _

	となり、トークン A と _ の種別は「関数」。プロパティとして、引数の数を持つ。(_ はダミー関数である点に注意)
	評価では、この引数の数だけオペランド(評価済み定数)を取り出し、関数の処理を行う。
	A はユーザー定義。_ は最後の引数を返す。


	"A(1+2,3+4,5+6)" をパーすするときは・・・
	, が見つかるたびに、opStack にある FuncCall までを出力リストに移す。
	) が見つかったら、FuncCall までを移し変えた後、FuncCall も消す。
	


	
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
	m_tokenList->Reserve(exprEnd - exprBegin + 2);

	// 実引数リスト解析処理が , 演算子の解析を兼ねられるように、リスト先頭にダミーの FuncCall を入れておく
	RPNTokenT headToken;
	headToken.Type = RPN_TT_OP_FuncCall;
	m_tokenList->Add(headToken);

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
		case TokenType_Identifier:
		{
			RPNTokenT token;
			token.Type = RPN_TT_Identifier;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case TokenType_NumericLiteral:
		{
			RPNTokenT token;
			token.Type = RPN_TT_NumericLiteral;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case TokenType_Operator:
		{
			static const RPNTokenType CppTypeToRPNType[] =
			{
				/* TT_CppOP_SeparatorBegin		*/	RPN_TT_Unknown,
				/* TT_CppOP_SharpSharp  	##	*/	RPN_TT_Unknown,
				/* TT_CppOP_Sharp			#	*/	RPN_TT_Unknown,
				/* TT_CppOP_ArrowAsterisk	->*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Arrow			->	*/	RPN_TT_Unknown,
				/* TT_CppOP_Comma			,	*/	RPN_TT_OP_Comma,
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
				/* TT_CppOP_Parseint		%	*/	RPN_TT_OP_Modulus,
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
				/* TT_CppOP_LeftParen		(	*/	RPN_TT_OP_GroupStart,
				/* TT_CppOP_RightParen		)	*/	RPN_TT_OP_GroupEnd,
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
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_Identifier,			// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLiteral,		// (Dummy)
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupStart,		// (
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupEnd,			// )
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
				{ 15,	OpeatorAssociation_Left },	// RPN_TT_OP_CondFalse,			// : (条件演算子)
				{ 17,	OpeatorAssociation_Left },	// RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_OP_FuncCall,			// (Dummy)
			};
			assert(LN_ARRAY_SIZE_OF(TokenInfoTable) == RPN_TT_Max);

			RPNTokenT token;
			token.Type = CppTypeToRPNType[pos->GetLangTokenType() - TT_CppOP_SeparatorBegin];

			// ( かつひとつ前が識別子の場合は関数呼び出しとする
			if (token.Type == RPN_TT_OP_GroupStart &&
				!m_tokenList->IsEmpty() &&
				m_tokenList->GetLast().Type == RPN_TT_Identifier)
			{
				// Identifer の種類を FuncCall に変更し、( はトークンとして抽出しない
				m_tokenList->GetLast().Type = RPN_TT_OP_FuncCall;
			}
			else
			{
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
			}
			break;
		}
		default:
			// TODO:error
			break;
		}
	}

	// 実引数リスト解析処理が , 演算子の解析を兼ねられるように、リスト終端にダミーの GroupEnd を入れておく
	RPNTokenT tailToken;
	tailToken.Type = RPN_TT_OP_GroupEnd;
	m_tokenList->Add(tailToken);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::Parse()
{
	m_tmpRPNTokenList.Reserve(m_tokenList->GetCount());
	m_lastToken = NULL;

	LN_FOREACH(RPNTokenT& token, *m_tokenList)
	{
		// 現在の () 深さを振っておく
		token.GroupLevel = m_groupStack.GetCount();

		// 定数は出力リストへ積んでいく
		if (token.Type == RPN_TT_NumericLiteral)
		{
			m_tmpRPNTokenList.Add(&token);
		}
		else if (token.IsOperator())
		{
			switch (token.Type)
			{
			case RPN_TT_OP_FuncCall:
				m_groupStack.Push(&token);
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_GroupStart:
				m_groupStack.Push(&token);
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_GroupEnd:			// m_tokenList の最後はダミーの ) を入れているため、最後に1度必ず通る
				if (m_lastToken->Type != RPN_TT_OP_GroupStart) {
					m_groupStack.GetTop()->ElementCount++;	// () 無いの引数の数を増やす。ただし、"Func()" のように実引数が無ければ増やさない
				}
				PopOpStackGroupEnd(false);		// opStack の GroupStart または FuncCall までの内容を出力リストに移す。GroupStart または FuncCall は削除する。
				CloseGroup(false);				// 同レベル () 内の ':' 全てに CondGoto を振る。最後に現在のグループ情報を削除する
				break;
			case RPN_TT_OP_CondTrue:
				PushOpStack(&token);
				m_tmpRPNTokenList.Add(&token);
				break;
			case RPN_TT_OP_CondFalse:
			{
				RPNTokenT* condTrue = PopOpStackCondFalse();
				if (!condTrue) { return; }		// Error : ':' に対応する ? が見つからなかった
				m_tmpRPNTokenList.Add(&token);
				m_condStack.Push(&token);
				condTrue->CondGoto = m_tmpRPNTokenList.GetCount();	// ジャンプ先として ':' の次を指す
				break;
			}
			case RPN_TT_OP_Comma:
				// 現在のグループ内部の実引数の数をインクリメント。最初の , の場合は 1 余分に++する。
				//if (m_groupStack.GetTop()->ElementCount == 0) {	
				//	m_groupStack.GetTop()->ElementCount++;
				//}
				m_groupStack.GetTop()->ElementCount++;	

				PopOpStackGroupEnd(true);				// ( ～ , までの opStack の内容を出力リストに移す。ただし、( は残したままにする
				CloseGroup(true);						// グループ内の条件演算子の処理を行う。ただし、その後グループ情報は削除しない
				break;
			// 通常の演算子
			default:
				PushOpStack(&token);
				break;
			}
		}

		m_lastToken = &token;
	}

	PopOpStackCondFalse();

	// 同レベル () 内の ':' 全てに CondGoto を振る (一番外側の GroupEnd を閉じるという考え)
	//CloseGroup();

	// 演算子用スタックに残っている要素を全て出力リストに移す
	while (!m_opStack.IsEmpty())
	{
		RPNTokenT* top;
		m_opStack.Pop(&top);
		if (top->Type == RPN_TT_OP_GroupStart) {
			//TODO: error括弧が閉じていない
			break;
		}
		else if (top->Type == RPN_TT_OP_CondTrue) {
			//TODO: error条件演算子が閉じていない
			break;
		}
		m_tmpRPNTokenList.Add(top);
	}

	// 出力用のリストへ、トークンのコピーを作成しつつ移す
	m_rpnTokenList.Attach(LN_NEW RPNTokenListT());
	m_rpnTokenList->Reserve(m_tmpRPNTokenList.GetCount());
	LN_FOREACH(RPNTokenT* token, m_tmpRPNTokenList)
	{
		m_rpnTokenList->Add(*token);
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
		if (top->Type == RPN_TT_OP_GroupStart || top->Type == RPN_TT_OP_FuncCall) {
			// '(' は特別扱い。とにかく演算子スタックの先頭に積む。(演算子の優先度でどうこうできない)
			// 別途、')' が見つかったとき、対応する '(' までのスタック要素を全てを出力リストへ移す。
			break;
		}

		if (top->Precedence < token->Precedence)	// [+ * ← +] と言う状態なら、*(5) +(6) なので * を取り除く
		{
			// 出力リストへ
			m_tmpRPNTokenList.Add(top);
			m_opStack.Pop();
		}
		else {
			break;
		}
	}
	m_opStack.Push(token);
}

//-----------------------------------------------------------------------------
// GroupEnd (')') または , が見つかったら呼ばれる
//-----------------------------------------------------------------------------
template<typename TChar>
RPNToken<TChar>* RPNParser<TChar>::PopOpStackGroupEnd(bool fromArgsSeparator)
{
	// 対応する GroupStart ('(') が見つかるまでスタックの演算子を出力リストへ移していく。
	RPNTokenT* top = NULL;
	while (!m_opStack.IsEmpty())
	{
		top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_GroupStart)
		{
			m_opStack.Pop();	// GroupStart は捨てる
			break;
		}
		else if (top->Type == RPN_TT_OP_FuncCall)
		{
			// FuncCall は出力リストの末尾に積み、終了する。
			// ただし、, の場合は積まない。
			if (!fromArgsSeparator)
			{
				m_tmpRPNTokenList.Add(top);	
				m_opStack.Pop();
			}
			break;
		}

		// 出力リストへ
		m_tmpRPNTokenList.Add(top);
		m_opStack.Pop();
		top = NULL;
	}

	if (!top) {
		// TODO: error 必ず GroupStart が無ければならない
		// ↑×。最後にも1度呼ばれるためエラーにしてはいけない
		return NULL;
	}
	return top;
}

//-----------------------------------------------------------------------------
// CondFalse (':') が見つかったら呼ばれる
//-----------------------------------------------------------------------------
template<typename TChar>
RPNToken<TChar>* RPNParser<TChar>::PopOpStackCondFalse()
{
	// 対応する CondStart ('?') が見つかるまでスタックの演算子を出力リストへ移していく。
	RPNTokenT* top = NULL;
	while (!m_opStack.IsEmpty())
	{
		top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_CondTrue) {
			m_opStack.Pop();	// CondTrue は捨てる
			break;
		}

		// 出力リストへ
		m_tmpRPNTokenList.Add(top);
		m_opStack.Pop();
	}
	/* 
		↑直前の ? を探しに行くだけで良い。
		( ) と同じ考え方。
		x    ?    x ? x : x    :    x ? x : x
	*/

	if (!top) {
		// TODO: error 必ず CondTrue が無ければならない
		return NULL;
	}
	return top;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::CloseGroup(bool fromArgsSeparator)
{
	// 現在の () レベルの ':' 全てに CondGoto を振り、スタックから取り除く
	while (!m_condStack.IsEmpty())
	{
		RPNTokenT* condFalse = m_condStack.GetTop();
		if (condFalse->GroupLevel < m_groupStack.GetCount()) {
			break;
		}
		condFalse->CondGoto = m_tmpRPNTokenList.GetCount();
		m_condStack.Pop();
	}

	// グループ情報を1つ削除する。ただし、, の場合は残しておく。
	if (!fromArgsSeparator) {
		m_groupStack.Pop();
	}
}

// テンプレートのインスタンス化
template class RPNParser<char>;
template class RPNParser<wchar_t>;

} // namespace Parser
LN_NAMESPACE_END
#endif
