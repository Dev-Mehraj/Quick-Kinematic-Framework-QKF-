#pragma once

#include "qkf/common.hpp"
#include <string>
#include <unordered_map>

namespace qkf {

enum class TokenType {
    // End of file & error
    Eof,
    Invalid,

    // Identifiers & Literals
    Identifier,
    NumberLiteral,
    StringLiteral,
    BooleanLiteral,

    // Delimiters (Strict Semicolon!)
    Semicolon,        // ;  (Strictly required)
    Colon,            // :
    Comma,            // ,
    Dot,              // .
    OpenBrace,        // {
    CloseBrace,       // }
    OpenParen,        // (
    CloseParen,       // )
    OpenBracket,      // [
    CloseBracket,     // ]

    // Operators
    Equal,            // =
    Plus,             // +
    Minus,            // -
    Star,             // *
    Slash,            // /
    Percent,          // %
    EqualEqual,       // ==
    ExclamationEqual, // !=
    LessThan,         // <
    GreaterThan,      // >
    LessEqual,        // <=
    GreaterEqual,     // >=
    AmpersandAmpersand,// &&
    PipePipe,         // ||
    Exclamation,      // !
    Arrow,            // ->
    FatArrow,         // =>
    ColonColon,       // :: (scope resolution operator)
    PlusPlus,         // ++
    MinusMinus,       // --
    PlusEqual,        // +=
    MinusEqual,       // -=
    StarEqual,        // *=
    SlashEqual,       // /=

    // Keywords
    KeywordCustom,
    KwF,              // f (function declaration)
    KwVoid,           // void (no return type)
    KwLet,            // let (variable declaration)
    KwTake,           // take (selective import)
    KwFrom,           // from (source module/file)
    KwUse,            // use (alias: take x from "file" use y;)
    KwImport,
    KwClass,          // class
    KwStruct,         // struct
    KwNamespace,      // namespace
    KwEnum,           // enum
    KwTemplate,       // template
    KwTypename,       // typename
    KwPublic,         // public
    KwPrivate,        // private
    KwProtected,      // protected
    KwStatic,         // static
    KwConst,          // const
    KwIf,             // if
    KwElse,           // else
    KwWhile,          // while
    KwDo,             // do
    KwFor,            // for
    KwIn,             // in
    KwBreak,          // break
    KwContinue,       // continue
    KwReturn,         // return
    KwTry,            // try
    KwCatch,          // catch
    KwFinally,        // finally
    KwThrow,          // throw
    KwWindow,         // window
    KwPage,
    KwComponent,
    KwState,
    KwAction
};

std::string token_type_name(TokenType type);

struct Token {
    TokenType type;
    std::string text;
    SourceLocation location;
};

// Keyword registry mapping strings to TokenTypes
class KeywordRegistry {
public:
    static KeywordRegistry& instance();

    void register_keyword(const std::string& name, TokenType type);
    bool is_keyword(const std::string& text, TokenType& out_type) const;
    const std::unordered_map<std::string, TokenType>& get_all_keywords() const;

private:
    KeywordRegistry();
    std::unordered_map<std::string, TokenType> keywords_;
};

} // namespace qkf
