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

fn main() {
        let plugin = unsafe {DisplayPlugin::new("plugins/libsdl_display.so").expect("Failed loading display!")};

        let mut display = Display::new(&plugin).expect("");
        println!("Sucessfully loaded plugin!");

        let args: Vec<String> = std::env::args().collect();
        let file = &args[1];
        println!("Loading image {}", file);

        let properties = display.query_display_properties().expect("Failed to query properties");
        println!("Display Properties:\n{:?}", properties);

        let mut img = image::open(file)
            .expect("Can't load image!")
            .resize(properties.width as u32, properties.height as u32, image::imageops::FilterType::CatmullRom)
            .into_rgb8();

        if properties.mode == ColorMode_indexed {
            let palette: ColorVec = display.query_color_palette().unwrap().into();
            println!("Indexed display with palette {:?}", &palette);
            image::imageops::dither(&mut img, &palette);
        }

        display.clear();

        for (x, y, pixel) in img.enumerate_pixels() {
            let color = conv(pixel);
            display.set_pixel(x, y, color);
        }

        display.present();
        std::thread::sleep(std::time::Duration::from_millis(2000));
}
