#include "rocksdb/env.h"

#include "rocks/ctypes.hpp"

using namespace rocksdb;

using std::shared_ptr;

extern "C" {
rocks_env_t* rocks_create_default_env() {
  rocks_env_t* result = new rocks_env_t;
  result->rep = Env::Default();
  result->is_default = true;
  return result;
}

rocks_env_t* rocks_create_mem_env() {
  rocks_env_t* result = new rocks_env_t;
  result->rep = rocksdb::NewMemEnv(Env::Default());
  result->is_default = false;
  return result;
}

void rocks_env_set_background_threads(rocks_env_t* env, int n) {
  env->rep->SetBackgroundThreads(n);
}

void rocks_env_set_high_priority_background_threads(rocks_env_t* env, int n) {
  env->rep->SetBackgroundThreads(n, Env::HIGH);
}

void rocks_env_join_all_threads(rocks_env_t* env) { env->rep->WaitForJoin(); }

void rocks_env_destroy(rocks_env_t* env) {
  if (!env->is_default) delete env->rep;
  delete env;
}
}

extern "C" {
rocks_envoptions_t* rocks_envoptions_create() {
  rocks_envoptions_t* opt = new rocks_envoptions_t;
  return opt;
}

void rocks_envoptions_destroy(rocks_envoptions_t* opt) { delete opt; }

void rocks_envoptions_set_use_mmap_reads(rocks_envoptions_t* opt,
                                         unsigned char val) {
  opt->rep.use_mmap_reads = val;
}
void rocks_envoptions_set_use_mmap_writes(rocks_envoptions_t* opt,
                                          unsigned char val) {
  opt->rep.use_mmap_writes = val;
}

void rocks_envoptions_set_use_direct_reads(rocks_envoptions_t* opt,
                                           unsigned char val) {
  opt->rep.use_direct_reads = val;
}
void rocks_envoptions_set_use_direct_writes(rocks_envoptions_t* opt,
                                            unsigned char val) {
  opt->rep.use_direct_writes = val;
}
void rocks_envoptions_set_allow_fallocate(rocks_envoptions_t* opt,
                                          unsigned char val) {
  opt->rep.allow_fallocate = val;
}
// FIXME: bad name?
void rocks_envoptions_set_fd_cloexec(rocks_envoptions_t* opt,
                                     unsigned char val) {
  opt->rep.set_fd_cloexec = val;
}
void rocks_envoptions_set_bytes_per_sync(rocks_envoptions_t* opt,
                                         uint64_t val) {
  opt->rep.bytes_per_sync = val;
}
void rocks_envoptions_set_fallocate_with_keep_size(rocks_envoptions_t* opt,
                                                   unsigned char val) {
  opt->rep.fallocate_with_keep_size = val;
}
void rocks_envoptions_set_compaction_readahead_size(rocks_envoptions_t* opt,
                                                    size_t val) {
  ;
  opt->rep.compaction_readahead_size = val;
}
void rocks_envoptions_set_random_access_max_buffer_size(rocks_envoptions_t* opt,
                                                        size_t val) {
  ;
  opt->rep.random_access_max_buffer_size = val;
}
void rocks_envoptions_set_writable_file_max_buffer_size(rocks_envoptions_t* opt,
                                                        size_t val) {
  opt->rep.writable_file_max_buffer_size = val;
}
/*
void rocks_envoptions_set_rate_limiter(rocks_envoptions_t* opt, ....) {

}
*/
}

extern "C" {
void rocks_logger_destroy(rocks_logger_t* logger) { delete logger; }
}
