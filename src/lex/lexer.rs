use std::collections::VecDeque;

use super::tokens::Token;

#[derive(Debug, PartialEq, Clone)]
pub struct Lexer {
    source : VecDeque<char>,
    original_source_code : String,
    line : i32,
    index : i32,
}


impl Lexer {
    pub fn new(source_code: Vec<char>) -> Self {
        Lexer {
             source: VecDeque::<char>::from_iter(source_code.clone()),
             original_source_code: String::from_iter(source_code),
             line: 1,
             index: 0,
        }
    }



    pub fn lex(&mut self) -> Result<Vec<Token>, String> {
        
        let mut tokens = Vec::<Token>::new();

        // iterate over the whole source code until nothing is left
        while !self.source.is_empty() {

            self.skip_whitespaces();

            if self.source.is_empty() {break}; // if skip whitespaces skips over to the end

            // get the next char from the source code
            let current = self.eat().unwrap();

            let curr_token = 
                match current {
                //try to lex every type of token


                '0'..='9' => self.lex_number(current), // try to lex a number
                'a'..='z' | 'A'..='Z' | '_' => self.lex_identifier(current), // try to lex an identifier

                ';' => Ok(Token::Semicolon),
                '{' => Ok(Token::L_brace),
                '}' => Ok(Token::R_brace),
                '(' => Ok(Token::L_paren),
                ')' => Ok(Token::R_brace),

                _ => {
                    // if the result of the function returns Ok then return it
                    if let Ok(token) = self.lex_binary_operators(current) {
                        Ok(token) //try to lex binary operators (+, - ...)
                    }
                    else if let Ok(token) = self.lex_boolian_operators(current) {
                        Ok(token) // try to lex bollian ops (<, ==)
                    } 
                    else { //if all else fails, throw an error
                        Err(format!("Unexpected character: {}", current))
                    }
            }};

            //push the selected token
            tokens.push(curr_token.unwrap());
        }

        // add end of file to the token list, and return it
        tokens.push(Token::EOF);
        
        Ok(tokens)
    }


    fn peek(&self) -> Option<&char> {
        self.source.front()
    }
    fn eat(&mut self) -> Option<char> {
        self.index += 1;
        self.source.pop_front()
    }

    fn skip_whitespaces(&mut self) {
        loop {
            match self.peek() {
                Some(' ') | Some('\t') => (),
                Some('\n') => {
                    self.index = 0;
                    self.line += 1;
                },
                _ => break,
            }
            self.eat();
        }
    }

    fn lex_number(&mut self, current: char) -> Result<Token, String> {
        // add numbers until the next char isnt a number
        let mut number = String::new();
        number.push(current);

        while let Some(ch) = self.peek() {
            if ch.is_ascii_digit() {
                number.push(self.eat().unwrap());
            } else {
                break;
            }
        }

        Ok(Token::Integer(number.parse::<i32>().unwrap()))
    }

    fn lex_identifier(&mut self, current: char) -> Result<Token, String> {
        // add letters to the string until it is not a letter
        let mut identifier = String::new();    
        identifier.push(current);

        while let Some(ch) = self.peek() {
            if ch.is_ascii_alphanumeric() || *ch == '_' {
                identifier.push(self.eat().unwrap());
            } else {
                break;
            }
        }

        Ok(Token::Identifier(identifier))
    }

    fn lex_binary_operators(&mut self, current: char) -> Result<Token, String> {
        match current {
            '+' => Ok(Token::Plus),
            '-' => Ok(Token::Minus),
            '/' => Ok(Token::Slash),
            '*' => Ok(Token::Star),
            _ => Err(format!("not a binary op"))
        }
    }

    fn lex_boolian_operators(&mut self, current: char) -> Result<Token, String> {
        match current {
            '&' => { match self.peek().unwrap() {
                '&' => {
                    self.eat();
                    Ok(Token::And)
                },
                _ => Err(format!("not and"))
                }
            }

            '|' => { match self.peek().unwrap() {
                '|' => {
                    self.eat();
                    Ok(Token::Or)
                },
                _ => Err(format!("not or"))
                }
            }

            '=' => { match self.peek().unwrap() {
                '=' => {
                    self.eat();
                    Ok(Token::Equals)
                },
                _ => Ok(Token::Assignment)
                }
            }

            '<' => { match self.peek().unwrap() {
                '=' => {
                    self.eat();
                    Ok(Token::Less_equal)
                },
                _ => Ok(Token::Less)
                }
            }

            '>' => { match self.peek().unwrap() {
                '=' => {
                    self.eat();
                    Ok(Token::Greater_equal)
                },
                _ => Ok(Token::Greater)
                }
            }

            '!' => { match self.peek().unwrap() {
                '=' => {
                    self.eat();
                    Ok(Token::Not_equal)
                },
                _ => Ok(Token::Exclamation)
                }
            }

            _ => Err(format!("not logical op"))
        }
    }
}
