#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name )
  // TODO: initialize additional member variables
{
  // TODO: implement
  pthread_mutex_init(&mutex, NULL);
}

Table::~Table()
{
  // TODO: implement
  pthread_mutex_destroy(&mutex);
}

void Table::lock()
{
  // TODO: implement
  pthread_mutex_lock(&mutex);
}

void Table::unlock()
{
  // TODO: implement
  pthread_mutex_unlock(&mutex);
}

bool Table::trylock()
{
  // TODO: implement
  // pthread_mutex_trylock returns 0 if successful
  return (pthread_mutex_trylock(&mutex) == 0);
}

void Table::set( const std::string &key, const std::string &value )
{
  // TODO: implement
  table[key] = value;
  uncommitted_map[key] = value;
}

std::string Table::get( const std::string &key )
{
  // TODO: implement
  if (has_key(key)) {
    return table[key];
  } else {
    throw OperationException("key does not exist");
  }
}

bool Table::has_key( const std::string &key )
{
  // TODO: implement
  return (table.find(key) != table.end());
}

void Table::commit_changes()
{
  // TODO: implement
  // empty uncommitted map since data is committed
  uncommitted_map.clear();
}

void Table::rollback_changes()
{
  // TODO: implement
  for (auto &pair : uncommitted_map) {
    table.erase(pair.first); // remove each uncommitted pair in table
  }
  uncommitted_map.clear(); // empty uncommitted map
}
