// falaise/snemo/cuts/particle_track_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/particle_track_data_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>

// SuperNEMO data models :
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(particle_track_data_cut,"snemo::cut::particle_track_data_cut");

    void particle_track_data_cut::_set_defaults()
    {
      _PTD_label_              = "";
      _mode_                   = MODE_UNDEFINED;
      _charge_type_            = "";
      _particles_range_min_    = -1;
      _particles_range_max_    = -1;
      _calorimeters_range_min_ = -1;
      _calorimeters_range_max_ = -1;
      return;
    }

    void particle_track_data_cut::set_PTD_label(const std::string & PTD_label_)
    {
      _PTD_label_ = PTD_label_;
      return;
    }

    const std::string & particle_track_data_cut::get_PTD_label() const
    {
      return _PTD_label_;
    }

    uint32_t particle_track_data_cut::get_mode() const
    {
      return _mode_;
    }

    bool particle_track_data_cut::is_mode_has_particles() const
    {
      return _mode_ & MODE_HAS_PARTICLES;
    }

    bool particle_track_data_cut::is_mode_range_particles() const
    {
      return _mode_ & MODE_RANGE_PARTICLES;
    }

    bool particle_track_data_cut::is_mode_charge() const
    {
      return _mode_ & MODE_CHARGE;
    }

    bool particle_track_data_cut::is_mode_has_associated_calorimeters() const
    {
      return _mode_ & MODE_HAS_ASSOCIATED_CALORIMETERS;
    }

    bool particle_track_data_cut::is_mode_range_associated_calorimeters() const
    {
      return _mode_ & MODE_RANGE_ASSOCIATED_CALORIMETERS;
    }

    bool particle_track_data_cut::is_mode_has_vertex_on_foil() const
    {
      return _mode_ & MODE_HAS_VERTEX_ON_FOIL;
    }

    particle_track_data_cut::particle_track_data_cut(datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults();
      return;
    }

    particle_track_data_cut::~particle_track_data_cut()
    {
      if (is_initialized()) this->particle_track_data_cut::reset();
      return;
    }

    void particle_track_data_cut::reset()
    {
      _set_defaults();
      this->i_cut::_reset();
      this->i_cut::_set_initialized(false);
      return;
    }

    void particle_track_data_cut::initialize(const datatools::properties & configuration_,
                                             datatools::service_manager  & /* service_manager_ */,
                                             cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Cut '" << get_name() << "' is already initialized ! ");

      this->i_cut::_common_initialize(configuration_);

      if (_PTD_label_.empty())
        {
          DT_THROW_IF(! configuration_.has_key("PTD_label"), std::logic_error,
                      "Missing 'PTD_label' property !");
          set_PTD_label(configuration_.fetch_string("PTD_label"));
        }

      if (_mode_ == MODE_UNDEFINED)
        {
          if (configuration_.has_flag("mode.has_particles"))
            {
              _mode_ |= MODE_HAS_PARTICLES;
            }

          if (configuration_.has_flag("mode.range_particles"))
            {
              _mode_ |= MODE_RANGE_PARTICLES;
            }

          if (configuration_.has_flag("mode.has_charge"))
            {
              _mode_ |= MODE_CHARGE;
            }

          if (configuration_.has_flag("mode.has_associated_calorimeters"))
            {
              _mode_ |= MODE_HAS_ASSOCIATED_CALORIMETERS;
            }

          if (configuration_.has_flag("mode.range_associated_calorimeters"))
            {
              _mode_ |= MODE_RANGE_ASSOCIATED_CALORIMETERS;
            }

          if (configuration_.has_flag("mode.has_vertex_on_foil"))
            {
              _mode_ |= MODE_HAS_VERTEX_ON_FOIL;
            }

          DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error,
                      "Missing at least a 'mode.XXX' property !");

          // mode HAS_PARTICLES:
          if (is_mode_has_particles())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using HAS_PARTICLES mode...");
            } // end if is_mode_has_particles

          // mode RANGE_PARTICLES:
          if (is_mode_range_particles())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_PARTICLES mode...");

              int count = 0;
              if (configuration_.has_key("range_particles.min"))
                {
                  const int nmin = configuration_.fetch_integer("range_particles.min");
                  DT_THROW_IF(nmin < 0, std::range_error, "Invalid min number of particles (" << nmin << ") !");
                  _particles_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key("range_particles.max"))
                {
                  const int nmax = configuration_.fetch_integer("range_particles.max");
                  DT_THROW_IF(nmax < 0, std::range_error, "Invalid max number of particles (" << nmax << ") !");
                  _particles_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF(count == 0, std::logic_error,
                          "Missing 'range_particles.min' or 'range_particles.max' property !");
              if (count == 2 && _particles_range_min_ >= 0 && _particles_range_max_ >= 0)
                {
                  DT_THROW_IF(_particles_range_min_ > _particles_range_max_, std::range_error,
                              "Invalid 'range_particles.min' > 'range_particles.max' values !");
                }
            } // end if is_mode_range_particles

          // mode CHARGE:
          if (is_mode_charge())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using CHARGE mode...");
              DT_THROW_IF(! configuration_.has_key("has_charge.type"), std::logic_error,
                          "Missing 'has_charge.type' property !");
              _charge_type_ = configuration_.fetch_string("has_charge.type");
              DT_THROW_IF (_charge_type_ != "negative" && _charge_type_ != "positive" && _charge_type_ != "undefined",
                           std::logic_error,
                           "Invalid charge type label ! Must be either 'negative', 'positive' or 'undefined'.");
            } // end if is_mode_charge

          // mode HAS_ASSOCIATED_CALORIMETERS:
          if (is_mode_has_associated_calorimeters())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using HAS_ASSOCIATED_CALORIMETERS mode...");
            } // end if is_mode_has_associated_calorimeters

          // mode RANGE_ASSOCIATED_CALORIMETERS:
          if (is_mode_range_associated_calorimeters())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_ASSOCIATED_CALORIMETERS mode...");
              int count = 0;
              if (configuration_.has_key("range_associated_calorimeters.min"))
                {
                  const int nmin = configuration_.fetch_integer("range_associated_calorimeters.min");
                  DT_THROW_IF(nmin < 0, std::range_error,
                              "Invalid min number of associated calorimeters (" << nmin << ") !");
                  _calorimeters_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key("range_associated_calorimeters.max"))
                {
                  const int nmax = configuration_.fetch_integer("range_associated_calorimeters.max");
                  DT_THROW_IF(nmax < 0, std::range_error,
                              "Invalid max number of associated calorimeters (" << nmax << ") !");
                  _calorimeters_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF(count == 0, std::logic_error,
                          "Missing 'range_associated_calorimeters.min' or 'range_associated_calorimeters.max' property !");
              if (count == 2 && _calorimeters_range_min_ >= 0 && _calorimeters_range_max_ >= 0)
                {
                  DT_THROW_IF(_calorimeters_range_min_ > _calorimeters_range_max_, std::logic_error,
                              "Invalid 'range_associated_calorimeters.min' > 'range_associated_calorimeters.max' values !");
                }

            } // end if is_mode_range_associated_calorimeters

          // mode HAS_VERTEX_ON_FOIL:
          if (is_mode_has_vertex_on_foil())
            {
              DT_LOG_DEBUG (get_logging_priority(), "Using HAS_VERTEX_ON_FOIL mode...");
            } // end if is_mode_has_vertex_on_foil
        }

      this->i_cut::_set_initialized(true);
      return;
    }


    int particle_track_data_cut::_accept()
    {
      uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has(_PTD_label_))
        {
          DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _PTD_label_ << "' bank !");
          return cut_returned;
        }

      // Get Particle Track Data bank
      const snemo::datamodel::particle_track_data & PTD
        = ER.get<snemo::datamodel::particle_track_data>(_PTD_label_);

      // Check if the particle track data has some specific particle trajectory :
      bool check_has_particles = true;
      if (is_mode_has_particles())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running HAS_PARTICLES mode...");
          if (!PTD.has_particles())
            {
              check_has_particles = false;
            }
        }

      // Check if the particle track data has some specific category of hits :
      bool check_range_particles = true;
      if (is_mode_range_particles())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_PARTICLES mode...");
          if (!PTD.has_particles())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Particle track data has no particle");
              return cuts::SELECTION_INAPPLICABLE;
            }

          const size_t nparticles = PTD.get_number_of_particles();
          DT_LOG_DEBUG(get_logging_priority(), "Number of particle track = " << nparticles << " (min = "
                       << _particles_range_min_ << " , max = " << _particles_range_max_ << ")");
          bool check = true;
          if (_particles_range_min_ >= 0)
            {
              if (nparticles < (size_t)_particles_range_min_)
                {
                  check = false;
                }
            }
          if (_particles_range_max_ >= 0)
            {
              if (nparticles > (size_t)_particles_range_max_)
                {
                  check = false;
                }
            }
          if (! check)
            {
              DT_LOG_DEBUG(get_logging_priority(), "Event rejected by RANGE_PARTICLES cut!");
              check_range_particles = false;
            }
        }

      // Check if the charge of the particle track :
      bool check_charge = true;
      if (is_mode_charge())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running CHARGE mode...");
          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const int8_t a_charge = iparticle->get().get_charge();
              if (a_charge == snemo::datamodel::particle_track::negative && _charge_type_ != "negative")
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Found negative particle! Reject event!");
                  check_charge = false;
                  break;
                }
              else if (a_charge == snemo::datamodel::particle_track::positive && _charge_type_ != "positive")
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Found positive particle! Reject event!");
                  check_charge = false;
                  break;
                }
              else if (a_charge == snemo::datamodel::particle_track::undefined && _charge_type_ != "undefined")
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Found undefined particle! Reject event!");
                  check_charge = false;
                  break;
                }
            }
        }// end mode CHARGE

      // Check if each particle tracks have an associated calorimeter hit :
      bool check_association = true;
      if (is_mode_has_associated_calorimeters())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running HAS_ASSOCIATED_CALORIMETERS mode...");
          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const snemo::datamodel::particle_track & a_particle = iparticle->get();

              if (!a_particle.has_associated_calorimeters())
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no associated calorimeter !");
                  check_association = false;
                  break;
                }
            }
        }// end mode HAS_ASSOCIATED_CALORIMETERS

      bool check_range_calorimeters = true;
      if (is_mode_range_associated_calorimeters())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_ASSOCIATED_CALORIMETERS mode...");

          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const snemo::datamodel::particle_track & a_particle = iparticle->get();

              if (!a_particle.has_associated_calorimeters())
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no associated calorimeter !");
                  return cuts::SELECTION_INAPPLICABLE;
                }

              const snemo::datamodel::calibrated_calorimeter_hit::collection_type & the_calorimeters
                = a_particle.get_associated_calorimeters();
              const size_t ncalorimeters = the_calorimeters.size();
              DT_LOG_DEBUG(get_logging_priority(),
                           "Number of associated calorimeters = " << ncalorimeters << " (min = "
                           << _calorimeters_range_min_ << " , max = " << _calorimeters_range_max_ << ")");

              bool check = true;
              if (_calorimeters_range_min_ >= 0 && ncalorimeters < (size_t)_calorimeters_range_min_)
                {
                  check = false;
                }
              if (_calorimeters_range_max_ >= 0 && ncalorimeters > (size_t)_calorimeters_range_max_)
                {
                  check = false;
                }
              if (! check)
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Event rejected by RANGE_ASSOCIATED_CALORIMETERS cut!");
                  check_range_calorimeters = false;
                }
            }
        }// end mode RANGE_ASSOCIATED_CALORIMETERS


      // Check if each particle tracks have an vertex on foil :
      bool check_vertex_on_foil = true;
      if (is_mode_has_vertex_on_foil())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running HAS_VERTEX_ON_FOIL mode...");
          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const snemo::datamodel::particle_track & a_particle = iparticle->get();

              if (!a_particle.has_vertices())
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no vertices !");
                  check_vertex_on_foil = false;
                  break;
                }

              bool has_vertex = false;
              const snemo::datamodel::particle_track::vertex_collection_type & the_vertices
                = a_particle.get_vertices();
              for (snemo::datamodel::particle_track::vertex_collection_type::const_iterator
                     ivertex = the_vertices.begin();
                   ivertex != the_vertices.end(); ++ivertex)
                {
                  const geomtools::blur_spot & a_vertex = ivertex->get();
                  const datatools::properties & prop = a_vertex.get_auxiliaries();

                  if (prop.has_flag(snemo::datamodel::particle_track::vertex_on_source_foil_flag()))
                    {
                      has_vertex = true;
                    }
                }

              if (!has_vertex)
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no vertex on the foil !");
                  check_vertex_on_foil = false;
                  break;
                }
            }
        }// end mode HAS_VERTEX_ON_FOIL

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_charge             &&
          check_has_particles      &&
          check_range_particles    &&
          check_association        &&
          check_range_calorimeters &&
          check_vertex_on_foil)
        {
          cut_returned = cuts::SELECTION_ACCEPTED;
        }

      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/