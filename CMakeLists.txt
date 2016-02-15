CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(loss-calculation)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_STATIC_RUNTIME ON)
FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system program_options)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

FIND_PACKAGE(gdal)
INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})

SET (LOSS_HEADERS
		./util/coordinate_2d.h
		./util/default_construction_functor.h
		./util/dereference_proxy.h
		./util/exceptions.h
		./util/gdal_raster_iterator.h
		./util/gdal_raster_lru.h
		./util/gdal_raster.h
		./util/raster_iterator.h
		./util/raster_traits.h
		./util/zip_range.h
		./util/utility.h
    )

SET (LOSS_SOURCES 
        risk_calc.cpp
        )
        
ADD_EXECUTABLE(loss ${LOSS_SOURCES} ${LOSS_HEADERS})
TARGET_LINK_LIBRARIES(loss "${GDAL_LIBRARY}" "${Boost_LIBRARIES}" )

set_property(TARGET loss PROPERTY CXX_STANDARD 11)
