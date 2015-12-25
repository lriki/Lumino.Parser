
#include "Frontend\Cpp\CppLexer.h"
#include "DiagnosticsManager.h"
#include "RPNParser.h"

LN_NAMESPACE_BEGIN
namespace parser
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


	"A(1+2,3+4,5+6)" をパースするときは・・・
	, が見つかるたびに、opStack にある FuncCall までを出力リストに移す。
	) が見つかったら、FuncCall までを移し変えた後、FuncCall も消す。
	
	■ カンマ演算子の解析
	
	前述の通り、ダミー関数を利用する。
	ダミー関数は一番後ろの引数の評価結果を返す関数とする。


	
*/
#if 0
// http://en.cppreference.com/w/cpp/language/expressions
// http://en.cppreference.com/w/cpp/language/operator_logical
template<typename T>
struct RpnOperator
{
	static T UnaryPlus(T /*lhs*/, T rhs) { return rhs; }
	static T UnaryMinus(T /*lhs*/, T rhs) { return -rhs; }

	static T Multiply(T lhs, T rhs) { return lhs * rhs; }
	static T Divide(T lhs, T rhs) { return lhs / rhs; }
	static T Modulus(T lhs, T rhs) { return lhs % rhs; }
	static T BinaryPlus(T lhs, T rhs) { return lhs + rhs; }
	static T BinaryMinus(T lhs, T rhs) { return lhs - rhs; }

	static T LeftShift(T lhs, T rhs) { return lhs << rhs; }
	static T RightShift(T lhs, T rhs) { return lhs >> rhs; }

	static bool CompLessThan(T lhs, T rhs) { return lhs < rhs; }
	static bool CompLessThanEqual(T lhs, T rhs) { return lhs <= rhs; }
	static bool CompGreaterThen(T lhs, T rhs) { return lhs > rhs; }
	static bool CompGreaterThenEqual(T lhs, T rhs) { return lhs >= rhs; }
	static bool CompEqual(T lhs, T rhs) { return lhs == rhs; }
	static bool CompNotEqual(T lhs, T rhs) { return lhs != rhs; }

	static bool CompLessThan_Boolean(T lhs, T rhs) { return lhs < rhs; }
	static bool CompLessThanEqual_Boolean(T lhs, T rhs) { return lhs <= rhs; }
	static bool CompGreaterThen_Boolean(T lhs, T rhs) { return lhs > rhs; }
	static bool CompGreaterThenEqual_Boolean(T lhs, T rhs) { return lhs >= rhs; }
	static bool CompEqual_Boolean(T lhs, T rhs) { return lhs == rhs; }
	static bool CompNotEqual_Boolean(T lhs, T rhs) { return lhs != rhs; }


	static T BitwiseNot(T /*lhs*/, T rhs) { return ~rhs; }
	static T BitwiseAnd(T lhs, T rhs) { return lhs & rhs; }
	static T BitwiseXor(T lhs, T rhs) { return lhs ^ rhs; }
	static T BitwiseOr(T lhs, T rhs) { return lhs | rhs; }

	static bool LogicalNot(T /*lhs*/, T rhs) { return !(rhs != 0); }
	static bool LogicalAnd(T lhs, T rhs) { return (lhs != 0) && (rhs != 0); }
	static bool LogicalOr(T lhs, T rhs) { return (lhs != 0) || (rhs != 0); }

	static bool LogicalNot_Boolean(bool /*lhs*/, bool rhs) { return !rhs; }
	static bool LogicalAnd_Boolean(bool lhs, bool rhs) { return lhs && rhs; }
	static bool LogicalOr_Boolean(bool lhs, bool rhs) { return lhs || rhs; }
};



struct TokenTypeTableItem
{
	RpnTokenGroup		tokenGroup;
	RpnOperatorGroup	operatorGroup;
	bool				isUnary;
};
static TokenTypeTableItem g_tokenTypeTable[] = 
{
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_Unknown = 0,
	{ RpnTokenGroup::Identifier,	RpnOperatorGroup::Unknown,		false },		// RPN_TT_Identifier,				///< 識別子
	//{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLiteral,			///< 数値リテラル
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_GroupStart,			// (	※ 括弧はパースで取り除かれるので Unknown
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_GroupEnd,				// )	※ 括弧はパースで取り除かれるので Unknown
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	true },			// RPN_TT_OP_UnaryPlus,			// + (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	true },			// RPN_TT_OP_UnaryMinus,			// - (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_Multiply,				// *
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_Divide,				// /
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_Modulus,				// %
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_BinaryPlus,			// + (Binary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_BinaryMinus,			// - (Binary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_LeftShift,			// <<
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_RightShift,			// >>
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompLessThan,			// <
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompLessThanEqual,	// <=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompGreaterThen,		// >
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompGreaterThenEqual,	// >=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompEqual,			// ==
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompNotEqual,			// !=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		true },			// RPN_TT_OP_BitwiseNot,			// ~ (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_BitwiseAnd,			// &
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_BitwiseXor,			// ^
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_BitwiseOr,			// |
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		true },			// RPN_TT_OP_LogicalNot,			// ! (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		false },		// RPN_TT_OP_LogicalAnd,			// &&
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		false },		// RPN_TT_OP_LogicalOr,			// ||
	{ RpnTokenGroup::CondTrue,		RpnOperatorGroup::Conditional,	false },		// RPN_TT_OP_CondTrue,				// ? (条件演算子)
	{ RpnTokenGroup::CondFalse,		RpnOperatorGroup::Conditional,	false },		// RPN_TT_OP_CondFalse,			// : (条件演算子)
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)	※未対応
	{ RpnTokenGroup::FunctionCall,	RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_FuncCall,				///< 関数呼び出し (識別子部分を指す)
	
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Null,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_True,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_False,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Int32,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_UInt32,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Int64,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_UInt64,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Float,	/**< 32bit */
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Double,	/**< 64bit */
};

