#ifndef STUDIO_APPLICATION_PREFERENCES_H
#define STUDIO_APPLICATION_PREFERENCES_H

#include <QSettings>
#include <QString>

//! Application preferences
/*!
 * The Preferences singleton controls all preferences for the application.
 * Values persist via the QSettings class.
 */
class Preferences : public QObject
{
  Q_OBJECT;

public:

  //--- general preferences

  /// Main window size
  QSize get_main_window_size();
  void set_main_window_size( QSize size );
  
  /// last directory used (open/save)
  QString get_last_directory();
  void set_last_directory( QString location );

  //--- analysis preferences

  bool get_cache_enabled();
  void set_cache_enabled( bool enabled );

  int get_cache_memory();
  void setCacheMemory( int value );

  int get_color_scheme();
  void set_color_scheme( int value );

  float get_glyph_quality();
  void set_glyph_quality( float value );

  float get_glyph_size();
  void set_glyph_size( float value );

  int get_num_threads();
  void set_num_threads( int value );

  bool get_parallel_enabled();
  void set_parallel_enabled( bool enabled );

  float get_pca_range();
  void set_pca_range( float value );

  int get_num_pca_steps();
  void set_num_pca_steps( int value );

  int get_num_regression_rteps();
  void set_num_regression_steps( int value );

  QStringList get_recent_files();
  void add_recent_file( QString file );

  /// restore all default values
  void restore_defaults();

  float get_smoothing_amount();
  void set_smoothing_amount(float value);

  float get_cache_epsilon();
  void set_cache_epsilon(float value);

  float get_spacing();
  void set_spacing(float value);

  int get_neighborhood();
  void set_neighborhood(int value);

Q_SIGNALS:
  void color_scheme_changed( int newIndex );
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

public:

  enum { MAX_RECENT_FILES = 4 };
  Preferences();

private:
  /// the QSettings object
  QSettings settings;

};

#endif // ifndef STUDIO_APPLICATION_PREFERENCES_H
