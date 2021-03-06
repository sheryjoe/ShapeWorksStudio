#ifndef STUDIO_DATA_PROJECT_H
#define STUDIO_DATA_PROJECT_H

#include <vector>

#include <QSharedPointer>
#include <QVector>
#include "Application/Preferences.h"
#include "Data/MeshManager.h"

class Shape;

class Project;
typedef QSharedPointer< Project > ProjectHandle;

//! Representation of a project.
/*!
 * The Project class encapsulates everything about a project.
 *
 */
class Project : public QObject
{
  Q_OBJECT;

public:

  /// constructor
  Project(Preferences& prefs);

  /// destructor
  ~Project();

  /// set QWidget parent
  void set_parent( QWidget* parent );

  /// save project to file
  bool save_project( QString filename = "" );

  /// load project from file
  bool load_project( QString filename );

  /// load legacy ShapeWorks parameter file
  bool load_legacy( QString filename );

  /// get the pointer to the data manager
  //QSharedPointer<DataManager> get_data_manager();

  /// import files
  void load_original_files( QStringList file_names );

  /// load groomed files
  void load_groomed_files( QStringList file_names );

  /// load point files
  bool load_point_files( QStringList file_names );

  /// remove shapes
  void remove_shapes( QList<int> list );

  /// return all shapes
  QVector<QSharedPointer<Shape> > get_shapes();
  
  void calculate_reconstructed_samples();

  /// reset the project
  void reset();

  /// set the current tool state
  void set_tool_state( QString tool );

  /// get the current tool state
  QString get_tool_state();

  /// get the filename
  QString get_filename();

  /// get the display mode
  void set_display_state( QString mode );

  /// set the display mode
  QString get_display_state();

  /// set the zoom state
  void set_zoom_state( int zoom );

  /// get the zoom state
  int get_zoom_state();

  bool original_present();
  bool groomed_present();
  bool reconstructed_present();
  int get_num_shapes();

  QSharedPointer<MeshManager> get_mesh_manager() { return this->mesh_manager_; }


public Q_SLOTS:
  void handle_clear_cache();
  void handle_new_mesh();

signals:
  /// signal that the data has changed
  void data_changed();
  void update_display();

public:
  // constants
  const static QString DATA_C;
  const static QString GROOM_C;
  const static QString OPTIMIZE_C;
  const static QString ANALYSIS_C;

private:

  Preferences& preferences_;

  void renumber_shapes();

  static bool check_if_legacy( QString filename );

  static bool find_files( QStringList list, QString project_file, QStringList &fixed_list );

  QWidget* parent_;

  /// project filename
  QString filename_;

  /// collection of shapes
  QVector<QSharedPointer<Shape> > shapes_;

  QSharedPointer<MeshManager> mesh_manager_;

  bool original_present_;
  bool groomed_present_;
  bool reconstructed_present_;

  QString tool_state_;
  QString display_state_;
  int zoom_state_;
};

#endif /* STUDIO_DATA_PROJECT_H */
