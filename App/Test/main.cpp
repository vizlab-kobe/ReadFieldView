#include <ReadFieldView/Lib/FieldViewData.h>
#include <ReadFieldView/Lib/ImportFieldViewData.h>
#include <string>
#include <iostream>
#include <kvs/Program>
#include <kvs/UnstructuredVolumeExporter>
#include <kvs/KVSMLObjectUnstructuredVolume>

typedef ReadFieldView::FieldViewData Data;
typedef kvs::UnstructuredVolumeObject Object;

class Program : public kvs::Program
{
    int exec( int argc, char** argv )
    {
        Data data;
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

        const size_t gindex = 0;
        const size_t vindex = 0;

        int etype = 0;
        if ( data.grid( gindex ).nelements[ Data::Tet ] > 0 ) { etype = Data::Tet; }
        else if ( data.grid( gindex ).nelements[ Data::Hex ] > 0 ) { etype = Data::Hex; }

        Object* object = ReadFieldView::ImportFieldViewData( data, etype, gindex, vindex );
        if ( !object ) return 1;

        object->print( std::cout << std::endl );

        delete object;

        return 0;
    }
};


int main( int argc, char** argv )
{
    Program program;
    return program.start( argc, argv );
}
