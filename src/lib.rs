//! Rust-Rocks
//!
//! The RocksDB API in Rustic Style.

/*
#![cfg_attr(feature = "dev", feature(plugin))]
#![cfg_attr(feature = "dev", plugin(clippy))]
#![cfg_attr(not(feature = "dev"), allow(unknown_lints))]
#![allow(not_unsafe_ptr_arg_deref,
         wrong_self_convention,
         doc_markdown)]
*/

#![allow(unused_variables, unused_imports, dead_code)]

extern crate rocks_sys;

#[cfg(test)]
extern crate tempdir;

use std::result;

pub use error::Status;

/// The type returned by RocksDB
pub type Result<T> = result::Result<T, Status>;

pub mod advanced_options;
pub mod cache;
pub mod compaction_filter;
pub mod comparator;
pub mod db;
pub mod env;
pub mod iterator;
pub mod listener;
pub mod merge_operator;
pub mod options;
pub mod rate_limiter;
pub mod slice;
pub mod slice_transform;
pub mod snapshot;
pub mod sst_file_manager;
pub mod sst_file_writer;
pub mod statistics;
pub mod error;
pub mod table;
pub mod table_properties;
pub mod types;
pub mod universal_compaction;
pub mod write_batch;
pub mod write_buffer_manager;
pub mod metadata;
pub mod db_dump_tool;
pub mod perf_level;
pub mod iostats_context;
pub mod perf_context;
pub mod wal_filter;
pub mod filter_policy;

pub mod rocksdb;

// for raw pointer infomation hiding
mod to_raw;


#[doc(hidden)]
pub mod c {
    use std::ptr;
    use std::slice;

    #[no_mangle]
    pub extern "C" fn rust_hello_world() {
        println!("Hello World! from rust");
    }


    #[no_mangle]
    pub unsafe extern "C" fn rust_string_assign(s: *mut String, p: *const u8, len: usize) {
        (*s).reserve(len);
        ptr::copy(p, (*s).as_mut_vec().as_mut_ptr(), len);
        (*s).as_mut_vec().set_len(len);
    }


    #[no_mangle]
    pub unsafe extern "C" fn rust_vec_u8_assign(v: *mut Vec<u8>, p: *const u8, len: usize) {
        // (*v).extend_from_slice(slice::from_raw_parts(p, len))
        (*v).reserve(len);
        ptr::copy(p, (*v).as_mut_ptr(), len);
        (*v).set_len(len);
    }
}
