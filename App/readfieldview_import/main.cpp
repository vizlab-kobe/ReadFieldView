#include <ReadFieldView/Lib/FieldViewData.h>
#include <ReadFieldView/Lib/ImportFieldViewData.h>
#include <string>
#include <iostream>
#include <kvs/Program>
#include <kvs/Timer>
#include <kvs/File>
#include <kvs/UnstructuredVolumeExporter>
#include <kvs/KVSMLObjectUnstructuredVolume>

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/ExternalFaces>

typedef ReadFieldView::FieldViewData Data;
typedef kvs::UnstructuredVolumeObject Object;

class Program : public kvs::Program
{
    int exec( int argc, char** argv )
    {
      kvs::glut::Application app( argc, argv );
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

        const size_t gindex = 0;
        const size_t vindex = 0;

        int etype = 0;
        if ( data.grid( gindex ).nelements[ Data::Tet ] > 0 ) { etype = Data::Tet; }
        else if ( data.grid( gindex ).nelements[ Data::Hex ] > 0 ) { etype = Data::Hex; }

        Object* TestObject = ReadFieldView::ImportFieldViewData( data, etype, gindex, vindex );
        if ( ! TestObject ) return 1;

	kvs::PolygonObject* object = new kvs::ExternalFaces(  TestObject ); 

	delete TestObject;
	kvs::glut::Screen screen( &app );
	screen.setTitle( "Unstructured Volume Object" );
	screen.registerObject( object );
	screen.show();

	return app.run();

/*
          kvs::File file( argv[1] );
        object->print( std::cout << std::endl );
//        object->write( file.baseName() + ".kvsml", false );

        delete object;

        return 0;

*/
    }
};

int main( int argc, char** argv )
{
    Program program;
    return program.start( argc, argv );
}
