/*
 * Shapeworks license
 */

/**
 * @file MeshManager.h
 * @brief Class to manage meshes
 *
 * The MeshManager handles all aspects of mesh generation and caching.
 * It houses the cache and manages threads to work on mesh generation
 * in the background.
 */

#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <vtkSmartPointer.h>

#include <QWaitCondition>

#include <Data/MeshCache.h>
#include <Data/MeshGenerator.h>
#include <Data/MeshWorkQueue.h>
#include <Data/MeshWorker.h>
#include <Application/Preferences.h>

class MeshManager : public QObject
{
  Q_OBJECT

public:
  MeshManager(Preferences& prefs);
  ~MeshManager();

  // pre-generate and cache a mesh for this shape
  void generateMesh( const vnl_vector<double>& shape );

  vtkSmartPointer<vtkPolyData> getMesh( const vnl_vector<double>& shape );

  void clear_cache();

public Q_SLOTS:
  void handle_thread_complete();

signals:
  void new_mesh();

private:

  Preferences& prefs_;

  // cache of shape meshes
  MeshCache meshCache_;

  // our own mesh generator
  MeshGenerator meshGenerator_;

  // queue of meshes to build
  MeshWorkQueue workQueue_;
  
  // the workers
  std::vector<QThread*> threads_;
  
  size_t thread_count_;
};

#endif // ifndef MESH_Manager_H
