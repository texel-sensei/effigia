#![allow(non_upper_case_globals)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

extern crate image;


fn conv(color: &image::Rgb<u8>) -> i32 {
    color[2] as i32 | ((color[1] as i32) << 8) | ((color[0] as i32) << 16)
}

fn main() {
    unsafe {
        let plugin = DisplayPlugin::new("plugins/libsdl_display.so").expect("Failed loading display!");
        let instance = plugin.initialize_display();
        println!("Sucessfully loaded plugin!");

        let args: Vec<String> = std::env::args().collect();
        let file = &args[1];
        println!("Loading image {}", file);

        let img = image::open(file).expect("Can't load image!").into_rgb8();

        plugin.clear(instance);

        for (x, y, pixel) in img.enumerate_pixels() {
            let color = conv(pixel);
            plugin.set_pixel(instance, x as i32, y as i32, color);
        }

        plugin.display(instance);
        std::thread::sleep(std::time::Duration::from_millis(2000));
        plugin.destroy_display(instance);
    }
}