typedef nullptr_t(*NullOperator)(nullptr_t lhs, nullptr_t rhs);
typedef bool(*BooleanOperator)(bool lhs, bool rhs);
typedef int32_t(*Int32Operator)(int32_t lhs, int32_t rhs);
typedef uint32_t(*UInt32Operator)(uint32_t lhs, uint32_t rhs);
typedef int64_t(*Int64Operator)(int64_t lhs, int64_t rhs);
typedef uint64_t(*UInt64Operator)(uint64_t lhs, uint64_t rhs);
typedef float(*FloatOperator)(float lhs, float rhs);
typedef double(*DoubleOperator)(double lhs, double rhs);

typedef bool(*NullLogicalOperator)(nullptr_t lhs, nullptr_t rhs);
typedef bool(*BooleanLogicalOperator)(bool lhs, bool rhs);
typedef bool(*Int32LogicalOperator)(int32_t lhs, int32_t rhs);
typedef bool(*UInt32LogicalOperator)(uint32_t lhs, uint32_t rhs);
typedef bool(*Int64LogicalOperator)(int64_t lhs, int64_t rhs);
typedef bool(*UInt64LogicalOperator)(uint64_t lhs, uint64_t rhs);
typedef bool(*FloatLogicalOperator)(float lhs, float rhs);
typedef bool(*DoubleLogicalOperator)(double lhs, double rhs);

struct RpnTypedOperatorTableItem
{
	NullOperator	nullptrOperator;
	BooleanOperator	booleanOperator;
	Int32Operator	int32Operator;
	UInt32Operator	uint32Operator;
	Int64Operator	int64Operator;
	UInt64Operator	uint64Operator;
	FloatOperator	floatOperator;
	DoubleOperator	doubleOperator;

	NullLogicalOperator		nullptrLogicalOperator;
	BooleanLogicalOperator	booleanLogicalOperator;
	Int32LogicalOperator	int32LogicalOperator;
	UInt32LogicalOperator	uint32LogicalOperator;
	Int64LogicalOperator	int64LogicalOperator;
	UInt64LogicalOperator	uint64LogicalOperator;
	FloatLogicalOperator	floatLogicalOperator;
	DoubleLogicalOperator	doubleLogicalOperator;
};

