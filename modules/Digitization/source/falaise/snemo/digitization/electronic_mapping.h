// snemo/digitization/electronic_mapping.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H

// - Boost:
#include <boost/cstdint.hpp>
#include <boost/bimap.hpp>


// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>
// - Bayeux/datatools :
#include <bayeux/datatools/logger.h>

// This project
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/ID_convertor.h>


namespace snemo {
  
  namespace digitization {

    class electronic_mapping 
    {

			typedef boost::bimap< geomtools::geom_id, geomtools::geom_id > ID_bimap;
			typedef ID_bimap::value_type ID_doublet;
			
    public :
      /// Default constructor
      electronic_mapping();

      /// Destructor
      virtual ~electronic_mapping();

      /// Initializing
      void initialize(std::string type);
      void initialize();

      /// Check if the object is initialized 
      bool is_initialized();

      /// Reset the object : 
      void reset();

			void init_geiger();
			void init_mcalo();

			geomtools::geom_id convert_GID_to_EID(const geomtools::geom_id & geom_id_);
			geomtools::geom_id convert_EID_to_GID(const geomtools::geom_id & geom_id_);
			void set_geo_manager(const geomtools::manager & mgr_);
			void set_module_number(int);
			
    private :
      
      bool _initialized_; //!< Initialization flag
			datatools::logger::priority _logging_;
			ID_convertor _ID_convertor;
      const geomtools::manager * _geo_manager_;
      int _module_number_;

			ID_bimap _geiger_id_bimap;
			ID_bimap _xcalo_id_bimap;
			ID_bimap _mcalo_id_bimap;
			ID_bimap _gveto_id_bimap;





    };
	       
  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/