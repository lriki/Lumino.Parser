
#pragma once
#include <unordered_map>
#include "../../Common.h"
#include "../../TokenList.h"

LN_NAMESPACE_BEGIN
namespace parser
{

template<typename TValue>
class IdentifierMap
{
public:
	IdentifierMap()
		: m_table1(nullptr)
	{
		m_table1 = LN_NEW ItemArray[MaxKeyFirstCount * MaxKeyLastCount * MaxKeyLength];
	}

	virtual ~IdentifierMap()
	{
		LN_SAFE_DELETE_ARRAY(m_table1);
	}

	void Insert(const TokenChar* keyBegin, const TokenChar* keyEnd, TValue value)
	{
		assert(keyBegin);
		assert(keyEnd);
		assert(keyBegin < keyEnd);
		int len = keyEnd - keyBegin;
		if (len < MaxKeyLength)
		{
			Item* item = GetItem(*keyBegin, *(keyEnd - 1), len);
			if (item != nullptr)
			{
				memcpy(item->key.data(), keyBegin, len);
				item->value = value;
				return;
			}
		}

		StringA key(keyBegin, len);
		m_table2[key] = value;
	}

	bool Find(const TokenChar* keyBegin, const TokenChar* keyEnd, TValue* outValue)
	{
		if (len < MaxKeyLength)
		{
			Item* item = GetItem(*keyBegin, *(keyEnd - 1), len);
			if (item != nullptr)
			{
				if (StringTraits::Compare(keyBegin)
			}
		}

	}

public:

	/*
		a b c d e f g h i j k l m
		n o p q r s t u v w x y z
		A B C D E F G H I J K L M
		N O P Q R S T U V W X Y Z _

		0 1 2 3 4 5 6 7 8 9

		universal-character-name:
			\u hex-quad
			\U hex-quad hex-quad
	*/

	static const int MaxKeyFirstCount = 64;
	static const int MaxKeyLastCount = 64;
	static const int MaxKeyLength = 64;

	struct Item
	{
		std::array<TokenChar, MaxKeyLength>	key = {};
		TValue								item;
	};

	typedef Array<Item>	ItemArray;

	ItemArray*					m_table1;	// [先頭文字][終端文字][文字数]
	std::unordered_map<StringA, Item>	m_table2;

	Item* GetItem(TokenChar first, TokenChar last, int len)
	{
		static int charIndexTable[128] =
		{
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,			// '0'~'9'
			-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,	// 'A'~'O'
			25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, 36,	// 'P'~'Z', '_'
			-1, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,	// 'a'~'o'
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1, -1, -1, -1, -1,
		};
		assert(first < 128);
		assert(last < 128);
		int i1 = charIndexTable[first];
		int i2 = charIndexTable[last];
		if (i1 == -1) return nullptr;
		if (i2 == -1) return nullptr;
		return &m_table1[(i1 * MaxKeyLastCount * MaxKeyLength) + (i2 * MaxKeyLength) + len];
	}
};

// マクロ定義の実体。同名マクロはありえる(undef 後の再定義しても前のは保持する)。その場合は通常、定義行が違う。
// 
class MacroEntity
{
public:
	TokenString	name;
	TokenString	replacementContentString;	// 置換要素を文字列で並べたもの (完全一致の確認で使う。前後の空白は消しておく)
};

class MacroMap
{
public:
	MacroEntity* Insert(const Token& name, const TokenChar* replacementBegin, const TokenChar* replacementEnd);

	MacroEntity* Find(const Token& name);

private:
	Array<MacroEntity>			m_allMacroList;	// 過去に定義された全てのマクロ
	IdentifierMap<MacroEntity*>	m_macroMap;		// 再定義されたりしたものは一番新しいマクロが格納される
};

class RawReferenceMap
{

};

// プリプロセスしたファイル情報。
// .c か .h かは問わない。
// トークンリストは保持しないので注意。
class PreprocessedFileCacheItem
{
public:
	PathNameA		filePath;
	MacroMap		inputMacroMap;

	MacroMap		outputMacroMap;
	RawReferenceMap	outputRawReferenceMap;
};

class HeaderFileManager
{
public:

};
	
/**
	@brief
*/
class Preprocessor
{
public:
	Preprocessor();

	ResultState BuildPreprocessedTokenList(TokenList* tokenList, PreprocessedFileCacheItem* outFileCache, DiagnosticsItemSet* diag);

private:

	ResultState PollingDirectiveLine(Token* lineBegin, Token* lineEnd);

	//TokenList::iterator GetNextGenericToken(TokenList::iterator pos);


	enum class DirectiveSec
	{
		Idle,			// 何もしていない
		LineHead,		// 行頭である。または初期状態
		FindIdent,		// 識別子を探している
		FindLineEnd,	// "#" を見つけた
	};

	TokenList*					m_tokenList;
	PreprocessedFileCacheItem*	m_fileCache;
	DiagnosticsItemSet*			m_diag;
	DirectiveSec				m_seqDirective;
	Token*						m_preproLineHead;	// # の次のトークンを指している
};

} // namespace Parser
LN_NAMESPACE_END