#define LN_RPN_OPERATOR_DEFINE_NONE \
	{	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, \
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE(op) \
	{	nullptr, RpnOperator<bool>::op, RpnOperator<int32_t>::op, RpnOperator<uint32_t>::op, RpnOperator<int64_t>::op, RpnOperator<uint64_t>::op, RpnOperator<float>::op, RpnOperator<double>::op, \
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE_INTEGER(op) \
	{	nullptr, RpnOperator<bool>::op, RpnOperator<int32_t>::op, RpnOperator<uint32_t>::op, RpnOperator<int64_t>::op, RpnOperator<uint64_t>::op, nullptr, nullptr, \
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE_LOGICAL(op) \
	{	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, \
		nullptr, RpnOperator<bool>::op##_Boolean, RpnOperator<int32_t>::op, RpnOperator<uint32_t>::op, RpnOperator<int64_t>::op, RpnOperator<uint64_t>::op, RpnOperator<float>::op, RpnOperator<double>::op, }

RpnTypedOperatorTableItem g_rpnTypedOperatorTable[] =
{
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_Unknown = 0,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_Identifier,				///< 識別子
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_GroupStart,			// (
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_GroupEnd,				// )

	LN_RPN_OPERATOR_DEFINE(UnaryPlus),		// RPN_TT_OP_UnaryPlus,			// + (Unary)
	LN_RPN_OPERATOR_DEFINE(UnaryMinus),		// RPN_TT_OP_UnaryMinus,			// - (Unary)
	LN_RPN_OPERATOR_DEFINE(Multiply),		// RPN_TT_OP_Multiply,				// *
	LN_RPN_OPERATOR_DEFINE(Divide),			// RPN_TT_OP_Divide,				// /
	LN_RPN_OPERATOR_DEFINE(Modulus),		// RPN_TT_OP_Modulus,				// %
	LN_RPN_OPERATOR_DEFINE(BinaryPlus),		// RPN_TT_OP_BinaryPlus,			// + (Binary)
	LN_RPN_OPERATOR_DEFINE(BinaryMinus),	// RPN_TT_OP_BinaryMinus,			// - (Binary)

	LN_RPN_OPERATOR_DEFINE_INTEGER(LeftShift),		// RPN_TT_OP_LeftShift,			// <<
	LN_RPN_OPERATOR_DEFINE_INTEGER(RightShift),		// RPN_TT_OP_RightShift,			// >>

	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompLessThan),			// RPN_TT_OP_CompLessThan,			// <
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompLessThanEqual),		// RPN_TT_OP_CompLessThanEqual,	// <=
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompGreaterThen),		// RPN_TT_OP_CompGreaterThen,		// >
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompGreaterThenEqual),	// RPN_TT_OP_CompGreaterThenEqual,	// >=
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompEqual),				// RPN_TT_OP_CompEqual,			// ==
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompNotEqual),			// RPN_TT_OP_CompNotEqual,			// !=

	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseNot),		// RPN_TT_OP_BitwiseNot,			// ~ (Unary)
	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseAnd),		// RPN_TT_OP_BitwiseAnd,			// &
	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseXor),		// RPN_TT_OP_BitwiseXor,			// ^
	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseOr),		// RPN_TT_OP_BitwiseOr,			// |

	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalNot),		// RPN_TT_OP_LogicalNot,			// ! (Unary)
	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalAnd),		// RPN_TT_OP_LogicalAnd,			// &&
	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalOr),		// RPN_TT_OP_LogicalOr,			// ||

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_CondTrue,				// ? (条件演算子)
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_CondFalse,			// : (条件演算子)

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_FuncCall,				///< 関数呼び出し (識別子部分を指す)

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Int32,	/**< C/C++ の char/wchar_t もこれになる */
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_UInt32,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Int64,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_UInt64,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Float,	/**< 32bit */
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Double,	/**< 64bit */
};
#endif


// http://en.cppreference.com/w/cpp/language/expressions
// http://en.cppreference.com/w/cpp/language/operator_logical
struct RpnOperator
{
	template<typename T> static T UnaryPlus(T /*lhs*/, T rhs) { return rhs; }
	template<typename T> static T UnaryMinus(T /*lhs*/, T rhs) { return -rhs; }
	template<> static uint32_t UnaryMinus<uint32_t>(uint32_t /*lhs*/, uint32_t rhs) { return (uint32_t)-((int32_t)rhs); }
	template<> static uint64_t UnaryMinus<uint64_t>(uint64_t /*lhs*/, uint64_t rhs) { return (uint64_t)-((int64_t)rhs); }

	template<typename T> static T Multiply(T lhs, T rhs) { return lhs * rhs; }
	template<typename T> static T Divide(T lhs, T rhs) { return lhs / rhs; }
	template<typename T> static T Modulus(T lhs, T rhs) { return lhs % rhs; }
	template<typename T> static T BinaryPlus(T lhs, T rhs) { return lhs + rhs; }
	template<typename T> static T BinaryMinus(T lhs, T rhs) { return lhs - rhs; }

	template<typename T> static T LeftShift(T lhs, T rhs) { return lhs << rhs; }
	template<typename T> static T RightShift(T lhs, T rhs) { return lhs >> rhs; }

	template<typename T> static bool CompLessThan(T lhs, T rhs) { return lhs < rhs; }
	template<typename T> static bool CompLessThanEqual(T lhs, T rhs) { return lhs <= rhs; }
	template<typename T> static bool CompGreaterThen(T lhs, T rhs) { return lhs > rhs; }
	template<typename T> static bool CompGreaterThenEqual(T lhs, T rhs) { return lhs >= rhs; }
	template<typename T> static bool CompEqual(T lhs, T rhs) { return lhs == rhs; }
	template<typename T> static bool CompNotEqual(T lhs, T rhs) { return lhs != rhs; }

	template<typename T> static T BitwiseNot(T /*lhs*/, T rhs) { return ~rhs; }
	template<typename T> static T BitwiseAnd(T lhs, T rhs) { return lhs & rhs; }
	template<typename T> static T BitwiseXor(T lhs, T rhs) { return lhs ^ rhs; }
	template<typename T> static T BitwiseOr(T lhs, T rhs) { return lhs | rhs; }

	template<typename T> static bool LogicalNot(T /*lhs*/, T rhs) { return !(rhs != 0); }
	template<typename T> static bool LogicalAnd(T lhs, T rhs) { return (lhs != 0) && (rhs != 0); }
	template<typename T> static bool LogicalOr(T lhs, T rhs) { return (lhs != 0) || (rhs != 0); }


	template<> static bool LogicalNot<bool>(bool /*lhs*/, bool rhs) { return !rhs; }
	template<> static bool LogicalAnd<bool>(bool lhs, bool rhs) { return lhs && rhs; }
	template<> static bool LogicalOr<bool>(bool lhs, bool rhs) { return lhs || rhs; }
};



