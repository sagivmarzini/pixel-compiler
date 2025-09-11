use super::lexer_base::LexerBase;
use super::tokens;

pub struct Lexer{
    base : LexerBase,
    original_code : String,
}


impl Lexer {
    pub fn new(source_code : &Vec<char>) -> Self {
        Lexer {
            base : LexerBase::new(source_code.to_vec()),
            original_code : source_code.iter().collect::<String>(),
        }
    }

    pub fn lex() {

    }
}   