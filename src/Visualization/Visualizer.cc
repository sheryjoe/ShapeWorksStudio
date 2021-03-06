#include <vtkMath.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <Visualization/Visualizer.h>
#include <Data/Shape.h>
#include <Data/MeshManager.h>
#include <QThread>

const QString Visualizer::MODE_ORIGINAL_C( "Original" );
const QString Visualizer::MODE_GROOMED_C( "Groomed" );
const QString Visualizer::MODE_RECONSTRUCTION_C( "Reconstructed" );

//-----------------------------------------------------------------------------
Visualizer::Visualizer(Preferences &prefs) : preferences_(prefs)
{
  this->display_mode_ = Visualizer::MODE_ORIGINAL_C;

  QObject::connect(
    &preferences_, SIGNAL( glyph_properties_changed() ),
    this, SLOT( update_viewer_properties() ) );

  this->show_glyphs_ = true;
  this->show_surface_ = true;

  this->glyph_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  this->selected_point_one_ = -1;
  this->selected_point_two_ = -1;
}

//-----------------------------------------------------------------------------
Visualizer::~Visualizer()
{}

//-----------------------------------------------------------------------------
void Visualizer::set_lightbox( LightboxHandle lightbox )
{
  this->lightbox_ = lightbox;
  this->lightbox_->set_visualizer( this );
}

//-----------------------------------------------------------------------------
void Visualizer::set_project( ProjectHandle project )
{
  this->project_ = project;
}

//-----------------------------------------------------------------------------
void Visualizer::set_display_mode( QString mode )
{
  this->display_mode_ = mode;
}

//-----------------------------------------------------------------------------
void Visualizer::set_center( bool center )
{
  this->center_ = center;
}

