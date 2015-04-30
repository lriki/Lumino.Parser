
#pragma once

#include <Lumino/Base/Stack.h>
#include <Lumino/IO/PathName.h>
#include "../ParserObject.h"

namespace Lumino
{
namespace Parser
{
class ErrorManager;

enum RPNTokenType
{
	RPN_TT_Unknown = 0,

	RPN_TT_Identifier,				///< 識別子
	RPN_TT_NumericLiteral,			///< 数値リテラル

	RPN_TT_OP_GroupStart,			// (
	RPN_TT_OP_GroupEnd,				// )

	RPN_TT_OP_UnaryPlus,			// + (Unary)
	RPN_TT_OP_UnaryMinus,			// - (Unary)
	RPN_TT_OP_LogicalNot,			// !
	RPN_TT_OP_BitwiseNot,			// ~

	RPN_TT_OP_Multiply,				// *
	RPN_TT_OP_Divide,				// /
	RPN_TT_OP_Modulus,				// %

	RPN_TT_OP_BinaryPlus,			// + (Binary)
	RPN_TT_OP_BinaryMinus,			// - (Binary)

	RPN_TT_OP_LeftShift,			// <<
	RPN_TT_OP_RightShift,			// >>

	RPN_TT_OP_CompLessThan,			// <
	RPN_TT_OP_CompLessThanEqual,	// <=
	RPN_TT_OP_CompGreaterThen,		// >
	RPN_TT_OP_CompGreaterThenEqual,	// >=

	RPN_TT_OP_CompEqual,			// ==
	RPN_TT_OP_CompNotEqual,			// !=

	RPN_TT_OP_BitwiseAnd,			// &

	RPN_TT_OP_BitwiseXor,			// ^

	RPN_TT_OP_BitwiseOr,			// |

	RPN_TT_OP_LogicalAnd,			// &&

	RPN_TT_OP_LogicalOr,			// ||

	RPN_TT_OP_CondTrue,				// ? (条件演算子)
	RPN_TT_OP_CondFalse,			// : (条件演算子)

	RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)



	RPN_TT_OP_FuncCall,				///< 関数呼び出し (識別子部分を指す)

	//RPN_TT_OP_Negation,
	//RPN_TT_OP_Exponent,			// **

	RPN_TT_Max,	
};

/// 演算子の結合方向
enum OpeatorAssociation
{
	OpeatorAssociation_Left,
	OpeatorAssociation_Right,
};

/**
	@brief	逆ポーランド式を構成する要素です。
*/
template<typename TChar>
class RPNToken
{
public:
	RPNToken()
	{
		SourceToken = NULL;
		CondGoto = 0;
		ElementCount = 0;
	}

public:
	bool IsOperator() const { return RPN_TT_OP_GroupStart <= Type && Type <= RPN_TT_OP_FuncCall; }

	

public:
	RPNTokenType		Type;
	int					Precedence;		///< 優先順位
	OpeatorAssociation	Association;	///< 結合方向
	const Token<TChar>*	SourceToken;
	int					GroupLevel;		///< () ネストの深さ。ルートは 0

	int					CondGoto;		///< (Type が CondTrue または CondFalse のときに使用する)
	int					ElementCount;	///< , で区切られた要素数 (Type が GroupStart または FuncCall のときに使用する)


};

template<typename TChar>
class RPNParser
	: public ParserObject<TChar>
{
public:
	typename typedef RPNToken<TChar>		RPNTokenT;
	typename typedef ArrayList<RPNTokenT>	RPNTokenListT;

public:
	static ArrayList< RPNToken<TChar> >* ParseCppConstExpression(Position exprBegin, Position exprEnd, ErrorManager* errorInfo);

private:
	void TokenizeCppConst(Position exprBegin, Position exprEnd);
	void Parse();
	void PushOpStack(RPNTokenT* token);
	RPNToken<TChar>* PopOpStackGroupEnd(bool fromArgsSeparator);
	RPNToken<TChar>* PopOpStackCondFalse();
	void CloseGroup(bool fromArgsSeparator);

private:

	RefPtr<RPNTokenListT>	m_tokenList;
	RefPtr<RPNTokenListT>	m_rpnTokenList;

	//RefPtr<RPNTokenListT>	m_rpnTokenList;
	ArrayList<RPNTokenT*>	m_tmpRPNTokenList;
	Stack<RPNTokenT*>		m_opStack;			///< 演算子用の作業スタック
	Stack<RPNTokenT*>		m_condStack;		///< 条件演算子用の作業スタック。: を格納していく
	Stack<RPNTokenT*>		m_groupStack;		///< () の作業スタック。( または FuncCall を格納していく
	RPNTokenT*				m_lastToken;
};

} // namespace Parser
} // namespace Lumino
