#include <string>
#include <iostream>
#include <kvs/Program>
#include <kvs/Timer>
#include <kvs/File>
#include <kvs/FieldViewData>
#include <kvs/UnstructuredVolumeImporter>


typedef kvs::FieldViewData Data;
typedef kvs::UnstructuredVolumeObject Object;
typedef kvs::UnstructuredVolumeImporter Importer;

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

        const size_t gindex = 0;
        const size_t vindex = 0;

        int etype = 0;
        if ( data.grid( gindex ).nelements[ Data::Tet ] > 0 ) { etype = Data::Tet; }
        else if ( data.grid( gindex ).nelements[ Data::Hex ] > 0 ) { etype = Data::Hex; }

        data.setImportingElementType( etype );
        data.setImportingGridIndex( gindex );
        data.setImportingVariableIndex( vindex );
        Object* object = new Importer( &data );
        if ( !object ) return 1;

        kvs::File file( argv[1] );
        object->print( std::cout << std::endl );
        //object->write( file.baseName() + ".kvsml", false );

        delete object;

        return 0;
    }
};

int main( int argc, char** argv )
{
    Program program;
    return program.start( argc, argv );
}
