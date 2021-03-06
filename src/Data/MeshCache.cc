/*
 * Shapeworks license
 */

// Includes for platform specific functions
#ifdef _WIN32
#include <shlobj.h>
#include <tlhelp32.h>
#include <windows.h>
#include <LMCons.h>
#include <psapi.h>
#else
#include <stdlib.h>
#include <sys/types.h>
#ifndef __APPLE__
#include <unistd.h>
#include <sys/sysinfo.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#endif // ifndef __APPLE__
#endif // ifdef _WIN32

#include <Data/MeshCache.h>

#include <vtkPolyData.h>

bool vnl_vector_compare::operator()( const vnl_vector<double> &a, const vnl_vector<double> &b) const
{
  if ( a.size() < b.size() )
    return true;
  double eps = MeshCache::pref_ref_->get_cache_epsilon();
  for ( unsigned i = 0; i < a.size(); i++ ) {
	if ( (a[i] < b[i]) && ((b[i] - a[i]) > eps))
		return true;
	else if ( b[i] < a[i] && ((a[i] - b[i]) > eps))
		return false;
  }
  return false;
}

long long MeshCache::getTotalPhysicalMemory()
{
#ifdef _WIN32
  MEMORYSTATUSEX memInfo;
  memInfo.dwLength = sizeof( MEMORYSTATUSEX );
  GlobalMemoryStatusEx( &memInfo );
  return memInfo.ullTotalPhys;
#else
#ifdef __APPLE__
  unsigned long long total_physical_memory;
  int name[ 2 ];
  name[ 0 ] = CTL_HW;
  name[ 1 ] = HW_MEMSIZE;
  size_t length = sizeof( unsigned long long );
  sysctl( name, 2, &total_physical_memory, &length, NULL, 0 );
  return total_physical_memory;
#else
  struct sysinfo info;

  sysinfo( &info );
  long long total_physical_memory = info.totalram;
  total_physical_memory *= info.mem_unit;

  return total_physical_memory;
#endif // ifdef __APPLE__
#endif // ifdef _WIN32
}

long long MeshCache::getTotalAddressibleMemory()
{
  if ( sizeof ( void* ) == 8 )
  {
    return 1ULL << 62;
  }
  else
  {
    return 1ULL << 31;
  }
}

long long MeshCache::getTotalAddressiblePhysicalMemory()
{
  long long addressable = MeshCache::getTotalAddressibleMemory();
  long long physical = MeshCache::getTotalPhysicalMemory();
  if ( physical > addressable ) {return addressable; } else {return physical; }
}

Preferences *  MeshCache::pref_ref_ = 0;

MeshCache::MeshCache(Preferences& prefs) : preferences_(prefs)
{
  this->maxMemory = MeshCache::getTotalAddressiblePhysicalMemory();
  this->memorySize = 0;
  this->pref_ref_ = &this->preferences_;
}

vtkSmartPointer<vtkPolyData> MeshCache::getMesh( const vnl_vector<double>& shape )
{
  QMutexLocker locker( &mutex );

  if ( !preferences_.get_cache_enabled() )
  {
    return NULL;
  }

  // search the cache for this shape
  CacheMap::iterator it = this->meshCache.find( shape );
  if ( it == this->meshCache.end() )
  {
    return NULL;
  }

  return meshCache[shape];
}

void MeshCache::insertMesh( const vnl_vector<double>& shape, vtkSmartPointer<vtkPolyData> mesh )
{
  if ( !preferences_.get_cache_enabled() )
  {
    return;
  }

  QMutexLocker locker( &mutex );

  // compute the memory size of this shape
  size_t shapeSize = shape.size() * sizeof( double );
  size_t meshSize = mesh->GetActualMemorySize() * 1024; // given in kb
  size_t combinedSize = ( shapeSize * 2 ) + meshSize;

  this->freeSpaceForAmount( combinedSize );

  this->meshCache[shape] = mesh;
  this->memorySize += combinedSize;
  //std::cerr << "Cache now holds " << this->meshCache.size() << " items\n";

  // add to LRC list
  CacheListItem item;
  item.key = shape;
  item.memorySize = combinedSize;
  this->cacheList.push_back( item );
}

void MeshCache::clear()
{
  QMutexLocker locker( &mutex );

  this->meshCache.clear();
  this->memorySize = 0;
}

void MeshCache::freeSpaceForAmount( size_t allocation )
{
  size_t memoryLimit = ( preferences_.get_cache_memory() / 100.0 ) * this->maxMemory;

  while ( !this->cacheList.empty() && this->memorySize + allocation > memoryLimit )
  {
    CacheListItem item = this->cacheList.back();
    this->memorySize -= item.memorySize;
    this->cacheList.pop_back();
    std::cerr << "erasing item for " << item.memorySize / 1024 << " kb savings\n";
    this->meshCache.erase( item.key );
  }
}
