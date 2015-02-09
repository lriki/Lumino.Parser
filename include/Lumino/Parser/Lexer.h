
#pragma once

#include <Lumino/Base/RefBuffer.h>
#include "TokenList.h"

namespace Lumino
{
namespace Parser
{
// 文字の種別
enum AlphaNumType
{
	AlphaNumType_MBC = 0,			///< マルチバイト文字に配置されるコード
	AlphaNumType_Control = 1,		///< 制御文字
	AlphaNumType_OpChar = 2,		///< オペレーションコード
	AlphaNumType_Number = 3,		///< 数字
	AlphaNumType_Alphabet = 4,		///< 通常文字
	AlphaNumType_HexAlpha = 5,		///< 16進数字としても使える文字
};

class ErrorManager;

template<typename TChar>
class Lexer
{
public:
	typename typedef TokenList<TChar> TokenListT;


public:
	Lexer();
	virtual ~Lexer(){}

	void Analyze(RefBuffer* buffer, ErrorManager* errorManager);
	TokenListT& GetTokenList() { return m_tokenList; }


private:
	// 
	int		ReadToken(const TChar* buffer);

	// 
	int		ReadSpaceSequence(const TChar* buffer);				// 半角スペース, TAB, 全角スペース等 (空白の連続は塊で解析)
	int		ReadNewLine(const TChar* buffer);
	int		ReadIdentifier(const TChar* buffer);
	int		ReadKeyword(const TChar* buffer);
	int		ReadNumericLiteral(const TChar* buffer);		// 10, 0x08, 1.5e-9, .10 等
	int		ReadCharOrStringLiteral(const TChar* buffer);	// 'a', "aaa", L"aaa", @"aaa"
	int		ReadOperator(const TChar* buffer);
	int		ReadComment(const TChar* buffer);
	//int		ReadPrePro(const TChar* buffer);
	int		ReadEscNewLine(const TChar* buffer);			// 行末 \ (行コメントの解析に影響するため、lexer レベルで必要)
	int		ReadMBSSequence(const TChar* buffer);

	// 
	//int		ReadHexLiteralStart(const TChar* buffer);		// 0x (BASICやるなんてこと名はいと思うけど &h とか $ とか)

protected:
	AlphaNumType GetAlphaNumType(TChar ch);
	const TChar* GetBufferEnd() const { return m_bufferEnd; }

	// 以下をオーバーライドする場合、終端確認には GetBufferEnd() を使用する
	virtual int CheckSpaceChar(const TChar* buffer) = 0;
	virtual int CheckCommentStart(const TChar* buffer) = 0;
	virtual int CheckCommentEnd(const TChar* buffer) = 0;
	virtual int CheckCommentLine(const TChar* buffer) = 0;
	virtual int CheckIdentStart(const TChar* buffer) = 0;
	virtual int CheckIdentLetter(const TChar* buffer) = 0;
	virtual int CheckKeyword(const TChar* buffer, int* langTokenType) = 0;
	virtual int CheckHexLiteralStart(const TChar* buffer) = 0;
	virtual int CheckIntegerSuffix(const TChar* buffer) = 0;
	virtual int CheckRealSuffix(const TChar* buffer) = 0;
	virtual int CheckExponentStart(const TChar* buffer) = 0;
	virtual int CheckStringStart(const TChar* buffer) = 0;
	virtual int CheckStringEnd(const TChar* buffer, const TChar* start) = 0;
	virtual int CheckStringEscape(const TChar* buffer, const TChar* start) = 0;
	virtual int CheckOperator(const TChar* buffer) = 0;
	virtual int CheckEscNewLine(const TChar* buffer) = 0;
	virtual bool CheckCaseSensitive() = 0;

	// トークンが確定するたびに呼び出される
	virtual void PollingToken(Token<TChar>& token) {}

private:
	TChar*			m_cursor;
	TChar*			m_bufferEnd;	// 入力バッファの終端 (最後の文字の次(EOF)を指す)
	TokenListT		m_tokenList;
	ErrorManager*	m_errorManager;
};

} // namespace Parser
} // namespace Lumino
