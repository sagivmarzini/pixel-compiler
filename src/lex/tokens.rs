#[derive(Debug, PartialEq, Clone)]
pub enum Token {

    //
    Keyword(Keyword),
    Identifier(String),
    Integer(i32),

    EOF,

    // tecnical stuff
    LBrace,        // {
    RBrace,        // }
    LParen,        // (
    RParen,        // )
    Semicolon,      // ;

    Assignment,     // =

    //math operators
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /

    //logic operators
    And,            // &&
    Or,             // ||
    Equals,         // ==
    NotEqual,    // !=

    Less,           // <
    LessEqual,    // <=
    Greater,        // >
    GreaterEqual, // >=

    //unary operators
    Exclamation,    // !
}

#[derive(Debug, PartialEq, Clone)]
pub enum Keyword {
    Function,
    Return,
    Var,

    If,
    Else,
    Match,
    
    While,
    For,

}