#![allow(dead_code)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ops::Deref;
use std::os::raw::{c_void};
use std::ptr::{null_mut};
use std::mem::MaybeUninit;

pub struct Display<'a> {
    library: &'a DisplayPlugin,
    instance: *mut c_void
}

pub struct DisplayEventPtr<'a>{
    event: *mut DisplayEvent,
    library: &'a DisplayPlugin
}

impl<'a> DisplayEventPtr<'a> {
    pub fn new(event: *mut DisplayEvent, library: &'a DisplayPlugin) -> Self
    {
        assert!(event != null_mut());
        DisplayEventPtr{event, library}
    }
}

impl<'a> Drop for DisplayEventPtr<'a> {
    fn drop(&mut self) {
        unsafe {self.library.free_event(self.event); }
    }
}

impl<'a> Deref for DisplayEventPtr<'a> {
    type Target = DisplayEvent;

    fn deref(&self) -> &'a Self::Target {
        unsafe { &*self.event }
    }
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
            code_to_result(self.library.set_pixel(self.instance, x as i32, y as i32, color))
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

    pub fn query_color_palette(&self) -> Result<Vec<u32>, i32> {
        let properties = self.query_display_properties()?;
        assert_eq!(properties.mode, ColorMode_indexed);
        let mut palette: Vec<u32> = Vec::with_capacity(properties.color_depth as usize);
        unsafe {
            code_to_result(self.library.query_color_palette(self.instance, palette.as_mut_ptr()))?;
            palette.set_len(palette.capacity());
        }
        Ok(palette)
    }

    pub fn poll_events(&self) -> Option<DisplayEventPtr<'a>> {
        unsafe {
            let event_ptr = self.library.poll_events(self.instance);

            if event_ptr != null_mut() {
                Some(DisplayEventPtr::new(event_ptr, &self.library))
            } else {
                None
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
