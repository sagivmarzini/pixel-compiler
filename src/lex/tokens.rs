#[derive(Debug, PartialEq, Clone)]
pub enum Token {

    //
    Keyword(Keyword),
    Identifier(String),
    Integer(i32),

    EOF,

    // tecnical stuff
    L_brace,        // {
    R_brace,        // }
    L_paren,        // (
    R_paren,        // )
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
    Not_equal,    // !=

    Less,           // <
    Less_equal,    // <=
    Greater,        // >
    Greater_equal, // >=

    //unary operators
    Exclamation,    // !
}

#[derive(Debug, PartialEq, Clone)]
pub enum Keyword {
    Function,
    Return,
    Var,
}