struct TokenTypeTableItem
{
	RpnTokenGroup		tokenGroup;
	RpnOperatorGroup	operatorGroup;
	bool				isUnary;
};
static TokenTypeTableItem g_tokenTypeTable[] = 
{
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_Unknown = 0,
	{ RpnTokenGroup::Identifier,	RpnOperatorGroup::Unknown,		false },		// RPN_TT_Identifier,				///< 識別子
	//{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLiteral,			///< 数値リテラル
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_GroupStart,			// (	※ 括弧はパースで取り除かれるので Unknown
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_GroupEnd,				// )	※ 括弧はパースで取り除かれるので Unknown
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	true },			// RPN_TT_OP_UnaryPlus,			// + (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	true },			// RPN_TT_OP_UnaryMinus,			// - (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_Multiply,				// *
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_Divide,				// /
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_Modulus,				// %
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_BinaryPlus,			// + (Binary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false },		// RPN_TT_OP_BinaryMinus,			// - (Binary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_LeftShift,			// <<
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_RightShift,			// >>
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompLessThan,			// <
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompLessThanEqual,	// <=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompGreaterThen,		// >
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompGreaterThenEqual,	// >=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompEqual,			// ==
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false },		// RPN_TT_OP_CompNotEqual,			// !=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		true },			// RPN_TT_OP_BitwiseNot,			// ~ (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_BitwiseAnd,			// &
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_BitwiseXor,			// ^
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false },		// RPN_TT_OP_BitwiseOr,			// |
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		true },			// RPN_TT_OP_LogicalNot,			// ! (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		false },		// RPN_TT_OP_LogicalAnd,			// &&
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		false },		// RPN_TT_OP_LogicalOr,			// ||
	{ RpnTokenGroup::CondTrue,		RpnOperatorGroup::Conditional,	false },		// RPN_TT_OP_CondTrue,				// ? (条件演算子)
	{ RpnTokenGroup::CondFalse,		RpnOperatorGroup::Conditional,	false },		// RPN_TT_OP_CondFalse,			// : (条件演算子)
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)	※未対応
	{ RpnTokenGroup::FunctionCall,	RpnOperatorGroup::Unknown,		false },		// RPN_TT_OP_FuncCall,				///< 関数呼び出し (識別子部分を指す)
	
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Null,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_True,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_False,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Int32,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_UInt32,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Int64,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_UInt64,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Float,	/**< 32bit */
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false },		// RPN_TT_NumericLitaral_Double,	/**< 64bit */
};

typedef nullptr_t(*NullOperator)(nullptr_t lhs, nullptr_t rhs);
typedef bool(*BooleanOperator)(bool lhs, bool rhs);
typedef int32_t(*Int32Operator)(int32_t lhs, int32_t rhs);
typedef uint32_t(*UInt32Operator)(uint32_t lhs, uint32_t rhs);
typedef int64_t(*Int64Operator)(int64_t lhs, int64_t rhs);
typedef uint64_t(*UInt64Operator)(uint64_t lhs, uint64_t rhs);
typedef float(*FloatOperator)(float lhs, float rhs);
typedef double(*DoubleOperator)(double lhs, double rhs);

typedef bool(*NullLogicalOperator)(nullptr_t lhs, nullptr_t rhs);
typedef bool(*BooleanLogicalOperator)(bool lhs, bool rhs);
typedef bool(*Int32LogicalOperator)(int32_t lhs, int32_t rhs);
typedef bool(*UInt32LogicalOperator)(uint32_t lhs, uint32_t rhs);
typedef bool(*Int64LogicalOperator)(int64_t lhs, int64_t rhs);
typedef bool(*UInt64LogicalOperator)(uint64_t lhs, uint64_t rhs);
typedef bool(*FloatLogicalOperator)(float lhs, float rhs);
typedef bool(*DoubleLogicalOperator)(double lhs, double rhs);

struct RpnTypedOperatorTableItem
{
	NullOperator	nullptrOperator;
	BooleanOperator	booleanOperator;
	Int32Operator	int32Operator;
	UInt32Operator	uint32Operator;
	Int64Operator	int64Operator;
	UInt64Operator	uint64Operator;
	FloatOperator	floatOperator;
	DoubleOperator	doubleOperator;

	NullLogicalOperator		nullptrLogicalOperator;
	BooleanLogicalOperator	booleanLogicalOperator;
	Int32LogicalOperator	int32LogicalOperator;
	UInt32LogicalOperator	uint32LogicalOperator;
	Int64LogicalOperator	int64LogicalOperator;
	UInt64LogicalOperator	uint64LogicalOperator;
	FloatLogicalOperator	floatLogicalOperator;
	DoubleLogicalOperator	doubleLogicalOperator;
};

