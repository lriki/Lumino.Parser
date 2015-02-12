
#include <Lumino/Text/UnicodeUtils.h>
#include "../../include/Lumino/Parser/Lexer.h"
#include "../../include/Lumino/Parser/ErrorInfo.h"

namespace Lumino
{
namespace Parser
{

static int g_alphaNumTypeTable[256] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
	2, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2,
	2, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

//=============================================================================
// Lexer
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Lexer<TChar>::Lexer()
: m_cursor(NULL)
, m_bufferEnd(NULL)
, m_errorManager(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void Lexer<TChar>::Analyze(RefBuffer* buffer, ErrorManager* errorManager)
{
	m_errorManager = errorManager;
	m_tokenList.Attach(LN_NEW TokenListT());

	// 最悪のパターンで容量確保
	m_tokenList->Reserve(buffer->GetSize());

	m_cursor = (TChar*)buffer->GetPointer();
	m_bufferEnd = m_cursor + buffer->GetSize();

	while (m_cursor < m_bufferEnd)
	{
		int nLen = ReadToken(m_cursor);
		if (nLen == 0) {
			// エラーは呼び出し先で処理済
			break;
		}

		// 新しく読み取られたトークンを通知する
		PollingToken(m_tokenList->GetLast());
		m_cursor += nLen;
	}

	// 最後に EOF を入れておく
	m_tokenList->Add(Token<TChar>(TokenType_EOF, m_bufferEnd, m_bufferEnd + 1));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadToken(const TChar* buffer)
{
	int count;

	// 空白並び
	count = ReadSpaceSequence(buffer);
	if (count > 0) { return count; }
	// 改行
	count = ReadNewLine(buffer);
	if (count > 0) { return count; }
	// 予約語
	count = ReadKeyword(buffer);
	if (count > 0) { return count; }
	// 識別子
	count = ReadIdentifier(buffer);
	if (count > 0) { return count; }
	// 数値リテラル
	count = ReadNumericLiteral(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_NumericLiteral, buffer, buffer + count));
		return count;
	}
	// 文字列リテラル
	count = ReadCharOrStringLiteral(buffer);
	if (count > 0) {
		return count;
	}
	// コメント (演算子よりも優先する)
	count = ReadComment(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_Comment, buffer, buffer + count));
		return count;
	}
	// 演算子
	count = ReadOperator(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_Operator, buffer, buffer + count));
		return count;
	}
	// プリプロセッサディレクティブ
	//count = ReadPrePro(buffer);
	//if (count > 0) {
	//	m_tokenList.push_back(Token<TChar>(TokenType_PrePro, buffer, buffer + count));
	//	return count;
	//}
	// 行末 \ 
	count = ReadEscNewLine(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_EscNewLine, buffer, buffer + count));
		return count;
	}
	// マルチバイト文字並び
	count = ReadMBSSequence(buffer);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_MBSSequence, buffer, buffer + count));
		return count;
	}
	// 不明なトークンが見つかった
	m_errorManager->AddError(ErrorCode_Error_UnknownToken);
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadSpaceSequence(const TChar* buffer)
{
	// 連続するスペース文字の数を返す
	// (全角スペースを許容する場合はそれ全体の文字数もカウント)
	const TChar* pos = buffer;
	while (pos < m_bufferEnd)
	{
		int count = CheckSpaceChar(pos);
		if (count == 0) {
			break;
		}
		pos += count;
	}

	// トークン作成
	if (buffer < pos) {
		m_tokenList->Add(Token<TChar>(TokenType_SpaceSequence, buffer, pos));
	}
	return pos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadNewLine(const TChar* buffer)
{
	int count = 0;
	if (buffer[0] == '\r' &&
		buffer[1] == '\n')
	{
		// トークン作成
		m_tokenList->Add(Token<TChar>(TokenType_NewLine, buffer, buffer + 2));
		return 2;
	}
	if (buffer[0] == '\n' ||
		buffer[0] == '\n')
	{
		// トークン作成
		m_tokenList->Add(Token<TChar>(TokenType_NewLine, buffer, buffer + 1));
		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadIdentifier(const TChar* buffer)
{
	// 識別子の先頭文字か？
	int count = CheckIdentStart(buffer);
	if (count == 0) {
		return 0;	// 識別子ではない
	}

	// 後に続く文字は正しいか？
	const TChar* pPos = buffer + count;
	while (pPos < m_bufferEnd)
	{
		count = CheckIdentLetter(pPos);
		if (count == 0)
			break;
		pPos += count;
	}

	// トークン作成
	m_tokenList->Add(Token<TChar>(TokenType_Identifier, buffer, pPos));
	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadKeyword(const TChar* buffer)
{
	int lnagTokenType = 0;
	int count = CheckKeyword(buffer, &lnagTokenType);
	if (count > 0) {
		m_tokenList->Add(Token<TChar>(TokenType_Keyword, lnagTokenType, buffer, buffer + count));
	}
	return count;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadNumericLiteral(const TChar* buffer)
{
	/* 123	10進数リテラル
	* 012	8進数リテラル
	*
	*/


	// TODO: この内容は LangDef に持っていくべきかも？

	bool	bExistPeriod = false;		/* 小数点が登場したらTRUE */
	bool	bExistExp = false;		/* 指数が登場したらTRUE */
	bool	bExistSuffix = false;		/* 接尾語が登場したらTRUE */
	bool	bHexMode = false;		/* 16進数で有る場合にTRUE */

	// 16進数の先頭チェック
	const TChar* pPos = buffer;
	int count = CheckHexLiteralStart(buffer);
	if (count > 0) {
		pPos += count;
		bHexMode = true;	// 16 進数である
	}

	int nStep = 0;

	bool bEnd = false;
	while (pPos < m_bufferEnd && !bEnd)
	{
		if (*pPos > 255) {	// wchar_t の範囲チェック
			break;
		}
		AlphaNumType charType = (AlphaNumType)g_alphaNumTypeTable[*pPos];

		// マルチバイトコードまたは制御文字または . 以外の演算子であれば終了
		if (charType == AlphaNumType_MBC ||
			charType == AlphaNumType_Control ||
			(charType == AlphaNumType_OpChar && *pPos != '.')){
			break;	// 終了
		}

		// サフィックスのチェック (サフィックスは一番最後だが、いろいろなところに書ける。1f 1.0f .1f 1.0-e2f)
		if (charType == AlphaNumType_Alphabet || charType == AlphaNumType_HexAlpha){
			// 整数型
			count = CheckIntegerSuffix(pPos);
			if (count > 0) {
				pPos += count;
				bExistSuffix = true;	// サフィックスを見つけた
				nStep = 10;
				continue;
			}
			// 実数型
			count = CheckRealSuffix(pPos);
			if (count > 0) {
				pPos += count;
				bExistSuffix = true;	// サフィックスを見つけた
				nStep = 10;
				continue;
			}
			// ※ 50LL のように重複の可能性があるため、複数見つかってもエラーにしない
		}

		if (bExistSuffix) {
			return 0;	// サフィックスの後に、サフィックスではない文字が来た。おかしい
		}

		switch (nStep)
		{
		case 0:		// 整数部分
		{
						if (bHexMode && charType == AlphaNumType_HexAlpha) {
							pPos++;		// 16 進数なら HexAlpha を許可して継続
						}
						else if (charType == AlphaNumType_Number) {
							pPos++;		// 普通の数値も OK
						}
						else if (charType == AlphaNumType_OpChar && *pPos == '.') {
							nStep = 1;	// . が見つかった。小数部分へ移行
							pPos++;
						}
						else {
							return 0;	// 10G 等、変なサフィックスとか、次の識別子との間にスペースが無いとか
						}
						break;
		}
		case 1:		// 小数部分
		{
						if (bHexMode && charType == AlphaNumType_HexAlpha) {
							pPos++;		// 16 進数なら HexAlpha を許可して継続
						}
						else if (charType == AlphaNumType_Number) {
							pPos++;		// 普通の数値も OK
						}
						else
						{
							count = CheckExponentStart(pPos);
							if (count > 0) {	// e E + -
								nStep = 2;		// e- とかが見つかった。指数部分へ移行
								pPos += count;
							}
							else {
								return 0;		// 10.5G 等、変なサフィックスとか、次の識別子との間にスペースが無いとか
							}
						}
						break;
		}
		case 2:		// 指数部分
		{
						if (charType == AlphaNumType_Number) {
							pPos++;		// 普通の数値は OK
						}
						else {
							return 0;	// それ以外は失敗
						}
						break;
		}
		}
	}

	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadCharOrStringLiteral(const TChar* buffer)
{
	// 文字列の開始チェック
	int startCount = CheckStringStart(buffer);
	if (startCount == 0) {
		return 0;
	}

	const TChar* pStart = buffer;
	const TChar* pPos = buffer + 1;
	int endCount = 0;
	while (pPos < m_bufferEnd)
	{
		// エスケープシーケンスのチェック
		int count = CheckStringEscape(pPos, pStart);
		if (count > 0) {
			pPos += count;
			continue;	// この後の解析には回さないで次の文字へ
		}

		// 文字列の終了チェック
		endCount = CheckStringEnd(pPos, pStart);
		if (endCount > 0) {
			pPos += endCount;
			break;		// 文字列終了
		}

		pPos++;
	}

	Token<TChar> token(TokenType_CharOrStringLiteral, buffer, pPos);
	token.SetStringValue(buffer + startCount, pPos - endCount);
	m_tokenList->Add(token);

	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadOperator(const TChar* buffer)
{
	return CheckOperator(buffer);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadComment(const TChar* buffer)
{
	// 範囲コメントの開始チェック
	int count = CheckCommentStart(buffer);
	if (count != 0)
	{
		const TChar* pStart = buffer;
		const TChar* pPos = buffer + 1;
		while (pPos < m_bufferEnd)
		{
			// 終了チェック
			count = CheckCommentEnd(pPos);
			if (count > 0) {
				pPos += count;
				break;		// 終了
			}

			pPos++;
		}
		return pPos - buffer;
	}

	// 行コメントの開始チェック
	count = CheckCommentLine(buffer);
	if (count != 0)
	{
		const TChar* pStart = buffer;
		const TChar* pPos = buffer + 1;
		while (pPos < m_bufferEnd)
		{
			// 行末 \ チェック
			count = ReadEscNewLine(pPos);
			if (count > 0) {
				pPos += count;
				continue;		// 継続
			}

			// 終了チェック
			count = CheckCommentEnd(pPos);
			if (*pPos == '\n') {
				//pPos += 1;	// \n はコメントに含まない
				break;			// 終了
			}

			pPos++;
		}
		return pPos - buffer;
	}

	return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//template<typename TChar>
//int Lexer<TChar>::ReadPrePro(const TChar* buffer)
//{
//	return 0;
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadEscNewLine(const TChar* buffer)
{
	return CheckEscNewLine(buffer);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int Lexer<TChar>::ReadMBSSequence(const TChar* buffer)
{
	// マルチバイト文字並び

	const TChar* pPos = buffer;
	while (pPos < m_bufferEnd)
	{
		int extraCount = 0;
		Text::UnicodeUtils::CheckUTF8TrailingBytes((const Text::UnicodeUtils::UTF8*)pPos, (const Text::UnicodeUtils::UTF8*)m_bufferEnd, false, &extraCount);

		// 追加バイトが無い = シングルバイト文字だった場合は終了
		if (extraCount == 0) {
			break;
		}

		// 今回調べた文字と、追加文字分進める
		pPos += (1 + extraCount);
	};

	return pPos - buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
AlphaNumType Lexer<TChar>::GetAlphaNumType(TChar ch)
{
	if (ch > 255) {
		return AlphaNumType_MBC;	// wchar_t 型の多バイトコード
	}
	return (AlphaNumType)g_alphaNumTypeTable[ch];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//template<typename TChar>
//int Lexer<TChar>::ReadHexLiteralStart(const TChar* buffer)
//{
//	return CheckHexLiteralStart(buffer);
//}

// テンプレートのインスタンス化
template class Lexer<char>;
template class Lexer<wchar_t>;

} // namespace Parser
} // namespace Lumino
