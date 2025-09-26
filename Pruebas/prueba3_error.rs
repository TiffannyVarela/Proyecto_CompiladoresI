//if anidado con else mal cerrado
fn prueba3_error() {
    if 1 < 2 {
        if 3 < 4 {
            return;
        else {
            let x = 99;
        }
    } else {
    }
}

fn main() {
    prueba3_error();
}
