use crate::{DisplayProperties};
use image;
use image::{Pixel};

pub struct ProcessedImage {
    size: (u32, u32),
    data: Vec<u32>
}

impl ProcessedImage {
    pub fn new(size: (u32, u32), data: Vec<u32>) -> Self {
        Self{size, data}
    }

    pub fn index_to_pos(&self, index: usize) -> (u32, u32) {
        (index as u32 % self.size.0, index as u32 / self.size.0)
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }
}

impl std::ops::Index<usize> for ProcessedImage {
    type Output = u32;

    fn index(&self, index: usize) -> &Self::Output {
        &self.data[index]
    }
}

#[derive(Debug)]
pub struct ColorVec(Vec<image::Rgb<u8>>);


pub struct ImageProcessor {
    target_format: DisplayProperties,
    palette: Option<ColorVec>,
}


// Note: Code taken from the image crate.
/// Calculates the width and height an image should be resized to.
/// This preserves aspect ratio, and based on the `fill` parameter
/// will either fill the dimensions to fit inside the smaller constraint
/// (will overflow the specified bounds on one axis to preserve
/// aspect ratio), or will shrink so that both dimensions are
/// completely contained with in the given `width` and `height`,
/// with empty space on one axis.
fn resize_dimensions(width: u32, height: u32, nwidth: u32, nheight: u32, fill: bool) -> (u32, u32) {
    let ratio = u64::from(width) * u64::from(nheight);
    let nratio = u64::from(nwidth) * u64::from(height);

    let use_width = if fill {
        nratio > ratio
    } else {
        nratio <= ratio
    };
    let intermediate = if use_width {
        u64::from(height) * u64::from(nwidth) / u64::from(width)
    } else {
        u64::from(width) * u64::from(nheight) / u64::from(height)
    };
    let intermediate = std::cmp::max(1, intermediate);
    if use_width {
        if intermediate <= u64::from(::std::u32::MAX) {
            (nwidth, intermediate as u32)
        } else {
            (
                (u64::from(nwidth) * u64::from(::std::u32::MAX) / intermediate) as u32,
                ::std::u32::MAX,
            )
        }
    } else if intermediate <= u64::from(::std::u32::MAX) {
        (intermediate as u32, nheight)
    } else {
        (
            ::std::u32::MAX,
            (u64::from(nheight) * u64::from(::std::u32::MAX) / intermediate) as u32,
        )
    }
}

impl ImageProcessor {
    pub fn new(target_format: DisplayProperties, palette: Option<ColorVec>) -> Self {
        Self{ target_format, palette }
    }


    pub fn process<Image>(&self, input: &Image) -> ProcessedImage
        where Image: image::GenericImageView<Pixel=image::Rgb<u8>>
    {
        let mut output = Vec::new();
        let pixel_count = self.target_format.width * self.target_format.height;

        output.resize(pixel_count as usize, u32::default());

        let (w,h) = resize_dimensions(
            input.width(), input.height(),
            self.target_format.width as u32, self.target_format.height as u32,
            false
        );
        println!("Resized {:?} to {:?}", (self.target_format.width, self.target_format.height), (w,h));

        let mut input = image::imageops::resize(
            input,
            w,h,
            image::imageops::FilterType::CatmullRom
        );

        let x_off = (self.target_format.width as u32 - input.dimensions().0)/2;
        let y_off = (self.target_format.height as u32 - input.dimensions().1)/2;
        let index = |x,y| {(y * (self.target_format.width as u32)+x) as usize};

        if let Some(palette) = self.palette.as_ref() {
            image::imageops::dither(&mut input, palette);
            use image::imageops::colorops::ColorMap;
            for (x, y, pixel) in input.enumerate_pixels() {
                let color = palette.index_of(&pixel) as u32;
                output[index(x+x_off, y+y_off)] = color;
            }
        } else {
            for (x, y, pixel) in input.enumerate_pixels() {
                let color = conv(&pixel);
                output[index(x+x_off, y+y_off)] = color;
            }
        };

        ProcessedImage::new((self.target_format.width as u32, self.target_format.height as u32), output)
    }
}

fn conv(color: &image::Rgb<u8>) -> u32 {
    color[2] as u32 | ((color[1] as u32) << 8) | ((color[0] as u32) << 16)
}

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
