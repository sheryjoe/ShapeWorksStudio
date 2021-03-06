#ifndef STUDIO_GROOM_GROOMTOOL_H
#define STUDIO_GROOM_GROOMTOOL_H

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>
#include "Application/Preferences.h"

class Project;
class Ui_GroomTool;
class ShapeWorksStudioApp;

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool(Preferences& prefs);
  ~GroomTool();

  /// export XML for ShapeWorksGroom
  bool export_xml( QString filename );

  /// set the pointer to the project
  void set_project( QSharedPointer<Project> project );

  /// set the pointer to the application
  void set_app( ShapeWorksStudioApp* app );

Q_SIGNALS:
  void groom_complete();

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged( int state );
  void on_blur_checkbox_stateChanged( int state );
  void handle_thread_complete();
  void handle_progress(int val);
  void handle_error();

  /// Export XML
  void on_export_xml_button_clicked();

  /// Run groom tool
  void on_run_groom_button_clicked();

private:

  Ui_GroomTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;
  Preferences& preferences_;
  QProgressDialog * progress_;
};

#endif /* STUDIO_GROOM_GROOMTOOL_H */
