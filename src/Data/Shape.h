#ifndef STUDIO_DATA_SHAPE_H
#define STUDIO_DATA_SHAPE_H

#include <QSharedPointer>

#include <Data/Mesh.h>

//! Representation of a single shape/patient.
class Shape
{

public:
  Shape();
  ~Shape();

  /// Import the initial raw image file
  void import_initial_file( QString filename );

  /// Retrieve the initial mesh
  QSharedPointer<Mesh> get_initial_mesh();

  /// Import the groomed raw image file
  void import_groomed_file( QString filename );

  /// Retrieve the groomed mesh
  QSharedPointer<Mesh> get_groomed_mesh();

  /// Import correspondence point file
  bool import_point_file( QString filename );

  /// Retrieve the reconstructed mesh
  QSharedPointer<Mesh> get_reconstructed_mesh();

  /// Get the correspondence points
  vnl_vector<double> get_correspondence_points();

  /// Get the id of this shape
  int get_id();

  /// Set the id of this shape
  void set_id(int id);

private:

  int id_;

  QSharedPointer<Mesh> initial_mesh_;
  QSharedPointer<Mesh> groomed_mesh_;
  QSharedPointer<Mesh> reconstructed_mesh_;

  vnl_vector<double> correspondence_points_;
};

#endif /* STUDIO_DATA_SHAPE_H */
