# -*- mode: conf-unix; -*-
# module_0.geom

#########################
# Badic module geometry #
#########################

################################################################
# The SuperNEMO demonstrator module is built from a assembly
# of 5 submodules:
#
#                         ^ y
#                         :
#                         :
#                         :
#                        +-+
#         +----+         |:|         +----+
#         |    |+-------+|:|+-------+|    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |  stacking axis
# --------|----||-------||z||-------||----|-------> x
#  back   |    ||       ||:||       ||    | front
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    ||       ||:||       ||    |
#         |    |+-------+|:|+-------+|    |
#         +----+ tracker |:| tracker +----+
#          calo          +-+          calo
#                       source
#                         :
#                         :
#                         :
#
#
###########################################################
[name="module_basic.model" type="geomtools::stacked_model"]

#@config Simplified demonstrator module

# Stacking parameters:
stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

# Stacked models:
stacked.model_0   : string = "calorimeter_back_submodule.model"
stacked.model_1   : string = "tracker_back_submodule.model"
stacked.model_2   : string = "source_submodule.model"
stacked.model_3   : string = "tracker_front_submodule.model"
stacked.model_4   : string = "calorimeter_front_submodule.model"

# Labels associated to each stacked model:
stacked.label_0   : string = "calorimeter_back_submodule"
stacked.label_1   : string = "tracker_back_submodule"
stacked.label_2   : string = "source_submodule"
stacked.label_3   : string = "tracker_front_submodule"
stacked.label_4   : string = "calorimeter_front_submodule"

# Surrounding material:
material.ref    : string = "lab_air"

# Visibility properties:
visibility.hidden           : boolean = false
visibility.daughters.hidden : boolean = false
visibility.envelop_hidden   : boolean = false
visibility.color            : string  = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.calorimeter_back_submodule  : string  = "[calorimeter_submodule: side=0]"
mapping.daughter_id.tracker_back_submodule      : string  = "[tracker_submodule:     side=0]"
mapping.daughter_id.source_submodule            : string  = "[source_submodule]"
mapping.daughter_id.tracker_front_submodule     : string  = "[tracker_submodule:     side=1]"
mapping.daughter_id.calorimeter_front_submodule : string  = "[calorimeter_submodule: side=1]"


# end of module_0.geom
