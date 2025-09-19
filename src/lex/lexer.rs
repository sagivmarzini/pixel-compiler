use std::collections::VecDeque;

use super::token::{Keyword, Token};

pub struct Lexer {
    source: VecDeque<char>,
    line_index: i32,
    inline_pos: i32,
}

impl Lexer {
    pub fn new(source_code: Vec<char>) -> Self {
        Lexer {
            source: VecDeque::<char>::from_iter(source_code),
            line_index: 1,
            inline_pos: 0,
        }
    }

    pub fn lex(&mut self) -> Result<VecDeque<Token>, String> {
        let mut tokens = VecDeque::<Token>::new();

        while !self.source.is_empty() {
            // TODO: Replace unwrap usage
            let current = self.eat().unwrap();

            if let Some(token) = self.lex_single_char_token(current) {
                tokens.push_back(token);
                continue;
            } else if let Some(token) = self.lex_two_char_token(current)? {
                tokens.push_back(token);
                continue;
            }

            let token = match current {
                '\t' | ' ' => continue, // Skip whitespace
                '\n' => {
                    self.line_index += 1;
                    self.inline_pos = 0;
                    continue;
                }
                '0'..='9' => Token::Integer(self.read_number(current)?),
                'a'..='z' => match self.read_string(current)?.as_str() {
                    "function" => Token::Keyword(Keyword::Function),
                    "return" => Token::Keyword(Keyword::Return),
                    "var" => Token::Keyword(Keyword::Var),
                    string => Token::Identifier(String::from(string)),
                },
                _ => {
                    return Err(self.generate_unexpected_char_error(current));
                }
            };

            tokens.push_back(token);
        }

        tokens.push_back(Token::EOF);

        Ok(tokens)
    }

    fn peek(&self) -> Option<&char> {
        self.source.front()
    }
    fn eat(&mut self) -> Option<char> {
        self.inline_pos += 1;
        self.source.pop_front()
    }

    fn read_number(&mut self, first: char) -> Result<i32, String> {
        let mut num_str = String::new();
        num_str.push(first);

        while let Some(ch) = self.peek() {
            if ch.is_ascii_digit() {
                num_str.push(self.eat().unwrap());
            } else {
                break;
            }
        }

        Ok(num_str.parse::<i32>().unwrap())
    }

    fn read_string(&mut self, first: char) -> Result<String, String> {
        let mut string = String::new();
        string.push(first);

        while let Some(ch) = self.peek() {
            if ch.is_ascii_alphanumeric() || *ch == '_' {
                string.push(self.eat().unwrap());
            } else {
                break;
            }
        }

        Ok(string)
    }

    fn match_next(&mut self, expected: char) -> bool {
        if self.peek() == Some(&expected) {
            self.eat();
            true
        } else {
            false
        }
    }

    fn generate_unexpected_char_error(&self, ch: char) -> String {
        format!(
            "Unexpected character '{}' at line {}, column {}",
            ch, self.line_index, self.inline_pos
        )
    }

    fn lex_single_char_token(&mut self, current: char) -> Option<Token> {
        let token = match current {
            '{' => Token::LBrace,
            '}' => Token::RBrace,
            '(' => Token::LParen,
            ')' => Token::RParen,
            ';' => Token::Semicolon,
            // TODO: Parse as negative number if there is a number right after minus sign
            '-' => Token::Minus,
            '+' => Token::Plus,
            '*' => Token::Star,
            '/' => Token::Slash,

            _ => return None,
        };

        Some(token)
    }

    fn lex_two_char_token(&mut self, current: char) -> Result<Option<Token>, String> {
        let token = match current {
            '!' => {
                if self.match_next('=') {
                    Token::NotEqual
                } else {
                    Token::Exclamation
                }
            }

            '&' => {
                if self.match_next('&') {
                    Token::And
                } else {
                    return Err(self.generate_unexpected_char_error(current));
                }
            }

            '|' => {
                if self.match_next('|') {
                    Token::Or
                } else {
                    return Err(self.generate_unexpected_char_error(current));
                }
            }
            '=' => {
                if self.match_next('=') {
                    Token::Equal
                } else {
                    Token::Assignment
                }
            }
            '>' => {
                if self.match_next('=') {
                    Token::GreaterEqual
                } else {
                    Token::Greater
                }
            }
            '<' => {
                if self.match_next('=') {
                    Token::GreaterEqual
                } else {
                    Token::Less
                }
            }

            _ => return Ok(None),
        };

        Ok(Some(token))
    }
}