#define LN_RPN_OPERATOR_DEFINE_NONE \
	{	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, \
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE(op) \
	{	nullptr, nullptr, RpnOperator::op<int32_t>, RpnOperator::op<uint32_t>, RpnOperator::op<int64_t>, RpnOperator::op<uint64_t>, RpnOperator::op<float>, RpnOperator::op<double>, \
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE_INTEGER(op) \
	{	nullptr, nullptr, RpnOperator::op<int32_t>, RpnOperator::op<uint32_t>, RpnOperator::op<int64_t>, RpnOperator::op<uint64_t>, nullptr, nullptr, \
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE_LOGICAL(op) \
	{	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, \
		nullptr, RpnOperator::op<bool>, RpnOperator::op<int32_t>, RpnOperator::op<uint32_t>, RpnOperator::op<int64_t>, RpnOperator::op<uint64_t>, RpnOperator::op<float>, RpnOperator::op<double>, }

RpnTypedOperatorTableItem g_rpnTypedOperatorTable[] =
{
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_Unknown = 0,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_Identifier,				///< 識別子
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_GroupStart,			// (
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_GroupEnd,				// )

	LN_RPN_OPERATOR_DEFINE(UnaryPlus),		// RPN_TT_OP_UnaryPlus,			// + (Unary)
	LN_RPN_OPERATOR_DEFINE(UnaryMinus),		// RPN_TT_OP_UnaryMinus,			// - (Unary)
	LN_RPN_OPERATOR_DEFINE(Multiply),		// RPN_TT_OP_Multiply,				// *
	LN_RPN_OPERATOR_DEFINE(Divide),			// RPN_TT_OP_Divide,				// /
	LN_RPN_OPERATOR_DEFINE_INTEGER(Modulus),		// RPN_TT_OP_Modulus,				// %
	LN_RPN_OPERATOR_DEFINE(BinaryPlus),		// RPN_TT_OP_BinaryPlus,			// + (Binary)
	LN_RPN_OPERATOR_DEFINE(BinaryMinus),	// RPN_TT_OP_BinaryMinus,			// - (Binary)

	LN_RPN_OPERATOR_DEFINE_INTEGER(LeftShift),		// RPN_TT_OP_LeftShift,			// <<
	LN_RPN_OPERATOR_DEFINE_INTEGER(RightShift),		// RPN_TT_OP_RightShift,			// >>

	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompLessThan),			// RPN_TT_OP_CompLessThan,			// <
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompLessThanEqual),		// RPN_TT_OP_CompLessThanEqual,	// <=
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompGreaterThen),		// RPN_TT_OP_CompGreaterThen,		// >
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompGreaterThenEqual),	// RPN_TT_OP_CompGreaterThenEqual,	// >=
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompEqual),				// RPN_TT_OP_CompEqual,			// ==
	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompNotEqual),			// RPN_TT_OP_CompNotEqual,			// !=

	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseNot),		// RPN_TT_OP_BitwiseNot,			// ~ (Unary)
	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseAnd),		// RPN_TT_OP_BitwiseAnd,			// &
	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseXor),		// RPN_TT_OP_BitwiseXor,			// ^
	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseOr),		// RPN_TT_OP_BitwiseOr,			// |

	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalNot),		// RPN_TT_OP_LogicalNot,			// ! (Unary)
	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalAnd),		// RPN_TT_OP_LogicalAnd,			// &&
	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalOr),		// RPN_TT_OP_LogicalOr,			// ||

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_CondTrue,				// ? (条件演算子)
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_CondFalse,			// : (条件演算子)

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_OP_FuncCall,				///< 関数呼び出し (識別子部分を指す)

	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Int32,	/**< C/C++ の char/wchar_t もこれになる */
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_UInt32,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Int64,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_UInt64,
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Float,	/**< 32bit */
	LN_RPN_OPERATOR_DEFINE_NONE,	// RPN_TT_NumericLitaral_Double,	/**< 64bit */
};

// 両辺の型は揃っていることが前提
static bool EvalOperand(const RPNToken& token, const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outOperand)
{
	auto item = g_rpnTypedOperatorTable[token.Type];
	outOperand->type = rhs.type;	// 単項の場合は lhs が Unknown になっているので rhs で見る
	switch (rhs.type)
	{
	case RpnOperandType::Boolean:	outOperand->valueBoolean = item.booleanOperator(lhs.valueBoolean, rhs.valueBoolean); return true;
	case RpnOperandType::Int32:		outOperand->valueInt32 = item.int32Operator(lhs.valueInt32, rhs.valueInt32); return true;
	case RpnOperandType::UInt32:	outOperand->valueUInt32 = item.uint32Operator(lhs.valueUInt32, rhs.valueUInt32); return true;
	case RpnOperandType::Int64:		outOperand->valueInt64 = item.int64Operator(lhs.valueInt64, rhs.valueInt64); return true;
	case RpnOperandType::UInt64:	outOperand->valueUInt64 = item.uint64Operator(lhs.valueUInt64, rhs.valueUInt64); return true;
	case RpnOperandType::Float:		outOperand->valueFloat = item.floatOperator(lhs.valueFloat, rhs.valueFloat); return true;
	case RpnOperandType::Double:	outOperand->valueDouble = item.doubleOperator(lhs.valueDouble, rhs.valueDouble); return true;
	default:
		assert(0);
		return false;
	}
}

