extern crate image;

mod display;
use crate::display::*;

fn conv(color: &image::Rgb<u8>) -> u32 {
    color[2] as u32 | ((color[1] as u32) << 8) | ((color[0] as u32) << 16)
}

fn main() {
        let plugin = unsafe {DisplayPlugin::new("plugins/libsdl_display.so").expect("Failed loading display!")};

        let mut display = Display::new(&plugin).expect("");
        println!("Sucessfully loaded plugin!");

        let args: Vec<String> = std::env::args().collect();
        let file = &args[1];
        println!("Loading image {}", file);

        let properties = display.query_display_properties().expect("Failed to query properties");
        println!("Display Properties:\n{:?}", properties);

        let img = image::open(file)
            .expect("Can't load image!")
            .resize(properties.width as u32, properties.height as u32, image::imageops::FilterType::CatmullRom)
            .into_rgb8();

        display.clear();

        for (x, y, pixel) in img.enumerate_pixels() {
            let color = conv(pixel);
            display.set_pixel(x, y, color);
        }

        display.present();
        std::thread::sleep(std::time::Duration::from_millis(2000));
}