//-----------------------------------------------------------------------------
void Visualizer::display_samples()
{

  this->update_viewer_properties();

  QVector < QSharedPointer < DisplayObject > > display_objects;

  QVector < QSharedPointer < Shape > > shapes = this->project_->get_shapes();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<DisplayObject> object = QSharedPointer<DisplayObject>( new DisplayObject() );

    QSharedPointer<Mesh> mesh;
    QString filename;

    //load based on preference, but always load something

    mesh = shapes[i]->get_original_mesh();
    filename = shapes[i]->get_original_filename();
    object->set_correspondence_points( shapes[i]->get_local_correspondence_points() );

    if ( !mesh || this->display_mode_ == Visualizer::MODE_GROOMED_C )
    {
      mesh = shapes[i]->get_groomed_mesh();
      filename = shapes[i]->get_groomed_filename();
    }

    if ( !mesh || this->display_mode_ == Visualizer::MODE_RECONSTRUCTION_C )
    {
      mesh = shapes[i]->get_reconstructed_mesh();
      filename = shapes[i]->get_global_point_filename();

      // use global correspondence points for reconstructed mesh
      object->set_correspondence_points( shapes[i]->get_global_correspondence_points() );
    }
    object->set_mesh( mesh );

    if ( this->display_mode_ != Visualizer::MODE_RECONSTRUCTION_C )
    {
      object->set_exclusion_sphere_centers( shapes[i]->get_exclusion_sphere_centers() );
      object->set_exclusion_sphere_radii( shapes[i]->get_exclusion_sphere_radii() );
    }

    QStringList annotations;
    annotations << filename;
    annotations << "";
    annotations << QString::number( shapes[i]->get_id() );
    annotations << "";
    object->set_annotations( annotations );

    if ( this->center_ )
    {
      object->set_transform( mesh->get_center_transform() );
    }
    display_objects << object;
  }

  this->lightbox_->set_display_objects( display_objects );
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::display_shape( const vnl_vector<double> &points)
{
  
  QVector<DisplayObjectHandle> *list_ptr = getList(points);
  this->lightbox_->set_display_objects( *list_ptr );
  this->lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::display_sample(size_t i)
{
    size_t sample_count = this->project_->get_shapes().size();
    if (sample_count == 0) return;
    i = std::min(sample_count - 1, i);
  
    this->update_viewer_properties();
  
    QVector < QSharedPointer < DisplayObject > > list_ptr;

    QSharedPointer < Shape > shape = this->project_->get_shapes().at(i);

    QSharedPointer<Mesh> mesh;
    QString filename;

    //load based on preference, but always load something

    mesh = shape->get_original_mesh();
    filename = shape->get_original_filename();
    QSharedPointer<DisplayObject> object = QSharedPointer<DisplayObject>( new DisplayObject() );
    object->set_correspondence_points( shape->get_local_correspondence_points() );

    if ( !mesh || this->display_mode_ == Visualizer::MODE_GROOMED_C )
    {
      mesh = shape->get_groomed_mesh();
      filename = shape->get_groomed_filename();
    }  

    if ( !mesh || this->display_mode_ == Visualizer::MODE_RECONSTRUCTION_C )
    {
      mesh = shape->get_reconstructed_mesh();
      filename = shape->get_global_point_filename();

      // use global correspondence points for reconstructed mesh
      object->set_correspondence_points( shape->get_global_correspondence_points() );
    }
    object->set_mesh( mesh );

    if ( this->display_mode_ != Visualizer::MODE_RECONSTRUCTION_C )
    {
      object->set_exclusion_sphere_centers( shape->get_exclusion_sphere_centers() );
      object->set_exclusion_sphere_radii( shape->get_exclusion_sphere_radii() );
    }

    QStringList annotations;
    annotations << filename;
    annotations << "";
    annotations << QString::number( shape->get_id() );
    annotations << "";
    object->set_annotations( annotations );

    if ( this->center_ )
    {
      object->set_transform( mesh->get_center_transform() );
    }
    list_ptr << object;

    this->update_viewer_properties();
    this->lightbox_->set_display_objects( list_ptr );
    this->lightbox_->redraw();
}

//-----------------------------------------------------------------------------
QVector<DisplayObjectHandle> * Visualizer::getList( const vnl_vector<double> &points) {
    QVector<DisplayObjectHandle> *list_ptr = NULL;
	MeshHandle mesh = MeshHandle( new Mesh() );
	mesh->set_poly_data(this->project_->get_mesh_manager()->getMesh(points));

	DisplayObjectHandle object = DisplayObjectHandle( new DisplayObject() );

	object->set_mesh( mesh );
	object->set_correspondence_points( points );

	QStringList annotations;
	//annotations for the 4 corners of the view box
	annotations << "computed shape";
	annotations << "";
	annotations << "";
	annotations << "";
	object->set_annotations( annotations );
	list_ptr = new QVector<DisplayObjectHandle>();
	*list_ptr << object;
    return list_ptr;
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_glyphs( bool show )
{
  this->show_glyphs_ = show;
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_surface( bool show )
{
  this->show_surface_ = show;
}

//-----------------------------------------------------------------------------
void Visualizer::update_viewer_properties()
{
  double size = preferences_.get_glyph_size();
  double quality = preferences_.get_glyph_quality();

  std::cerr << "** update viewer properties, size = " << size << "\n";

  if ( this->lightbox_ )
  {
    foreach( ViewerHandle viewer, this->lightbox_->get_viewers() ) {
      viewer->set_glyph_size_and_quality( size, quality );
      viewer->set_show_glyphs( this->show_glyphs_ );
      viewer->set_show_surface( this->show_surface_ );
	  viewer->set_color_scheme(this->preferences_.get_color_scheme());
    }

	this->update_lut();

    this->lightbox_->redraw();
  }
}

//-----------------------------------------------------------------------------
void Visualizer::update_lut()
{
  int num_points = this->cached_mean_.size() / 3;

  this->glyph_lut_->SetNumberOfTableValues( num_points + 1 );
  this->glyph_lut_->SetTableRange( 0.0, (double)num_points + 1.0 );

  if ( this->selected_point_one_ < 0 )
  {
    this->glyph_lut_->ForceBuild();
  }
  else
  {

    if ( this->selected_point_one_ >= 0 && this->selected_point_two_ >= 0 )
    {
      // measurement mode

      for ( int i = 0; i < num_points; i++ )
      {
        this->glyph_lut_->SetTableValue( i, 0, 0, 0 );

        if ( this->selected_point_one_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 1, 1, 1 );
        }

        if ( this->selected_point_two_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 1, 1, 0 );
        }
      }
    }
    else
    {
      // color by distance from the selected point

      double p1[3];
      p1[0] = this->cached_mean_[ this->selected_point_one_ * 3 + 0];
      p1[1] = this->cached_mean_[ this->selected_point_one_ * 3 + 1];
      p1[2] = this->cached_mean_[ this->selected_point_one_ * 3 + 2];

      std::vector<double> distances;
      double max_distance = 0;
      for ( int i = 0; i < num_points; i++ )
      {

        double p2[3];
        p2[0] = this->cached_mean_[ i * 3 + 0];
        p2[1] = this->cached_mean_[ i * 3 + 1];
        p2[2] = this->cached_mean_[ i * 3 + 2];

        double distance = sqrt( vtkMath::Distance2BetweenPoints( p1, p2 ) );
        distances.push_back( distance );
        if ( distance > max_distance )
        {
          max_distance = distance;
        }
      }

      vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
      lut->SetTableRange( 0, max_distance );
      lut->SetHueRange( 0, 1 );
      lut->SetSaturationRange( 1, 1 );
      lut->SetValueRange( 1, 1 );
      lut->Build();

      for ( int i = 0; i < num_points; i++ )
      {

        unsigned char* color = lut->MapValue( max_distance - distances[i] );

        //this->glyph_lut_->SetTableValue( i, distances[i] / max_distance, 0, 1 );
        this->glyph_lut_->SetTableValue( i, color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f );

        if ( this->selected_point_one_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 1, 1, 1 );
        }

        if ( this->selected_point_two_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 0, 0, 0 );
        }
      }
    }
  }

  this->glyph_lut_->Modified();
  this->lightbox_->set_glyph_lut( this->glyph_lut_ );
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_one( int id )
{
  this->selected_point_one_ = id;
  this->update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_two( int id )
{
  this->selected_point_two_ = id;
  this->update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::compute_measurements()
{
  if ( this->selected_point_one_ >= 0 && this->selected_point_two_ >= 0 )
  {}
}

//-----------------------------------------------------------------------------
void Visualizer::setMean(const vnl_vector<double> & mean)
{
  this->cached_mean_ = mean;
}

void Visualizer::reset_camera() {
	
  if ( this->lightbox_ )
  {
    foreach( ViewerHandle viewer, this->lightbox_->get_viewers() ) {
      viewer->reset_camera();
	  viewer->resetRotation();
    }
  }
  this->update_viewer_properties();
}