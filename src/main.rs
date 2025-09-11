mod lex;

use lex::lexer::Lexer;
use std::time::Instant;


fn main() {
    let source : &str = "these\nare\nsome\nlines\n";

    let lexer = Lexer::new(&source.chars().collect());
    //record_time("tokenization", lexe);
}




// generic function to record time it takes to run a function
fn record_time<F, R>(name: &str, func: F) -> R
where
    F: FnOnce() -> R,
{
    println!("starting {}", name);
    let start = Instant::now();

    let result = func();

    let duration = start.elapsed();
    println!("finished {} in {:?}", name, duration);

    result
}
