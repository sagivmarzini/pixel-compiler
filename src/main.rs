use std::{env, fs, time::Instant};

use crate::lex::lexer::Lexer;

mod lex;

fn main() {
    // Get command-line arguments and check the second one (the file to compile)
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        panic!("Error: please provide a file to compile");
    }

    // Read the input file
    let input = fs::read_to_string(&args[1]).expect("Error reading file");

    let mut lexer = Lexer::new(input.chars().collect());
    println!("Tokens: \n{:?}\n", lexer.lex().unwrap());

    //let lexer = Lexer::new(&source.chars().collect());
    //record_time("tokenization", lex);
}

/// Record time it took to run a function
fn record_time<F, R>(name: &str, func: F) -> R
where
    F: FnOnce() -> R,
{
    let start = Instant::now();

    let result = func();

    let duration = start.elapsed();
    println!("finished {} in {:?}", name, duration);

    result
}
