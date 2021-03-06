#include "rocksdb/snapshot.h"

#include "rocks/ctypes.hpp"

using namespace rocksdb;

using std::shared_ptr;

extern "C" {

const rocks_snapshot_t* rocks_create_snapshot(rocks_db_t* db) {
  rocks_snapshot_t* result = new rocks_snapshot_t;
  result->rep = db->rep->GetSnapshot();
  return result;
}

void rocks_release_snapshot(rocks_db_t* db, const rocks_snapshot_t* snapshot) {
  db->rep->ReleaseSnapshot(snapshot->rep);
  delete snapshot;
}
}
