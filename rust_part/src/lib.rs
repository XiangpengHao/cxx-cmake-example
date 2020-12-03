use std::fmt;

#[cxx::bridge]
mod ffi {
    #[namespace = "shared"]
    struct Color {
        r: u8,
        g: u8,
        b: u8,
    }

    #[namespace = "shared"]
    struct SharedThing {
        points: Box<Points>,
        persons: UniquePtr<Person>,
        pixels: Vec<Color>,
    }

    unsafe extern "C++" {
        include!("cpp_part.h");
        type Person;

        fn get_name(person: &Person) -> &CxxString;
        fn make_person() -> UniquePtr<Person>;
        fn is_black(self: &Color) -> bool;
    }

    #[namespace = "rust_part"]
    extern "Rust" {
        type Points;
        fn print_shared_thing(points: &SharedThing);
        fn make_shared_thing() -> SharedThing;
        fn rust_echo(val: i32) -> i32;
    }

    #[namespace = "shared"]
    extern "Rust" {
        fn is_white(self: &Color) -> bool;
    }
}

#[derive(Debug)]
pub struct Points {
    x: Vec<u8>,
    y: Vec<u8>,
}

impl ffi::Color {
    pub fn white() -> Self {
        Self {
            r: 255,
            g: 255,
            b: 255,
        }
    }

    pub fn black() -> Self {
        Self { r: 0, g: 0, b: 0 }
    }

    pub fn is_white(&self) -> bool {
        self.r == 255 && self.g == 255 && self.b == 255
    }
}

impl fmt::Debug for ffi::Color {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Color")
            .field("r", &self.r)
            .field("g", &self.g)
            .field("b", &self.b)
            .finish()
    }
}

fn print_shared_thing(thing: &ffi::SharedThing) {
    println!("{:#?}", thing.points);
    println!(
        "Pixel 0 is white: {}, pixel is black: {}",
        thing.pixels[0].is_white(),
        thing.pixels[1].is_black()
    );
    println!("{:#?}", ffi::get_name(thing.persons.as_ref().unwrap()));
}

fn make_shared_thing() -> ffi::SharedThing {
    ffi::SharedThing {
        points: Box::new(Points {
            x: vec![1, 2, 3],
            y: vec![4, 5, 6],
        }),
        persons: ffi::make_person(),
        pixels: vec![ffi::Color::white(), ffi::Color::black()],
    }
}

#[inline(always)]
fn rust_echo(val: i32) -> i32 {
    val
}
