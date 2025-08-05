// Indica que estamos en el archivo principal
fn main() {
    println!("¡Hola, mundo!"); // Imprime un mensaje en consola

    println!("\n\nTipos de datos");
    tipos_datos();

    println!("\n\nCondicionales");
    condicionales();

    println!("\n\nCiclo For");
    ciclo_for();

    println!("\n\nArreglos");
    arreglos();

    println!("\n\nMapeo");
    mapeo();

    println!("\n\nEstructuras");
    estructuras();
}

// Función que demuestra uso de tipos de datos
fn tipos_datos() {
    let nombre = "Juan"; // Declara una variable inmutable
    let edad: i32 = 30;  // Declara una variable con tipo explícito
    println!("¡Hola, {}! Tienes {} años.", nombre, edad); // Imprime mensaje formateado
}

// Función que demuestra una estructura condicional
fn condicionales() {
    let numero = 10;
    if numero % 2 == 0 { // Verifica si el número es par
        println!("El número es par.");
    } else {
        println!("El número es impar.");
    }
}

// Función que muestra un ciclo for
fn ciclo_for() {
    for i in 0..8 { // Recorre del 0 al 7
        println!("Iteración: {}", i);
    }
}

// Función que muestra uso de vectores (similar a slices en Go)
fn arreglos() {
    let numeros = [1, 2, 3, 4, 5]; // Arreglo de enteros con tamaño fijo
    let mut tallas = vec!["S", "M", "L", "XL", "XXL"]; // Vector (slice dinámico)

    println!("Arreglo de números: {:?}", numeros); // {:?} para imprimir estructuras complejas
    println!("Slice de tallas: {:?}", tallas);

    tallas.push("XXXL"); // Agrega una talla al final del vector
    println!("Tallas actualizadas: {:?}", tallas);
}

// Función que muestra uso de mapas
fn mapeo() {
    use std::collections::HashMap;

    let mut disponibles = HashMap::new(); // Crea un nuevo HashMap
    disponibles.insert("Azul", 10);
    disponibles.insert("Rojo", 5);
    disponibles.insert("Verde", 8);
    disponibles.insert("Amarillo", 3); // Agrega un nuevo elemento

    // Accede e imprime valores específicos
    if let Some(rojo) = disponibles.get("Rojo") {
        println!("Camisas Rojas Disponibles: {}", rojo);
    }

    println!("Inventario de Camisas: {:?}", disponibles);
}

// Función que muestra uso de estructuras
fn estructuras() {
    // Declaración de la estructura Estudiante
    struct Estudiante {
        nombre: String,
        numero_cuenta: i32,
    }

    // Instancia de la estructura
    let estudiante = Estudiante {
        nombre: String::from("Tiffanny"),
        numero_cuenta: 11811146,
    };

    // Imprimir los campos de la estructura
    println!("Información del Estudiante:");
    println!("Nombre: {} Edad: {}", estudiante.nombre, estudiante.numero_cuenta);
}
// Este es un ejemplo de un programa en Rust que muestra varios conceptos básicos