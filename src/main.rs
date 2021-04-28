use std::ffi::CStr;

extern crate image;
use image::Pixel;

mod display;
use crate::display::*;

#[derive(Debug)]
struct ColorVec(Vec<image::Rgb<u8>>);

// TODO: this isn't a very good distance function as it compares in RGB space
fn color_distance(a: image::Rgb<u8>, b: image::Rgb<u8>) -> i32 {
    a.0.iter().zip(b.0.iter()).map(|(a,b)| ((*a as i32 - *b as i32)).pow(2)).sum::<i32>()
}

impl image::imageops::colorops::ColorMap for ColorVec {
    type Color=image::Rgb<u8>;

    fn index_of(&self, color: &Self::Color) -> usize {
        (0..self.0.len()).min_by_key(|idx| color_distance(self.0[*idx], *color)).expect("Can't have empty color map!")
    }

    fn map_color(&self, color: &mut Self::Color) {
        *color = self.0[self.index_of(color)];
    }
}

fn u32_to_rgb(color: u32) -> image::Rgb<u8> {
    image::Rgb::<u8>::from_channels(
        ((color & 0xFF0000) >> 16) as u8,
        ((color & 0x00FF00) >>  8) as u8,
        ((color & 0x0000FF) >>  0) as u8,
        0
    )
}

impl From<Vec<u32>> for ColorVec {
    fn from(vec: Vec<u32>) -> Self {
        ColorVec{0: vec.iter().map(|c| u32_to_rgb(*c)).collect()}
    }
}

fn conv(color: &image::Rgb<u8>) -> u32 {
    color[2] as u32 | ((color[1] as u32) << 8) | ((color[0] as u32) << 16)
}

fn display_image_file(display: &mut Display, file: &str) -> Result<(), i32> {
    let properties = display.query_display_properties().expect("Failed to query properties");

    println!("Loading image {}", file);
    let mut img = image::open(file)
        .expect("Can't load image!")
        .resize(properties.width as u32, properties.height as u32, image::imageops::FilterType::CatmullRom)
        .into_rgb8();

    let palette: Option<ColorVec>;
    if properties.mode == ColorMode_indexed {
        palette = Some(display.query_color_palette().unwrap().into());
        image::imageops::dither(&mut img, palette.as_ref().unwrap());
    } else {palette = None;}

    display.clear()?;

    let x_off = (properties.width as u32 - img.dimensions().0)/2;
    let y_off = (properties.height as u32 - img.dimensions().1)/2;

    for (x, y, pixel) in img.enumerate_pixels() {
        let color = if let Some(pal) = palette.as_ref() {
            use image::imageops::colorops::ColorMap;
            pal.index_of(pixel) as u32
        } else {
            conv(pixel)
        };
        display.set_pixel(x+x_off, y+y_off, color)?;
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

        let result = display_image_file(&mut display, file);
        if let Err(code) = result {
            println!("Failed to display image! Code {}", code);
        }

        let mut done = false;
        while !done {
            while let Some(event) = display.poll_events() {
                println!("Got event {}", event.id);
                match event.id {
                    0 => done = true,
                    2 => {
                        let file = unsafe {CStr::from_ptr(event.data.new_image.filename) };
                        let result = display_image_file(&mut display, file.to_str().unwrap());
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
