#include "rocksdb/db.h"
#include "rocks/ctypes.hpp"

#include "rocks/rust_export.h"

#include <iostream>

using namespace rocksdb;

using std::shared_ptr;

extern "C" {
const char* rocks_column_family_handle_get_name(
    const rocks_column_family_handle_t* handle) {
  return handle->rep->GetName().c_str();
}

uint32_t rocks_column_family_handle_get_id(
    const rocks_column_family_handle_t* handle) {
  return handle->rep->GetID();
}
}

extern "C" {
// DB
rocks_db_t* rocks_db_open(const rocks_options_t* options, const char* name,
                          rocks_status_t** status) {
  DB* db = nullptr;
  Status st = DB::Open(options->rep, std::string(name), &db);
  if (SaveError(status, std::move(st))) {
    return nullptr;
  } else {
    rocks_db_t* result = new rocks_db_t;
    result->rep = db;
    return result;
  }
}

void rocks_db_close(rocks_db_t* db) {
  delete db->rep;
  delete db;
}

rocks_db_t* rocks_db_open_for_read_only(const rocks_options_t* options,
                                        const char* name,
                                        unsigned char error_if_log_file_exist,
                                        rocks_status_t** status) {
  DB* db = nullptr;
  auto st = DB::OpenForReadOnly(options->rep, std::string(name), &db,
                                error_if_log_file_exist);
  if (SaveError(status, std::move(st))) {
    return nullptr;
  } else {
    rocks_db_t* result = new rocks_db_t;
    result->rep = db;
    return result;
  }
}

rocks_db_t* rocks_db_open_column_families(
    const rocks_options_t* db_options, const char* name,
    int num_column_families, const char* const* column_family_names,
    const rocks_cfoptions_t* const* column_family_options,
    rocks_column_family_handle_t** column_family_handles,
    rocks_status_t** status) {
  std::vector<ColumnFamilyDescriptor> column_families;
  for (int i = 0; i < num_column_families; i++) {
    column_families.push_back(ColumnFamilyDescriptor(
        std::string(column_family_names[i]),
        ColumnFamilyOptions(column_family_options[i]->rep)));
  }

  DB* db = nullptr;
  std::vector<ColumnFamilyHandle*> handles;
  if (SaveError(status, DB::Open(DBOptions(db_options->rep), std::string(name),
                                 column_families, &handles, &db))) {
    return nullptr;
  }

  for (size_t i = 0; i < handles.size(); i++) {
    rocks_column_family_handle_t* c_handle = new rocks_column_family_handle_t;
    c_handle->rep = handles[i];
    column_family_handles[i] = c_handle;
  }
  rocks_db_t* result = new rocks_db_t;
  result->rep = db;
  return result;
}

rocks_db_t* rocks_db_open_for_read_only_column_families(
    const rocks_options_t* db_options, const char* name,
    int num_column_families, const char* const* column_family_names,
    const rocks_cfoptions_t* const* column_family_options,
    rocks_column_family_handle_t** column_family_handles,
    unsigned char error_if_log_file_exist, rocks_status_t** status) {
  std::vector<ColumnFamilyDescriptor> column_families;
  for (int i = 0; i < num_column_families; i++) {
    column_families.push_back(ColumnFamilyDescriptor(
        std::string(column_family_names[i]),
        ColumnFamilyOptions(column_family_options[i]->rep)));
  }

  DB* db;
  std::vector<ColumnFamilyHandle*> handles;
  if (SaveError(status,
                DB::OpenForReadOnly(DBOptions(db_options->rep),
                                    std::string(name), column_families,
                                    &handles, &db, error_if_log_file_exist))) {
    return nullptr;
  }

  for (size_t i = 0; i < handles.size(); i++) {
    rocks_column_family_handle_t* c_handle = new rocks_column_family_handle_t;
    c_handle->rep = handles[i];
    column_family_handles[i] = c_handle;
  }
  rocks_db_t* result = new rocks_db_t;
  result->rep = db;
  return result;
}

char** rocks_db_list_column_families(const rocks_options_t* options,
                                     const char* name, size_t* lencfs,
                                     rocks_status_t** status) {
  std::vector<std::string> fams;
  auto st =
      DB::ListColumnFamilies(DBOptions(options->rep), std::string(name), &fams);
  if (SaveError(status, std::move(st))) {
    *lencfs = 0;
    return nullptr;
  }

  *lencfs = fams.size();
  char** column_families =
      static_cast<char**>(malloc(sizeof(char*) * fams.size()));
  for (size_t i = 0; i < fams.size(); i++) {
    column_families[i] = strdup(fams[i].c_str());
  }
  return column_families;
}

void rocks_db_list_column_families_destroy(char** list, size_t len) {
  if (list == nullptr) return;
  for (size_t i = 0; i < len; ++i) {
    free(list[i]);
  }
  free(list);
}

rocks_column_family_handle_t* rocks_db_create_column_family(
    rocks_db_t* db, const rocks_cfoptions_t* column_family_options,
    const char* column_family_name, rocks_status_t** status) {
  rocks_column_family_handle_t* handle = new rocks_column_family_handle_t;
  SaveError(status, db->rep->CreateColumnFamily(
                        ColumnFamilyOptions(column_family_options->rep),
                        std::string(column_family_name), &(handle->rep)));
  return handle;
}

rocks_column_family_handle_t* rocks_db_default_column_family(rocks_db_t* db) {
  return new rocks_column_family_handle_t{db->rep->DefaultColumnFamily()};
}

void rocks_db_drop_column_family(rocks_db_t* db,
                                 rocks_column_family_handle_t* handle,
                                 rocks_status_t** status) {
  SaveError(status, db->rep->DropColumnFamily(handle->rep));
}

void rocks_db_destroy_column_family_handle(rocks_db_t* db,
                                           rocks_column_family_handle_t* handle,
                                           rocks_status_t** status) {
  SaveError(status, db->rep->DestroyColumnFamilyHandle(handle->rep));
}

void rocks_column_family_handle_destroy(rocks_column_family_handle_t* handle) {
  delete handle->rep;
  delete handle;
}

void rocks_db_put(rocks_db_t* db, const rocks_writeoptions_t* options,
                  const char* key, size_t keylen, const char* val,
                  size_t vallen, rocks_status_t** status) {
  SaveError(status,
            db->rep->Put(options->rep, Slice(key, keylen), Slice(val, vallen)));
}

/*
void rocks_db_put_slice(
                  rocks_db_t* db,
                  const rocks_writeoptions_t* options,
                  const Slice* key, const Slice* value,
                  rocks_status_t** status) {
  SaveError(status,
            db->rep->Put(options->rep, *key, *value));
            }*/

void rocks_db_put_cf(rocks_db_t* db, const rocks_writeoptions_t* options,
                     rocks_column_family_handle_t* column_family,
                     const char* key, size_t keylen, const char* val,
                     size_t vallen, rocks_status_t** status) {
  SaveError(status, db->rep->Put(options->rep, column_family->rep,
                                 Slice(key, keylen), Slice(val, vallen)));
}

void rocks_db_delete(rocks_db_t* db, const rocks_writeoptions_t* options,
                     const char* key, size_t keylen, rocks_status_t** status) {
  SaveError(status, db->rep->Delete(options->rep, Slice(key, keylen)));
}

void rocks_db_delete_cf(rocks_db_t* db, const rocks_writeoptions_t* options,
                        rocks_column_family_handle_t* column_family,
                        const char* key, size_t keylen,
                        rocks_status_t** status) {
  SaveError(status, db->rep->Delete(options->rep, column_family->rep,
                                    Slice(key, keylen)));
}

void rocks_db_single_delete(rocks_db_t* db, const rocks_writeoptions_t* options,
                            const char* key, size_t keylen,
                            rocks_status_t** status) {
  SaveError(status, db->rep->SingleDelete(options->rep, Slice(key, keylen)));
}

void rocks_db_single_delete_cf(rocks_db_t* db,
                               const rocks_writeoptions_t* options,
                               rocks_column_family_handle_t* column_family,
                               const char* key, size_t keylen,
                               rocks_status_t** status) {
  SaveError(status, db->rep->SingleDelete(options->rep, column_family->rep,
                                          Slice(key, keylen)));
}

void rocks_db_delete_range_cf(rocks_db_t* db,
                              const rocks_writeoptions_t* options,
                              rocks_column_family_handle_t* column_family,
                              const char* begin_key, size_t begin_keylen,
                              const char* end_key, size_t end_keylen,
                              rocks_status_t** status) {
  SaveError(status, db->rep->DeleteRange(options->rep, column_family->rep,
                                         Slice(begin_key, begin_keylen),
                                         Slice(end_key, end_keylen)));
}

void rocks_db_merge(rocks_db_t* db, const rocks_writeoptions_t* options,
                    const char* key, size_t keylen, const char* val,
                    size_t vallen, rocks_status_t** status) {
  SaveError(status, db->rep->Merge(options->rep, Slice(key, keylen),
                                   Slice(val, vallen)));
}

void rocks_db_merge_cf(rocks_db_t* db, const rocks_writeoptions_t* options,
                       rocks_column_family_handle_t* column_family,
                       const char* key, size_t keylen, const char* val,
                       size_t vallen, rocks_status_t** status) {
  SaveError(status, db->rep->Merge(options->rep, column_family->rep,
                                   Slice(key, keylen), Slice(val, vallen)));
}

void rocks_db_write(
    rocks_db_t* db, const rocks_writeoptions_t* options,
    rocks_raw_writebatch_t* batch,  // raw is pointer, non-raw is a wrapper
    rocks_status_t** status) {
  auto st = db->rep->Write(options->rep, reinterpret_cast<WriteBatch*>(batch));
  SaveError(status, std::move(st));
}

char* rocks_db_get(rocks_db_t* db, const rocks_readoptions_t* options,
                   const char* key, size_t keylen, size_t* vallen,
                   rocks_status_t** status) {
  char* result = nullptr;
  std::string tmp;
  Status st = db->rep->Get(options->rep, Slice(key, keylen), &tmp);
  if (!SaveError(status, std::move(st))) {
    *vallen = tmp.size();
    result = CopyString(tmp);
  }
  return result;
}

char* rocks_db_get_cf(rocks_db_t* db, const rocks_readoptions_t* options,
                      rocks_column_family_handle_t* column_family,
                      const char* key, size_t keylen, size_t* vallen,
                      rocks_status_t** status) {
  char* result = nullptr;
  std::string tmp;
  Status st =
      db->rep->Get(options->rep, column_family->rep, Slice(key, keylen), &tmp);
  if (!SaveError(status, std::move(st))) {
    *vallen = tmp.size();
    result = CopyString(tmp);
  }
  return result;
}

void rocks_db_get_cf_pinnable(rocks_db_t* db,
                              const rocks_readoptions_t* options,
                              rocks_column_family_handle_t* column_family,
                              const char* key, size_t keylen,
                              rocks_pinnable_slice_t* value,
                              rocks_status_t** status) {
  Status st = db->rep->Get(options->rep, column_family->rep, Slice(key, keylen),
                           &value->rep);
  SaveError(status, std::move(st));
}

void rocks_db_multi_get(rocks_db_t* db, const rocks_readoptions_t* options,
                        size_t num_keys, const char* const* keys_list,
                        const size_t* keys_list_sizes, char** values_list,
                        size_t* values_list_sizes, rocks_status_t** status) {
  std::vector<Slice> keys(num_keys);
  for (size_t i = 0; i < num_keys; i++) {
    keys[i] = Slice(keys_list[i], keys_list_sizes[i]);
  }
  std::vector<std::string> values(num_keys);
  std::vector<Status> statuses = db->rep->MultiGet(options->rep, keys, &values);
  for (size_t i = 0; i < num_keys; i++) {
    if (SaveError(status + i, std::move(Status(statuses[i])))) {
      values_list[i] = nullptr;
      values_list_sizes[i] = 0;
    } else {
      values_list[i] = CopyString(values[i]);
      values_list_sizes[i] = values[i].size();
    }
  }
}

void rocks_db_multi_get_cf(
    rocks_db_t* db, const rocks_readoptions_t* options,
    const rocks_column_family_handle_t* const* column_families, size_t num_keys,
    const char* const* keys_list, const size_t* keys_list_sizes,
    char** values_list, size_t* values_list_sizes, rocks_status_t** status) {
  std::vector<Slice> keys(num_keys);
  std::vector<ColumnFamilyHandle*> cfs(num_keys);
  for (size_t i = 0; i < num_keys; i++) {
    keys[i] = Slice(keys_list[i], keys_list_sizes[i]);
    cfs[i] = column_families[i]->rep;
  }
  std::vector<std::string> values(num_keys);
  std::vector<Status> statuses =
      db->rep->MultiGet(options->rep, cfs, keys, &values);
  for (size_t i = 0; i < num_keys; i++) {
    if (SaveError(status + i, std::move(Status(statuses[i])))) {
      values_list[i] = nullptr;
      values_list_sizes[i] = 0;
    } else {
      values_list[i] = CopyString(values[i]);
      values_list_sizes[i] = values[i].size();
    }
  }
}

unsigned char rocks_db_key_may_exist(rocks_db_t* db,
                                     const rocks_readoptions_t* options,
                                     const char* key, size_t key_len,
                                     char** value, size_t* value_len,
                                     unsigned char* value_found) {
  bool found;
  std::string val;
  bool ret =
      db->rep->KeyMayExist(options->rep, Slice(key, key_len), &val, &found);
  if (ret && value != nullptr) {
    *value_len = val.size();
    *value = CopyString(val);
  }
  if (value_found != nullptr) {
    *value_found = found;
  }
  return ret;
}

unsigned char rocks_db_key_may_exist_cf(
    rocks_db_t* db, const rocks_readoptions_t* options,
    const rocks_column_family_handle_t* column_family, const char* key,
    size_t key_len, char** value, size_t* value_len,
    unsigned char* value_found) {
  if (value_found != nullptr) {
    std::string val;
    bool ret =
        db->rep->KeyMayExist(options->rep, column_family->rep,
                             Slice(key, key_len), &val, (bool*)value_found);
    if (ret) {
      *value_len = val.size();
      *value = CopyString(val);
    }
    return ret;
  } else {
    return db->rep->KeyMayExist(options->rep, column_family->rep,
                                Slice(key, key_len), nullptr);
  }
}

rocks_iterator_t* rocks_db_create_iterator(rocks_db_t* db,
                                           const rocks_readoptions_t* options) {
  rocks_iterator_t* result = new rocks_iterator_t;
  result->rep = db->rep->NewIterator(options->rep);
  return result;
}

rocks_iterator_t* rocks_db_create_iterator_cf(
    rocks_db_t* db, const rocks_readoptions_t* options,
    rocks_column_family_handle_t* column_family) {
  rocks_iterator_t* result = new rocks_iterator_t;
  result->rep = db->rep->NewIterator(options->rep, column_family->rep);
  return result;
}

void rocks_db_create_iterators(rocks_db_t* db, rocks_readoptions_t* opts,
                               rocks_column_family_handle_t** column_families,
                               rocks_iterator_t** iterators, size_t size,
                               rocks_status_t** status) {
  std::vector<ColumnFamilyHandle*> column_families_vec;
  for (size_t i = 0; i < size; i++) {
    column_families_vec.push_back(column_families[i]->rep);
  }

  std::vector<Iterator*> res;
  Status st = db->rep->NewIterators(opts->rep, column_families_vec, &res);
  assert(res.size() == size);
  if (SaveError(status, std::move(st))) {
    return;
  }

  for (size_t i = 0; i < size; i++) {
    iterators[i] = new rocks_iterator_t;
    iterators[i]->rep = res[i];
  }
}

rocks_snapshot_t* rocks_db_get_snapshot(rocks_db_t* db) {
  rocks_snapshot_t* result = new rocks_snapshot_t;
  result->rep = db->rep->GetSnapshot();
  return result;
}

// also destroy
void rocks_db_release_snapshot(rocks_db_t* db, rocks_snapshot_t* snapshot) {
  db->rep->ReleaseSnapshot(snapshot->rep);
  delete snapshot;
}

// property

unsigned char rocks_db_get_property(rocks_db_t* db, const char* prop,
                                    const size_t prop_len, void* value) {
  std::string cval;
  auto has = db->rep->GetProperty(Slice(prop, prop_len), &cval);
  if (has) {
    rust_string_assign(value, cval.data(), cval.size());
  }
  return has;
}

unsigned char rocks_db_get_property_cf(rocks_db_t* db,
                                       rocks_column_family_handle_t* cf,
                                       const char* prop, const size_t prop_len,
                                       void* value) {
  std::string cval;
  auto has = db->rep->GetProperty(cf->rep, Slice(prop, prop_len), &cval);
  if (has) {
    rust_string_assign(value, cval.data(), cval.size());
  }
  return has;
}

unsigned char rocks_db_get_int_property(rocks_db_t* db, const char* prop,
                                        const size_t prop_len,
                                        uint64_t* value) {
  auto has = db->rep->GetIntProperty(Slice(prop, prop_len), value);
  return has;
}

unsigned char rocks_db_get_int_property_cf(rocks_db_t* db,
                                           rocks_column_family_handle_t* cf,
                                           const char* prop,
                                           const size_t prop_len,
                                           uint64_t* value) {
  auto has = db->rep->GetIntProperty(cf->rep, Slice(prop, prop_len), value);
  return has;
}

unsigned char rocks_db_get_aggregated_int_property(rocks_db_t* db,
                                                   const char* prop,
                                                   const size_t prop_len,
                                                   uint64_t* value) {
  auto has = db->rep->GetAggregatedIntProperty(Slice(prop, prop_len), value);
  return has;
}

void rocks_db_compact_range(rocks_db_t* db, const char* start_key,
                            size_t start_key_len, const char* limit_key,
                            size_t limit_key_len) {
  Slice a, b;
  db->rep->CompactRange(
      CompactRangeOptions(),
      // Pass nullptr Slice if corresponding "const char*" is nullptr
      (start_key ? (a = Slice(start_key, start_key_len), &a) : nullptr),
      (limit_key ? (b = Slice(limit_key, limit_key_len), &b) : nullptr));
}

void rocks_db_compact_range_cf(rocks_db_t* db,
                               rocks_column_family_handle_t* column_family,
                               const char* start_key, size_t start_key_len,
                               const char* limit_key, size_t limit_key_len) {
  Slice a, b;
  db->rep->CompactRange(
      CompactRangeOptions(), column_family->rep,
      // Pass nullptr Slice if corresponding "const char*" is nullptr
      (start_key ? (a = Slice(start_key, start_key_len), &a) : nullptr),
      (limit_key ? (b = Slice(limit_key, limit_key_len), &b) : nullptr));
}

void rocks_db_compact_range_opt(rocks_db_t* db,
                                rocks_compactrange_options_t* opt,
                                const char* start_key, size_t start_key_len,
                                const char* limit_key, size_t limit_key_len,
                                rocks_status_t** status) {
  Slice a, b;
  auto st = db->rep->CompactRange(
      opt->rep,
      // Pass nullptr Slice if corresponding "const char*" is nullptr
      (start_key ? (a = Slice(start_key, start_key_len), &a) : nullptr),
      (limit_key ? (b = Slice(limit_key, limit_key_len), &b) : nullptr));
  SaveError(status, std::move(st));
}

void rocks_db_compact_range_opt_cf(rocks_db_t* db,
                                   rocks_compactrange_options_t* opt,
                                   rocks_column_family_handle_t* column_family,
                                   const char* start_key, size_t start_key_len,
                                   const char* limit_key, size_t limit_key_len,
                                   rocks_status_t** status) {
  Slice a, b;
  auto st = db->rep->CompactRange(
      opt->rep, column_family->rep,
      // Pass nullptr Slice if corresponding "const char*" is nullptr
      (start_key ? (a = Slice(start_key, start_key_len), &a) : nullptr),
      (limit_key ? (b = Slice(limit_key, limit_key_len), &b) : nullptr));
  SaveError(status, std::move(st));
}

  void rocks_db_compact_files(rocks_db_t* db,
                              rocks_compaction_options_t* opt,
                              size_t num_files,
                              const char* const* file_names,
                              const size_t* file_name_lens,
                              const int output_level,
                              const int output_path_id,
                              rocks_status_t** status) {
    std::vector<std::string> input_file_names;
    for (auto i = 0; i < num_files ;i++) {
      input_file_names.push_back(std::string(file_names[i], file_name_lens[i]));
    }
    auto st = db->rep->CompactFiles(opt->rep, input_file_names, output_level, output_path_id);
    SaveError(status, std::move(st));
  }

void rocks_db_pause_background_work(rocks_db_t* db, rocks_status_t** status) {
  SaveError(status, std::move(db->rep->PauseBackgroundWork()));
}

void rocks_db_continue_background_work(rocks_db_t* db,
                                       rocks_status_t** status) {
  SaveError(status, std::move(db->rep->ContinueBackgroundWork()));
}

void rocks_db_enable_auto_compaction(
    rocks_db_t* db, const rocks_column_family_handle_t* const* column_families,
    size_t cf_len, rocks_status_t** status) {
  std::vector<ColumnFamilyHandle*> cfs;
  for (auto i = 0; i < cf_len; i++) {
    cfs.push_back(column_families[i]->rep);
  }
  SaveError(status, std::move(db->rep->EnableAutoCompaction(cfs)));
}

int rocks_db_number_levels_cf(rocks_db_t* db,
                              rocks_column_family_handle_t* column_family) {
  return db->rep->NumberLevels(column_family->rep);
}

int rocks_db_number_levels(rocks_db_t* db) { return db->rep->NumberLevels(); }

int rocks_db_max_mem_compaction_level_cf(
    rocks_db_t* db, rocks_column_family_handle_t* column_family) {
  return db->rep->MaxMemCompactionLevel(column_family->rep);
}

int rocks_db_max_mem_compaction_level(rocks_db_t* db) {
  return db->rep->MaxMemCompactionLevel();
}

int rocks_db_level0_stop_write_trigger_cf(
    rocks_db_t* db, rocks_column_family_handle_t* column_family) {
  return db->rep->Level0StopWriteTrigger(column_family->rep);
}

int rocks_db_level0_stop_write_trigger(rocks_db_t* db) {
  return db->rep->Level0StopWriteTrigger();
}

void rocks_db_compact_range_cf_opt(rocks_db_t* db,
                                   rocks_column_family_handle_t* column_family,
                                   rocks_compactrange_options_t* opt,
                                   const char* start_key, size_t start_key_len,
                                   const char* limit_key, size_t limit_key_len,
                                   rocks_status_t** status) {
  Slice a, b;
  auto st = db->rep->CompactRange(
      opt->rep, column_family->rep,
      // Pass nullptr Slice if corresponding "const char*" is nullptr
      (start_key ? (a = Slice(start_key, start_key_len), &a) : nullptr),
      (limit_key ? (b = Slice(limit_key, limit_key_len), &b) : nullptr));
  SaveError(status, std::move(st));
}

const char* rocks_db_get_name(rocks_db_t* db, size_t* len) {
  auto name = db->rep->GetName();
  *len = name.size();
  return name.data();
}

void rocks_db_flush(rocks_db_t* db, rocks_flushoptions_t* options,
                    rocks_status_t** status) {
  SaveError(status, std::move(db->rep->Flush(options->rep)));
}

void rocks_db_flush_cf(rocks_db_t* db, rocks_flushoptions_t* options,
                       rocks_column_family_handle_t* column_family,
                       rocks_status_t** status) {
  SaveError(status,
            std::move(db->rep->Flush(options->rep, column_family->rep)));
}

void rocks_db_sync_wal(rocks_db_t* db, rocks_status_t** status) {
  SaveError(status, std::move(db->rep->SyncWAL()));
}

uint64_t rocks_db_get_latest_sequence_number(rocks_db_t* db) {
  return db->rep->GetLatestSequenceNumber();
}

void rocks_db_disable_file_deletions(rocks_db_t* db, rocks_status_t** status) {
  SaveError(status, std::move(db->rep->DisableFileDeletions()));
}

void rocks_db_enable_file_deletions(rocks_db_t* db, unsigned char force,
                                    rocks_status_t** status) {
  SaveError(status, std::move(db->rep->EnableFileDeletions(force)));
}

cxx_string_vector_t* rocks_db_get_live_files(rocks_db_t* db,
                                             unsigned char flush_memtable,
                                             uint64_t* manifest_file_size,
                                             rocks_status_t** status) {
  cxx_string_vector_t* files = new cxx_string_vector_t;

  auto st =
      db->rep->GetLiveFiles(files->rep, manifest_file_size, flush_memtable);
  if (SaveError(status, std::move(st))) {
    delete files;
    return nullptr;
  }
  return files;
}

void rocks_db_delete_file(rocks_db_t* db, const char* name, size_t name_len,
                          rocks_status_t** status) {
  SaveError(status, db->rep->DeleteFile(std::string(name, name_len)));
}

const rocks_livefiles_t* rocks_db_get_livefiles_metadata(rocks_db_t* db) {
  rocks_livefiles_t* result = new rocks_livefiles_t;
  db->rep->GetLiveFilesMetaData(&result->rep);
  return result;
}

const rocks_column_family_metadata_t* rocks_db_get_column_family_metadata(
    rocks_db_t* db, rocks_column_family_handle_t* column_family) {
  rocks_column_family_metadata_t* result = new rocks_column_family_metadata_t;
  db->rep->GetColumnFamilyMetaData(column_family->rep, &result->rep);
  return result;
}

void rocks_db_ingest_external_file(
    rocks_db_t* db, const char* const* file_list, const size_t* file_list_sizes,
    size_t file_len, const rocks_ingestexternalfile_options_t* options,
    rocks_status_t** status) {
  std::vector<std::string> external_files;
  for (auto i = 0; i < file_len; i++) {
    external_files.push_back(std::string(file_list[i], file_list_sizes[i]));
  }
  auto st = db->rep->IngestExternalFile(external_files, options->rep);
  SaveError(status, std::move(st));
}

void rocks_db_ingest_external_file_cf(
    rocks_db_t* db, rocks_column_family_handle_t* column_family,
    const char* const* file_list, const size_t* file_list_sizes,
    size_t file_len, const rocks_ingestexternalfile_options_t* options,
    rocks_status_t** status) {
  std::vector<std::string> external_files;
  for (auto i = 0; i < file_len; i++) {
    external_files.push_back(std::string(file_list[i], file_list_sizes[i]));
  }
  auto st = db->rep->IngestExternalFile(column_family->rep, external_files,
                                        options->rep);
  SaveError(status, std::move(st));
}

void rocks_db_get_db_identity(rocks_db_t* db,
                              void* identity,  // *mut String
                              rocks_status_t** status) {
  std::string id;
  auto st = db->rep->GetDbIdentity(id);
  if (!SaveError(status, std::move(st))) {
    rust_string_assign(identity, id.data(), id.size());
  }
}

// public functions
void rocks_destroy_db(const rocks_options_t* options, const char* name,
                      rocks_status_t** status) {
  auto st = DestroyDB(name, options->rep);
  SaveError(status, std::move(st));
}

void rocks_repair_db(const rocks_options_t* options, const char* name,
                     rocks_status_t** status) {
  auto st = RepairDB(name, options->rep);
  SaveError(status, std::move(st));
}
}
