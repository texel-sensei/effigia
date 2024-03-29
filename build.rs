extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    // Invalidate the build whenever the DisplayModule header changes
    println!("cargo:rerun-if-changed=include/DisplayModule.h");
    println!("cargo:rerun-if-changed=include/Event.h");

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("include/DisplayModule.h")
        .header("include/Event.h")
        .dynamic_library_name("DisplayPlugin")
        // Disable generation of automatic tests
        // tests generate UB warnings, see the following issue:
        // https://github.com/rust-lang/rust-bindgen/issues/1651
        .layout_tests(false)
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
