set(srcs "src/bc660k_quectel.cpp")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS ""
                    REQUIRES modem)