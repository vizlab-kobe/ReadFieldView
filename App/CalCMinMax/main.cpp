#include <ReadFieldView/Lib/FieldViewData.h>
#include <ReadFieldView/Lib/ImportFieldViewData.h>
#include <string>
#include <iostream>
#include <kvs/Program>
#include <kvs/Timer>
#include <kvs/File>
#include <kvs/UnstructuredVolumeExporter>
#include <kvs/KVSMLObjectUnstructuredVolume>

typedef ReadFieldView::FieldViewData Data;
typedef kvs::UnstructuredVolumeObject Object;

#include <cfloat>
#include <kvs/Math>

class Program : public kvs::Program
{
    int exec( int argc, char** argv )
    {
        Data data;
        kvs::Timer timer( kvs::Timer::Start );
        if ( argc > 2 )
        {
            if ( data.read( argv[1], argv[2] ) ) data.print( std::cout << std::endl );
            else return 1;
        }
        else if ( argc > 1 )
        {
            if ( data.read( argv[1] ) ) data.print( std::cout << std::endl );
            else return 1;
        }
        else return 1;
        timer.stop();

        std::cout << "Reading time: " << timer.sec() << " [sec]" << std::endl;


	kvs::Vec3 min_coord = kvs::Vec3::All( FLT_MAX );
	kvs::Vec3 max_coord = kvs::Vec3::All( FLT_MIN );
	kvs::Real32 min_value = FLT_MAX;
	kvs::Real32 max_value = FLT_MIN;
	const Data::ElementType etype = Data::Tet;
	const size_t ngrids = data.numberOfGrids();
	for ( size_t i = 0; i < ngrids; i++ )
	  {
	    const size_t gindex = i;

	    const Data::Grid& grid = data.grid( gindex );

	    const size_t nelements = grid.nelements[etype];
	    if ( nelements != 0 )
	      {
		const size_t vindex = 0;
		const size_t nnodes = grid.nodes.size();
		for ( size_t j = 0; j < nnodes; j++ )
		  {
		    min_coord.x() = kvs::Math::Min( min_coord.x(), grid.nodes[j].x );
		    min_coord.y() = kvs::Math::Min( min_coord.y(), grid.nodes[j].y );
		    min_coord.z() = kvs::Math::Min( min_coord.z(), grid.nodes[j].z );
		    max_coord.x() = kvs::Math::Max( max_coord.x(), grid.nodes[j].x );
		    max_coord.y() = kvs::Math::Max( max_coord.y(), grid.nodes[j].y );
		    max_coord.z() = kvs::Math::Max( max_coord.z(), grid.nodes[j].z );

		    min_value = kvs::Math::Min( min_value, grid.variables[vindex].data[j] );
		    max_value = kvs::Math::Max( max_value, grid.variables[vindex].data[j] );
		  }

	      }
	  }
	std::cout << "min_coords: " << min_coord << std::endl;
	std::cout << "max_coords: " << max_coord << std::endl;
	std::cout << "min_value: " << min_value << std::endl;
	std::cout << "max_value: " << max_value << std::endl;
	return 0;
    }
};

int main( int argc, char** argv )
{
    Program program;
    return program.start( argc, argv );
}
