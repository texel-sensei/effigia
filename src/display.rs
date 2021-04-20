#![allow(non_upper_case_globals)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::os::raw::{c_void};
use std::ptr::null_mut;
use std::mem::MaybeUninit;

pub struct Display<'a> {
    library: &'a DisplayPlugin,
    instance: *mut c_void
}

fn code_to_result(code: i32) -> Result<(),i32> {
    match code {
        0 => Ok(()),
        code => Err(code)
    }
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
            code_to_result(self.library.clear(self.instance))
        }
    }

    pub fn set_pixel(&mut self, x: u32, y: u32, color: u32)  -> Result<(), i32>{
        unsafe {
            code_to_result(self.library.set_pixel(self.instance, x as i32, y as i32, color as i32))
        }
    }

    pub fn present(&mut self) -> Result<(), i32> {
        unsafe {
            code_to_result(self.library.present(self.instance))
        }
    }

    pub fn query_display_properties(&self) -> Result<DisplayProperties, i32> {
        unsafe {
            let mut properties = MaybeUninit::<DisplayProperties>::uninit();
            code_to_result(self.library.query_display_properties(self.instance, properties.as_mut_ptr()))?;
            Ok(properties.assume_init())
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
