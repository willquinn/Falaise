// test_snemo_datamodel_particle_track_data.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      std::clog << "Test program for the 'particle_track_data' class." << std::endl;

      // bool debug = false;
      // bool verbose = false;

      // int iarg = 1;
      // while (iarg < argc_)
      //   {
      //     string token = argv_[iarg];

      //     if (token[0] == '-')
      //       {
      //         string option = token;
      //         if ((option == "-d") || (option == "--debug"))
      //           {
      //             debug = true;
      //           }
      //         else if ((option == "-v") || (option == "--verbose"))
      //           {
      //             verbose = true;
      //           }
      //         else
      //           {
      //             clog << "warning: ignoring option '" << option << "'!" << endl;
      //           }
      //       }
      //     else
      //       {
      //         string argument = token;
      //         {
      //           clog << "warning: ignoring argument '" << argument << "'!" << endl;
      //         }
      //       }
      //     iarg++;
      //   }

      namespace sdm = snemo::datamodel;

      // Create a handle on some trajectory pattern :
      datatools::handle<sdm::base_trajectory_pattern> hLTP0;
      sdm::line_trajectory_pattern * LTP = new sdm::line_trajectory_pattern;
      // access to the internal geometry model :
      LTP->grab_segment ().set_first (geomtools::vector_3d (3. * CLHEP::mm,
                                                            5. * CLHEP::mm,
                                                            7. * CLHEP::mm));
      LTP->grab_segment ().set_last (geomtools::vector_3d (13. * CLHEP::mm,
                                                           -5. * CLHEP::mm,
                                                           12. * CLHEP::mm));
      hLTP0.reset (LTP);

      datatools::handle<sdm::tracker_trajectory> hTJ0;
      hTJ0.reset (new sdm::tracker_trajectory);
      hTJ0.grab ().set_trajectory_id (0);
      hTJ0.grab ().set_pattern_handle (hLTP0);
      hTJ0.grab ().grab_auxiliaries ().store_flag ("test");
      hTJ0.grab ().grab_auxiliaries ().store ("chi2", 0.234);
      hTJ0.get ().tree_dump (std::clog, "Tracker trajectory : ");

      // Create a handle of fake vertices :
      datatools::handle<geomtools::blur_spot> hV0;
      hV0.reset (new geomtools::blur_spot (geomtools::blur_spot::dimension_three));
      hV0.grab ().set_position (geomtools::vector_3d (1.0 * CLHEP::mm,
                                                     2.0 * CLHEP::mm,
                                                     3.0 * CLHEP::mm));
      hV0.grab ().set_x_error (0.5 * CLHEP::mm);
      hV0.grab ().set_y_error (0.5 * CLHEP::mm);
      hV0.grab ().set_z_error (0.5 * CLHEP::mm);
      hV0.grab ().grab_auxiliaries ().store_flag ("vertex_on_foil");
      hV0.get ().tree_dump (std::clog, "Foil vertex : ");

      datatools::handle<geomtools::blur_spot> hV1;
      hV1.reset (new geomtools::blur_spot (geomtools::blur_spot::dimension_three));
      hV1.grab ().set_position (geomtools::vector_3d (10.0 * CLHEP::mm,
                                                     20.0 * CLHEP::mm,
                                                     30.0 * CLHEP::mm));
      hV1.grab ().set_x_error (2.5 * CLHEP::mm);
      hV1.grab ().set_y_error (2.5 * CLHEP::mm);
      hV1.grab ().set_z_error (2.5 * CLHEP::mm);
      hV1.grab ().grab_auxiliaries ().store_flag ("vertex_on_calorimeter");
      hV1.get ().tree_dump (std::clog, "Calorimeter vertex : ");

      // Create the particle track :
      datatools::handle<sdm::particle_track> hPT0;
      hPT0.reset (new sdm::particle_track);
      hPT0.grab ().set_track_id (0);
      hPT0.grab ().set_charge (sdm::particle_track::positive);
      hPT0.grab ().set_trajectory_handle (hTJ0);
      hPT0.grab ().grab_vertices ().push_back (hV0);
      hPT0.grab ().grab_vertices ().push_back (hV1);
      hPT0.grab ().grab_auxiliaries ().store_flag ("fake_electron");
      hPT0.get ().tree_dump (std::clog, "Particle track : ");

      // Event record :
      sdm::event_record ER;

      // Particle track data bank :
      sdm::particle_track_data & PTD =
        ER.add<sdm::particle_track_data> (sdm::data_info::default_particle_track_data_label());
      PTD.add_particle (hPT0);
      PTD.grab_auxiliaries ().store_flag ("test_ptd");
      PTD.tree_dump (std::clog, "Particle track data :");

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error !" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}