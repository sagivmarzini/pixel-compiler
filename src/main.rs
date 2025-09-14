use std::{time::Instant, process::exit, env, fs};

mod lex;


fn main() {

    //get main args and check the second one (the file to compile)
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        println!("Error: please provide a file to compile");
        exit(1);
    }
    
    //get the input file and read it into input
    let input = fs::read_to_string(&args[1]).expect("Error reading file");

    
    let mut lexer = lex::lexer::Lexer::new(input.chars().collect());
    println!("Tokens: \n{:?}\n", lexer.lex().unwrap());


    //let lexer = Lexer::new(&source.chars().collect());
    //record_time("tokenization", lex);
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
