
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

	RPN_TT_NumericLiteral,			///< 数値リテラル

	RPN_TT_OP_LeftParen,			// (
	RPN_TT_OP_RightParen,			// )

	RPN_TT_OP_UnaryPlus,			// + (Unary)
	RPN_TT_OP_UnaryMinus,			// - (Unary)
	RPN_TT_OP_LogicalNot,			// !
	RPN_TT_OP_BitwiseNot,			// ~

	RPN_TT_OP_Multiply,				// *
	RPN_TT_OP_Divide,				// /
	RPN_TT_OP_IntegerDivide,		// %

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
	bool IsOperator() const { return RPN_TT_OP_LeftParen <= Type && Type <= RPN_TT_OP_CondFalse; }

	

public:
	RPNTokenType		Type;
	int					Precedence;		///< 優先順位
	OpeatorAssociation	Association;	///< 結合方向
	const Token<TChar>*	SourceToken;
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
	void PopOpStackGroupEnd();

private:
	RefPtr<RPNTokenListT>	m_tokenList;

	RefPtr<RPNTokenListT>	m_rpnTokenList;
	Stack<RPNTokenT*>		m_opStack;		///< 演算子用の作業スタック
	Stack<RPNTokenT*>		m_condStack;	///< 条件演算子用の作業スタック
};

} // namespace Parser
} // namespace Lumino
