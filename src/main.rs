use std::ffi::CStr;

extern crate image;

mod display;
use crate::display::*;

mod imgproc;
use imgproc::{ImageProcessor, ProcessedImage};

fn display_image_file(display: &mut Display, processor: &ImageProcessor, file: &str) -> Result<(), i32> {
    println!("Loading image {}", file);
    let img = image::open(file).expect("Can't load image!").into_rgb8();

    let processed = processor.process(&img);
    display_image(display, &processed)
}

fn display_image(display: &mut Display, image: &ProcessedImage) -> Result<(), i32> {
    display.clear()?;

    for i in 0..image.len() {
        let (x,y) = image.index_to_pos(i);
        display.set_pixel(x, y, image[i])?;
    }
    display.present()?;
    Ok(())

}

fn main() {
    let plugin = unsafe {DisplayPlugin::new("plugins/sdl_display.mod").expect("Failed loading display!")};

    let mut display = Display::new(&plugin).expect("");
    println!("Sucessfully loaded plugin!");

    let args: Vec<String> = std::env::args().collect();
    let file = &args[1];

    let props = display.query_display_properties().expect("Failed to get display properies!");
    let palette = if props.mode == ColorMode_indexed {
        Some(display.query_color_palette().expect("Failed to query color palette!").into())
    } else { None };
    let processor = imgproc::ImageProcessor::new(props, palette);

    let result = display_image_file(&mut display, &processor, file);
    if let Err(code) = result {
        println!("Failed to display image! Code {}", code);
    }

    let mut done = false;
    while !done {
        while let Some(event) = display.poll_events() {
            println!("Got event {}", event.id);
            match event.id {
                EFFIGIA_EVENT_SHUTDOWN_REQUESTED => done = true,
                EFFIGIA_EVENT_NEW_IMAGE_REQUESTED => {
                    let file = unsafe {CStr::from_ptr(event.data.new_image.filename) };
                    let result = display_image_file(&mut display, &processor, file.to_str().unwrap());
                    if let Err(code) = result {
                        println!("Failed to display image! Code {}", code);
                    }
                },
                _ => ()
            }
        };
        std::thread::sleep(std::time::Duration::from_millis(100));
    }
}
