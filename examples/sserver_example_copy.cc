// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
// This is an example program for the testing the iostats, perfstats, memory and stats in rocksdb

#include <cstdio>
#include <string>
#include <iostream>

#include "rocksdb/db.h"

// Why is slice important in rocksdb
// The return it->key and it->value return instances of slice rocksdb::Slice.
// Slice is a simple structure that contains a length and a pointer to an external byte array
// Returning a slice is a cheaper alternative to the returning a std::string since we do not need to copy
// potentially large keys and values.
//In addition, rocksdb methods do not allow null terminated C-styke string since rocksdb keys and values are allowed to contain '\0' bytes
// C++ strings and null-terminated C-style strings can be easily converted to a Slice

// rocksdb::Slice s1 = "hello"
// std::string str("world")
// rocksdb::Slice s2 = str

// A slice can be easily converted back to a C++ string 
// std::string str = s1.ToString();
// assert(str == std::string("hello"))

// Be careful when using Slices since it is up to the caller to ensure that the external byte array into which the Slice points
// remains live during the usage of Slice

// Following code is buggy because str is no longer in scope and backing storage for slice s1 disappear
// rocksdb::Slice s1
// if(...){
//  std::string str = ...;
//  s1 = str;
//}
// Use(s1); 

#include "rocksdb/slice.h"
#include "rocksdb/options.h"


using namespace rocksdb;

std::string kDBPath = "/tmp/rdb_simple";

int main(){
  DB* db;
  Options options;

  //create a db if it is not already present
  options.create_if_missing = true;

  //open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // put a key and value in the dbstore
  s = db->Put(WriteOptions(), "k1", "v1");
  assert(s.ok());

  //get the stored value
  std::string value;
  s = db->Get(ReadOptions(),"k1",&value);
  assert(s.ok());

  //atomically applying a set of updates
  {
    WriteBatch batch;
    batch.Delete("k1");
    batch.Put("k2",value);
    s = db->Write(WriteOptions(),&batch);
  }

  std::string value2;
  s = db->Get(ReadOptions(),"k2", &value2);
  assert(s.ok());

  if (value == value2){
    std::cout << "This is true" << std::endl;
  }

}
