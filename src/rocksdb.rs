//! The RocksDB prelude entry.

use rocks_sys as ll;

pub use self::ll::version;

pub use error::Status;
pub use db::*;
pub use options::*;
pub use write_batch::WriteBatch;
pub use perf_level::*;
pub use table::*;
pub use slice::PinnableSlice;

pub use super::Result;

#[test]
fn test_version() {
    let v = version();
    assert!(v >= "5.2.1".into());
    println!("ver = {}", v);
}
