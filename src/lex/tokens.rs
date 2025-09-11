#[derive(Debug, PartialEq, Clone)]
pub enum Tokens {

    //
    Keyword,
    Identifier,
    Integer,

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

    Less,           // <
    Less_equals,    // <=
    Greater,        // >
    Greater_equals, // >=

    //unary operators
    Negative,       // -
    Exclamation,    // !
}