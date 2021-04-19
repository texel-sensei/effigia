#![allow(non_upper_case_globals)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::os::raw::{c_void};
use std::ptr::null_mut;

pub struct Display<'a> {
    library: &'a DisplayPlugin,
    instance: *mut c_void
}

impl<'a> Display<'a> {
    pub fn new(library: &'a DisplayPlugin) -> Result<Self, ()> {
        unsafe {
            let instance = library.initialize_display();
            if instance == null_mut() {
                return Err(())
            }
            Ok(Self{library, instance})
        }
    }

    pub fn clear(&mut self) -> Result<(), i32> {
        unsafe {
            match self.library.clear(self.instance) {
                0 => Ok(()),
                code => Err(code)
            }
        }
    }

    pub fn set_pixel(&mut self, x: u32, y: u32, color: u32)  -> Result<(), i32>{
        unsafe {
            match self.library.set_pixel(self.instance, x as i32, y as i32, color as i32) {
                0 => Ok(()),
                code => Err(code)
            }
        }
    }

    pub fn present(&mut self) -> Result<(), i32> {
        unsafe {
            match self.library.present(self.instance) {
                0 => Ok(()),
                code => Err(code)
            }
        }
    }
}

impl<'a> Drop for Display<'a> {
    fn drop(&mut self) {
        unsafe {
            self.library.destroy_display(self.instance);
        }
    }
}