//=============================================================================
// RPNToken
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RpnTokenGroup RPNToken::GetTokenGroup() const
{
	return g_tokenTypeTable[Type].tokenGroup;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RpnOperatorGroup RPNToken::GetOperatorGroup() const
{
	return g_tokenTypeTable[Type].operatorGroup;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RPNToken::IsUnary() const
{
	return g_tokenTypeTable[Type].isUnary;
}

//=============================================================================
// RPNParser
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RefPtr<RPNTokenList> RPNParser::ParseCppConstExpression(Position exprBegin, Position exprEnd, DiagnosticsItemSet* diag)
{
	// とりあえずここで追加忘れチェック
	assert(LN_ARRAY_SIZE_OF(g_tokenTypeTable) == RPN_TT_Max);
	RPNParser parser;
	parser.TokenizeCppConst(exprBegin, exprEnd);
	parser.Parse();
	return parser.m_rpnTokenList;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RPNParser::TokenizeCppConst(Position exprBegin, Position exprEnd)
{
	// とりあえず入力トークン数でメモリ確保 (スペースが含まれていれば Tokenize 後の使用量は少なくなるはず)
	m_tokenList.Attach(LN_NEW RPNTokenList());
	m_tokenList->Reserve(exprEnd - exprBegin + 2);

	// 実引数リスト解析処理が , 演算子の解析を兼ねられるように、リスト先頭にダミーの FuncCall を入れておく
	RPNToken headToken;
	headToken.Type = RPN_TT_OP_FuncCall;
	m_tokenList->Add(headToken);

	Position pos = exprBegin;

	for (; pos != exprEnd; ++pos)
	{
		switch (pos->GetCommonType())
		{
		case CommonTokenType::SpaceSequence:
		case CommonTokenType::NewLine:
		case CommonTokenType::Comment:
		case CommonTokenType::Unknown:
			break;	// これらは空白扱い。何もせず次へ
		case CommonTokenType::Identifier:
		{
			RPNToken token;
			token.Type = RPN_TT_Identifier;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case CommonTokenType::ArithmeticLiteral:
		{
			RPNToken token;
			switch (pos->GetLangTokenType())
			{
			case TT_NumericLitaralType_Char:		token.Type = RPN_TT_NumericLitaral_Int32; break;
			case TT_NumericLitaralType_WideChar:	token.Type = RPN_TT_NumericLitaral_Int32; break;
			case TT_NumericLitaralType_Int32:		token.Type = RPN_TT_NumericLitaral_Int32; break;
			case TT_NumericLitaralType_UInt32:		token.Type = RPN_TT_NumericLitaral_UInt32; break;
			case TT_NumericLitaralType_Int64:		token.Type = RPN_TT_NumericLitaral_Int64; break;
			case TT_NumericLitaralType_UInt64:		token.Type = RPN_TT_NumericLitaral_UInt64; break;
			case TT_NumericLitaralType_Float:		token.Type = RPN_TT_NumericLitaral_Float; break;
			case TT_NumericLitaralType_Double:		token.Type = RPN_TT_NumericLitaral_Double; break;
			default:
				m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidNumericType);
				return;
			}
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case CommonTokenType::Operator:
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
				//{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLiteral,		// (Dummy)
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupStart,		// (
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupEnd,			// )
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryPlus,			// +
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryMinus,		// -
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
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_BitwiseNot,		// ~
				{ 10,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseAnd,		// &
				{ 11,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseXor,		// ^
				{ 12,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseOr,			// |
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_LogicalNot,		// !
				{ 13,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalAnd,		// &&
				{ 14,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalOr,			// ||
				{ 15,	OpeatorAssociation_Right },	// RPN_TT_OP_CondTrue,			// ? (条件演算子)
				{ 15,	OpeatorAssociation_Left },	// RPN_TT_OP_CondFalse,			// : (条件演算子)
				{ 17,	OpeatorAssociation_Left },	// RPN_TT_OP_Comma,				// , (カンマ演算子 or 実引数区切り文字)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_OP_FuncCall,			// (Dummy)
				
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Null,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_True,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_False,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Int32,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_UInt32,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Int64,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_UInt64,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Float,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Double,// (Dummy)
			};
			assert(LN_ARRAY_SIZE_OF(TokenInfoTable) == RPN_TT_Max);

			RPNToken token;
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
		case CommonTokenType::Keyword:
		{
			RPNToken token;
			if (pos->GetLangTokenType() == TT_CppKW_true)
			{
				token.Type = RPN_TT_NumericLitaral_True;
				token.SourceToken = &(*pos);
				m_tokenList->Add(token);
			}
			else if (pos->GetLangTokenType() == TT_CppKW_false)
			{
				token.Type = RPN_TT_NumericLitaral_False;
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
	RPNToken tailToken;
	tailToken.Type = RPN_TT_OP_GroupEnd;
	m_tokenList->Add(tailToken);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RPNParser::Parse()
{
	m_tmpRPNTokenList.Reserve(m_tokenList->GetCount());
	m_lastToken = nullptr;

	for (RPNToken& token : *m_tokenList)
	{
		// 現在の () 深さを振っておく
		token.GroupLevel = m_groupStack.GetCount();

		// 定数は出力リストへ積んでいく
		if (token.GetTokenGroup() == RpnTokenGroup::Literal)
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
					m_groupStack.GetTop()->ElementCount++;	// () 内の引数の数を増やす。ただし、"Func()" のように実引数が無ければ増やさない
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
				RPNToken* condTrue = PopOpStackCondFalse();
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
		RPNToken* top;
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
	m_rpnTokenList.Attach(LN_NEW RPNTokenList());
	m_rpnTokenList->Reserve(m_tmpRPNTokenList.GetCount());
	for (RPNToken* token : m_tmpRPNTokenList)
	{
		m_rpnTokenList->Add(*token);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RPNParser::PushOpStack(RPNToken* token)	// Operator または CondTrue だけ PushOpStack される
{
	// スタックにあるものを取り除く作業。
	// これから入れようとしているものより、top の優先度の方が高ければ取り除く。
	// 同じ場合は取り除かない。
	// スタックには優先度の低いものが残り続けることになる。
	while (!m_opStack.IsEmpty())
	{
		RPNToken* top = m_opStack.GetTop();
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
RPNToken* RPNParser::PopOpStackGroupEnd(bool fromArgsSeparator)
{
	// 対応する GroupStart ('(') が見つかるまでスタックの演算子を出力リストへ移していく。
	RPNToken* top = NULL;
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
		top = nullptr;
	}

	if (!top) {
		// TODO: error 必ず GroupStart が無ければならない
		// ↑×。最後にも1度呼ばれるためエラーにしてはいけない
		return nullptr;
	}
	return top;
}

//-----------------------------------------------------------------------------
// CondFalse (':') が見つかったら呼ばれる
//-----------------------------------------------------------------------------
RPNToken* RPNParser::PopOpStackCondFalse()
{
	// 対応する CondStart ('?') が見つかるまでスタックの演算子を出力リストへ移していく。
	RPNToken* top = nullptr;
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
		return nullptr;
	}
	return top;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RPNParser::CloseGroup(bool fromArgsSeparator)
{
	// 現在の () レベルの ':' 全てに CondGoto を振り、スタックから取り除く
	while (!m_condStack.IsEmpty())
	{
		RPNToken* condFalse = m_condStack.GetTop();
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

//=============================================================================
// RpnEvaluator
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::TryEval(const RPNTokenList* tokenList, DiagnosticsItemSet* diag, RpnOperand* outValue)
{
	LN_CHECK_ARGS_RETURNV(tokenList != nullptr, false);
	LN_CHECK_ARGS_RETURNV(diag != nullptr, false);
	m_diag = diag;

	Stack<RpnOperand> operandStack;
	Array<RpnOperand> funcCallArgs;
	RpnOperand operand, lhs, rhs;
	for (int iToken = 0; iToken < tokenList->GetCount(); ++iToken)
	{
		const RPNToken& token = tokenList->GetAt(iToken);
		switch (token.GetTokenGroup())
		{
			case RpnTokenGroup::Literal:
				if (!MakeOperand(token, &operand)) return false;
				break;
			//case RpnTokenGroup::Constant:
			//case RpnTokenGroup::Identifier:
			case RpnTokenGroup::Operator:
				if (token.IsUnary() && operandStack.GetCount() >= 1)
				{
					operandStack.Pop(&rhs);
					lhs.type = RpnOperandType::Unknown;
				}
				else if (operandStack.GetCount() >= 2)
				{
					operandStack.Pop(&rhs);
					operandStack.Pop(&lhs);
				}
				else
				{
					// Error: 引数が足りない
					m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidOperatorSide);
					return false;
				}
				if (!EvalOperator(token, lhs, rhs, &operand)) return false;
				break;

			case RpnTokenGroup::CondTrue:
			case RpnTokenGroup::CondFalse:
				return false;
			case RpnTokenGroup::FunctionCall:
				if (operandStack.GetCount() >= token.ElementCount)
				{
					funcCallArgs.Resize(token.ElementCount);
					for (int i = token.ElementCount - 1; i >= 0; --i)
					{
						operandStack.Pop(&funcCallArgs[i]);
					}
					if (!CallFunction(token, funcCallArgs, &operand)) return false;
				}
				else
				{
					// Error: 引数が足りない
					m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidFuncCallArgsCount);
					return false;
				}
				break;

			case RpnTokenGroup::Assignment:
				break;
			default:
				m_diag->Report(DiagnosticsCode::RpnEvaluator_UnexpectedToken);
				return false;
		}

		operandStack.Push(operand);
	}

	// 全てのトークンを読み終えると、スタックに1つだけ結果が出ている
	if (operandStack.GetCount() != 1)
	{
		m_diag->Report(DiagnosticsCode::RpnEvaluator_InsufficientToken);
		return false;
	}
	*outValue = operandStack.GetTop();
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::MakeOperand(const RPNToken& token, RpnOperand* outOperand)
{
	// リテラル
	if (token.GetTokenGroup() == RpnTokenGroup::Literal)
	{
		const TokenChar* str = token.SourceToken->GetBegin();
		int len = token.SourceToken->GetLength();
		const TokenChar* dummy;
		NumberConversionResult r;
		switch (token.Type)
		{
			case RPN_TT_NumericLitaral_Null:
				outOperand->type = RpnOperandType::Null;
				r = NumberConversionResult::Success;
				break;
			case RPN_TT_NumericLitaral_True:
				outOperand->type = RpnOperandType::Boolean;
				outOperand->valueBoolean = true;
				r = NumberConversionResult::Success;
				break;
			case RPN_TT_NumericLitaral_False:
				outOperand->type = RpnOperandType::Boolean;
				outOperand->valueBoolean = false;
				r = NumberConversionResult::Success;
				break;
			case RPN_TT_NumericLitaral_Int32:
				outOperand->type = RpnOperandType::Int32;
				outOperand->valueInt32 = StringTraits::ToInt32(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_UInt32:
				outOperand->type = RpnOperandType::UInt32;
				outOperand->valueUInt32 = StringTraits::ToInt32(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_Int64:
				outOperand->type = RpnOperandType::Int64;
				outOperand->valueInt64 = StringTraits::ToInt64(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_UInt64:
				outOperand->type = RpnOperandType::UInt64;
				outOperand->valueUInt64 = StringTraits::ToUInt64(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_Float:
				outOperand->type = RpnOperandType::Float;
				outOperand->valueFloat = (float)StringTraits::ToDouble(str, len, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_Double:
				outOperand->type = RpnOperandType::Double;
				outOperand->valueDouble = StringTraits::ToDouble(str, len, &dummy, &r);
				break;
		}
		if (r == NumberConversionResult::Success)
		{
			return true;
		}
	}

	// Error: 数値にできなかった
	m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidNumericLiteral);
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::EvalOperator(const RPNToken& token, const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outOperand)
{
	switch (token.GetOperatorGroup())
	{
	case RpnOperatorGroup::Arithmetic:
		return EvalOperatorArithmetic(token, lhs, rhs, outOperand);
	case RpnOperatorGroup::Comparison:
	case RpnOperatorGroup::Logical:
	case RpnOperatorGroup::Bitwise:
	default:
		// TODO: FatalError
		return false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::EvalOperatorArithmetic(const RPNToken& token, const RpnOperand& lhs_, const RpnOperand& rhs_, RpnOperand* outOperand)
{
	RpnOperand lhs, rhs;
	ExpandOperands(lhs_, rhs_, &lhs, &rhs);
	return EvalOperand(token, lhs, rhs, outOperand);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::CallFunction(const RPNToken& token, Array<RpnOperand> args, RpnOperand* outOperand)
{
	// 対応するトークンが無ければ、これはカンマ演算子解析用のダミー。一番後ろの引数を返すだけ。
	if (token.SourceToken == nullptr)
	{
		*outOperand = args.GetLast();
		return true;
	}
	else
	{
		LN_THROW(0, NotImplementedException);
		return false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RpnOperandType RpnEvaluator::ExpandOperands(const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outlhs, RpnOperand* outrhs)
{
	RpnOperandType type = std::max(lhs.type, rhs.type);
	*outlhs = lhs;
	*outrhs = rhs;
	CastOperand(outlhs, type);
	CastOperand(outrhs, type);
	return type;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnEvaluator::CastOperand(RpnOperand* op, RpnOperandType to)
{
	if (op->type == RpnOperandType::Unknown || to == RpnOperandType::Unknown || op->type == to) return;
	RpnOperand t = *op;
	switch (op->type)
	{
		case RpnOperandType::Int32:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueInt32 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueInt32; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueInt32; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueInt32; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueInt32; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueInt32; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueInt32; break;
			}
			break;
		}
		case RpnOperandType::UInt32:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueUInt32 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueUInt32; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueUInt32; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueUInt32; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueUInt32; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueUInt32; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueUInt32; break;
			}
			break;
		}
		case RpnOperandType::Int64:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueInt64 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueInt64; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueInt64; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueInt64; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueInt64; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueInt64; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueInt64; break;
			}
			break;
		}
		case RpnOperandType::UInt64:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueUInt64 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueUInt64; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueUInt64; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueUInt64; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueUInt64; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueUInt64; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueUInt64; break;
			}
			break;
		}
		case RpnOperandType::Float:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueFloat != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueFloat; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueFloat; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueFloat; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueFloat; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueFloat; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueFloat; break;
			}
			break;
		}
		case RpnOperandType::Double:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueDouble != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueDouble; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueDouble; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueDouble; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueDouble; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueDouble; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueDouble; break;
			}
			break;
		}
	}

	op->type = to;
}

} // namespace parser
LN_NAMESPACE_END
