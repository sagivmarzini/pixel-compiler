use std::collections::VecDeque;

pub struct LexerBase 
{
    source: VecDeque<char>,
    index: i32,
    line: i32,
}

impl LexerBase 
{
    pub fn new(source_code: Vec<char>) -> Self 
    {
        Self {
            source: VecDeque::from_iter(source_code),
            index: 0,
            line: 1,
        }
    }

    // peek at the front of the vec
    pub fn get_front(&self) -> Option<&char> 
    {
        self.source.front()
    }

    // delete the front of the vec
    pub fn delete_front(&mut self) -> Option<char> 
    {
        match self.get_front() {
            Some(&'\n') => {
                self.line += 1;
                self.index = 0;
            }
            Some(_) => {
                self.index += 1;
            }
            None => {}
        }

        self.source.pop_front()
    }

    // read a whole number from the string
    pub fn read_number(&mut self) -> Result<i32, String> 
    {
        todo!("impl read number")
    }

    pub fn read_string(&mut self) -> Result<i32, String> 
    {
        todo!("impl read string")
    }

    pub fn print_error(&mut self, original_code: &str, msg: &str) 
    {
        let lines: Vec<&str> = original_code.lines().collect();

        if self.line == 0 || self.line as usize > lines.len() {
            println!("error: invalid line number {}", self.line);
            return;
        }

        let line_text = lines[(self.line - 1) as usize]; 
        
        println!("error in line {} at index {}: {}\n", self.line, self.index, msg);
        println!("{}", line_text);

        let mut underline = String::new();
        for i in 0..line_text.len() {
            if i as i32 == self.index {
                underline.push('^');
            } else {
                underline.push('~');
            }
        }
        println!("{}", underline);
    }
}
