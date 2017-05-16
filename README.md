# rocks

Make RocksDB really rocks!

working in progress.


## status

> checkbox means DONE, or NEEDLESS TO BE DONE.

- [x] ~~``rocksdb/c.h``~~
  - [x] we use c++ API
- [ ] ``rocksdb/cache.h``
- [ ] ``rocksdb/compaction_filter.h``
- [ ] ``rocksdb/compaction_job_stats.h``
- [ ] ``rocksdb/comparator.h``
- [ ] ``rocksdb/convenience.h``
- [ ] ``rocksdb/db.h``
  - [ ] column family
  - [x] open / close
  - [x] iterator
  - [x] get
  - [x] put
  - [x] write
  - [ ] delete
  - [ ] merge
- [ ] ``rocksdb/db_bench_tool.h``
- [ ] ``rocksdb/db_dump_tool.h``
- [ ] ``rocksdb/env.h``
- [ ] ``rocksdb/experimental.h``
- [ ] ``rocksdb/filter_policy.h``
- [ ] ``rocksdb/flush_block_policy.h``
- [ ] ``rocksdb/iostats_context.h``
- [x] ``rocksdb/iterator.h``
  - [x] adapter for Rust Iterator
- [ ] ``rocksdb/ldb_tool.h``
- [ ] ``rocksdb/listener.h``
- [ ] ``rocksdb/memtablerep.h``
- [ ] ``rocksdb/merge_operator.h``
- [ ] ``rocksdb/metadata.h``
- [x] ``rocksdb/options.h``
  - [x] builder style
- [ ] ``rocksdb/perf_context.h``
- [ ] ``rocksdb/perf_level.h``
- [ ] ``rocksdb/persistent_cache.h``
- [ ] ``rocksdb/rate_limiter.h``
- [x] ~~``rocksdb/slice.h``~~
  - [x] use ``&[u8]`` to replace
- [ ] ``rocksdb/slice_transform.h``
- [ ] ``rocksdb/snapshot.h``
- [ ] ``rocksdb/sst_dump_tool.h``
- [ ] ``rocksdb/sst_file_manager.h``
- [ ] ``rocksdb/sst_file_writer.h``
- [ ] ``rocksdb/statistics.h``
- [x] ``rocksdb/status.h``
  - [ ] Rust style Error? (i.e. remove Status::OK)
- [ ] ``rocksdb/table.h``
- [ ] ``rocksdb/table_properties.h``
- [ ] ``rocksdb/thread_status.h``
- [ ] ``rocksdb/threadpool.h``
- [ ] ``rocksdb/transaction_log.h``
- [x] ~~``rocksdb/types.h``~~
  - [x] a sequence number type, wrapped in Snapshot
- [x] ``rocksdb/version.h``
- [ ] ``rocksdb/wal_filter.h``
- [x] ``rocksdb/write_batch.h``
  - [x] basic functions
  - [x] builder style
  - [ ] batch cf ops
- [ ] ~~``rocksdb/write_batch_base.h``~~
- [ ] ``rocksdb/write_buffer_manager.h``
- [ ] ``rocksdb/universal_compaction.h``
- [ ] ``rocksdb/utilities/backupable_db.h``
- [ ] ``rocksdb/utilities/checkpoint.h``
- [ ] ``rocksdb/utilities/convenience.h``
- [ ] ``rocksdb/utilities/date_tiered_db.h``
- [ ] ``rocksdb/utilities/db_ttl.h``
- [ ] ``rocksdb/utilities/document_db.h``
- [ ] ``rocksdb/utilities/env_librados.h``
- [ ] ``rocksdb/utilities/env_mirror.h``
- [ ] ``rocksdb/utilities/geo_db.h``
- [ ] ``rocksdb/utilities/info_log_finder.h``
- [ ] ``rocksdb/utilities/json_document.h``
- [ ] ``rocksdb/utilities/ldb_cmd.h``
- [ ] ``rocksdb/utilities/ldb_cmd_execute_result.h``
- [ ] ``rocksdb/utilities/leveldb_options.h``
- [ ] ``rocksdb/utilities/lua/rocks_lua_compaction_filter.h``
- [ ] ``rocksdb/utilities/lua/rocks_lua_custom_library.h``
- [ ] ``rocksdb/utilities/lua/rocks_lua_util.h``
- [ ] ``rocksdb/utilities/memory_util.h``
- [ ] ``rocksdb/utilities/object_registry.h``
- [ ] ``rocksdb/utilities/optimistic_transaction_db.h``
- [ ] ``rocksdb/utilities/option_change_migration.h``
- [ ] ``rocksdb/utilities/options_util.h``
- [ ] ``rocksdb/utilities/sim_cache.h``
- [ ] ``rocksdb/utilities/spatial_db.h``
- [ ] ``rocksdb/utilities/stackable_db.h``
- [ ] ``rocksdb/utilities/table_properties_collectors.h``
- [ ] ``rocksdb/utilities/transaction.h``
- [ ] ``rocksdb/utilities/transaction_db.h``
- [ ] ``rocksdb/utilities/transaction_db_mutex.h``
- [ ] ``rocksdb/utilities/utility_db.h``
- [ ] ``rocksdb/utilities/write_batch_with_index.h``
