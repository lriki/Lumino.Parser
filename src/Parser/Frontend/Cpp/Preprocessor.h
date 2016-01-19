
#pragma once
#include <unordered_map>
#include "../../Common.h"
#include "../../TokenList.h"
#include "../RpnParser.h"

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
		m_table1 = LN_NEW Item[MaxKeyFirstCount * MaxKeyLastCount * MaxKeyLength];
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
				item->item = value;
				return;
			}
		}

		TokenString key(keyBegin, len);
		m_table2[key] = value;
	}

	bool Find(const TokenChar* keyBegin, const TokenChar* keyEnd, TValue* outValue, CaseSensitivity cs)
	{
		int len = keyEnd - keyBegin;
		if (len < MaxKeyLength)
		{
			Item* item = GetItem(*keyBegin, *(keyEnd - 1), len);
			if (item != nullptr && item->key[0] != '\0')
			{
				if (StringTraits::Compare(keyBegin, item->key.data(), len, cs) == 0)
				{
					*outValue = item->item;
					return true;
				}
			}
		}

		auto itr = m_table2.find(TokenString(keyBegin, keyEnd - keyBegin));	// TODO: StringRef で検索できるように
		if (itr != m_table2.end())
		{
			*outValue = itr->second;
			return true;
		}
		return false;
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
		std::array<TokenChar, MaxKeyLength>	key = {};	// [0] が '\0' ならInsertされていないということ
		TValue								item;
	};

	//typedef Array<Item>	ItemArray;

	Item*					m_table1;	// [先頭文字][終端文字][文字数]
	//std::unordered_map<StringA, Item>	m_table2;
	std::map<TokenString, TValue>	m_table2;

	// 先頭文字、終端文字、長さから Item を取得する。名前が一致するかは改めてチェックすること。
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

	bool		undef = false;
};

class MacroMap
{
public:
	MacroEntity* Insert(const Token& name, const TokenChar* replacementBegin, const TokenChar* replacementEnd);

	MacroEntity* Find(const Token& name);

	bool IsDefined(const Token& name);

private:
	Array<MacroEntity>			m_allMacroList;	// 過去に定義された全てのマクロ
	IdentifierMap<MacroEntity*>	m_macroMap;		// 再定義されたりしたものは一番新しいマクロが格納される
};

class RawReferenceMap
{

};

// プリプロセスしたファイル情報。
// .c か .h かは問わない。
// ×トークンリストは保持しないので注意。
/*
	トークンリストも保持する。UIColors.h とか。
*/
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

	bool IsInValidSection() const;

	//TokenList::iterator GetNextGenericToken(TokenList::iterator pos);



	enum class DirectiveSec
	{
		Idle,			// 何もしていない
		LineHead,		// 行頭である。または初期状態
		FindIdent,		// 識別子を探している
		FindLineEnd,	// "#" を見つけた
	};

	// #if ～ #endif までの判定状態
	enum class ConditionalSectionState
	{
		None,			// 判定前、セクション外
		Valid,			// 有効判定グループ内
		Invalid,		// 無効判定グループ内
		Skip,			// 有効無効判定終了後、#endifまでskip可能
	};

	// #if ～ #endif までの情報
	struct ConditionalSection
	{
		ConditionalSectionState	state = ConditionalSectionState::None;	// #if～#endif までの現在の判定状態
		bool					elseProcessed = false;					// #else受付後はtrue(#else～#else防止の為)
	};

	TokenList*					m_tokenList;
	PreprocessedFileCacheItem*	m_fileCache;
	DiagnosticsItemSet*			m_diag;

	DirectiveSec				m_seqDirective;
	Stack<ConditionalSection>	m_conditionalSectionStack;
	Token*						m_preproLineHead;	// # の次のトークンを指している

	TokenList					m_preproExprTokenList;	// 前処理定数式のトークンを展開する
	RpnParser					m_rpnParser;
	RpnEvaluator				m_rpnEvaluator;
};

} // namespace Parser
LN_NAMESPACE_END

