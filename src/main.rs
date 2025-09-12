mod lex;
use std::time::Instant;


fn main() {
    let source : &str = "     \n          these\nare\nsome\nlines\n";

    let mut lexer = lex::lexer::Lexer::new(source.chars().collect());
    println!("Tokens: \n{:?}\n", lexer.lex().unwrap());


    //let lexer = Lexer::new(&source.chars().collect());
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
