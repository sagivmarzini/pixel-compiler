#[derive(Debug)]
pub enum Token {
    Keyword(Keyword),
    Identifier(String),
    Integer(i32),
    EOF,

    LBrace,    // {
    RBrace,    // }
    LParen,    // (
    RParen,    // )
    Semicolon, // ;

    Assignment, // =

    Plus,  // +
    Minus, // -
    Star,  // *
    Slash, // /

    And,         // &&
    Or,          // ||
    Equal,       // ==
    NotEqual,    // !=
    Exclamation, // !

    Less,         // <
    LessEqual,    // <=
    Greater,      // >
    GreaterEqual, // >=
}

#[derive(Debug)]
pub enum Keyword {
    Function,
    Return,
    Var,

    If,
    Else,

    While,
    For,
}
