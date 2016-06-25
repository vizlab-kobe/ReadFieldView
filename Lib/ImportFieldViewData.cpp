/*****************************************************************************/
/**
 *  @file   ImportFieldViewData.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#include "ImportFieldViewData.h"
#include <kvs/Assert>


namespace
{

size_t NumberOfNodesPerElement[5] = {
    0, // N/A
    4, // Tetra (1)
    8, // Hexa (2)
    6, // Prism (3)  not supported
    5, // Pyramid (4)
};

kvs::UnstructuredVolumeObject::CellType CellType[5] = {
    kvs::UnstructuredVolumeObject::UnknownCellType, // N/A
    kvs::UnstructuredVolumeObject::Tetrahedra, // Tetra (1)
    kvs::UnstructuredVolumeObject::Hexahedra, // Hexa (2)
    kvs::UnstructuredVolumeObject::Prism, // Prism (3)
    kvs::UnstructuredVolumeObject::Pyramid // Pyramid (4)
};

}

namespace ReadFieldView
{

kvs::UnstructuredVolumeObject* ImportFieldViewData(
    const FieldViewData& data,
    const int etype,
    const size_t gindex,
    const size_t vindex )
{
    KVS_ASSERT( gindex < data.numberOfGrids() );
    KVS_ASSERT( vindex < data.numberOfVariables() );

    typedef kvs::UnstructuredVolumeObject Object;

    const FieldViewData::Grid& grid = data.grid( gindex );
    if ( grid.nelements[ etype ] == 0 )
    {
        std::cerr << "Not found the specified element type." << std::endl;
        return NULL;
    }

    const size_t nnodes = grid.nodes.size();
    const size_t ncells = grid.nelements[ etype ];
    const size_t veclen = grid.variables[ vindex ].type;
    const size_t nnodes_per_cell = ::NumberOfNodesPerElement[ etype ];

    kvs::ValueArray<kvs::Real32> coords( nnodes * 3 );
    for ( size_t i = 0; i < nnodes; i++ )
    {
        coords[ 3 * i + 0 ] = grid.nodes[i].x;
        coords[ 3 * i + 1 ] = grid.nodes[i].y;
        coords[ 3 * i + 2 ] = grid.nodes[i].z;
    }

    kvs::ValueArray<kvs::UInt32> connections( ncells * nnodes_per_cell );
    for ( size_t i = 0, j = 0; i < grid.nelements[0]; i++ )
    {
        KVS_ASSERT( j < ncells * nnodes_per_cell );
        if ( grid.elements[i].type == etype )
        {
            KVS_ASSERT( grid.elements[i].id.size() == nnodes_per_cell );

            if ( etype == 1 ) // tet
            {
                connections[ nnodes_per_cell * j + 0 ] = grid.elements[i].id[0] - 1;
                connections[ nnodes_per_cell * j + 1 ] = grid.elements[i].id[1] - 1;
                connections[ nnodes_per_cell * j + 2 ] = grid.elements[i].id[2] - 1;
                connections[ nnodes_per_cell * j + 3 ] = grid.elements[i].id[3] - 1;
            }
            else if ( etype == 2 ) // hex
            {
                connections[ nnodes_per_cell * j + 0 ] = grid.elements[i].id[0] - 1;
                connections[ nnodes_per_cell * j + 1 ] = grid.elements[i].id[1] - 1;
                connections[ nnodes_per_cell * j + 2 ] = grid.elements[i].id[3] - 1;
                connections[ nnodes_per_cell * j + 3 ] = grid.elements[i].id[2] - 1;
                connections[ nnodes_per_cell * j + 4 ] = grid.elements[i].id[4] - 1;
                connections[ nnodes_per_cell * j + 5 ] = grid.elements[i].id[5] - 1;
                connections[ nnodes_per_cell * j + 6 ] = grid.elements[i].id[7] - 1;
                connections[ nnodes_per_cell * j + 7 ] = grid.elements[i].id[6] - 1;
            }
            else if ( etype == 3 ) // prism
            {
                connections[ nnodes_per_cell * j + 0 ] = grid.elements[i].id[0] - 1;
                connections[ nnodes_per_cell * j + 1 ] = grid.elements[i].id[3] - 1;
                connections[ nnodes_per_cell * j + 2 ] = grid.elements[i].id[5] - 1;
                connections[ nnodes_per_cell * j + 3 ] = grid.elements[i].id[1] - 1;
                connections[ nnodes_per_cell * j + 4 ] = grid.elements[i].id[2] - 1;
                connections[ nnodes_per_cell * j + 5 ] = grid.elements[i].id[4] - 1;
            }
            else if ( etype == 4 ) // pyramid
            {
                connections[ nnodes_per_cell * j + 0 ] = grid.elements[i].id[4] - 1;
                connections[ nnodes_per_cell * j + 1 ] = grid.elements[i].id[0] - 1;
                connections[ nnodes_per_cell * j + 2 ] = grid.elements[i].id[1] - 1;
                connections[ nnodes_per_cell * j + 3 ] = grid.elements[i].id[2] - 1;
                connections[ nnodes_per_cell * j + 4 ] = grid.elements[i].id[3] - 1;
            }
            j++;
        }
    }

    kvs::ValueArray<kvs::Real32> values( nnodes * veclen );
    if ( veclen == 1 )
    {
        KVS_ASSERT( nnodes * veclen == grid.variables[ vindex ].data.size() );
        values = grid.variables[ vindex ].data;
    }
    else if ( veclen == 3 )
    {
        KVS_ASSERT( vindex + 2 < data.numberOfVariables() );
        KVS_ASSERT( grid.variables[ vindex + 0 ].type == grid.variables[ vindex + 1 ].type );
        KVS_ASSERT( grid.variables[ vindex + 1 ].type == grid.variables[ vindex + 2 ].type );
        for ( size_t i = 0; i < nnodes; i++ )
        {
            values[ 3 * i + 0 ] = grid.variables[ vindex + 0 ].data[ i ];
            values[ 3 * i + 1 ] = grid.variables[ vindex + 1 ].data[ i ];
            values[ 3 * i + 2 ] = grid.variables[ vindex + 2 ].data[ i ];
        }
    }

    Object* object = new Object();
    object->setCellType( ::CellType[ etype ] );
    object->setVeclen( veclen );
    object->setNumberOfNodes( nnodes );
    object->setNumberOfCells( ncells );
    object->setCoords( coords );
    object->setConnections( connections );
    object->setValues( values );
    object->updateMinMaxValues();
    object->updateMinMaxCoords();

    return object;
}

kvs::UnstructuredVolumeObject* ImportFieldViewData(
    const FieldViewData& data,
    const int etype,
    const size_t vindex )
{
    KVS_ASSERT( vindex < data.numberOfVariables() );

    typedef kvs::UnstructuredVolumeObject Object;

    const size_t veclen = data.grid(0).variables[ vindex ].type;
    const size_t nnodes_per_cell = ::NumberOfNodesPerElement[ etype ];

    const size_t total_nnodes = data.totalNumberOfNodes();
    const size_t total_ncells = data.totalNumberOfElements( etype );
    kvs::ValueArray<kvs::Real32> coords( total_nnodes * 3 );
    kvs::ValueArray<kvs::Real32> values( total_nnodes * veclen );
    kvs::ValueArray<kvs::UInt32> connections( total_ncells * nnodes_per_cell );

    kvs::Real32* pcoords = coords.data();
    kvs::Real32* pvalues = values.data();
    kvs::UInt32* pconnections = connections.data();

    const size_t ngrids = data.numberOfGrids();
    for ( size_t i = 0; i < ngrids; i++ )
    {
        const FieldViewData::Grid& grid = data.grid(i);

        const size_t nnodes = data.grid(i).nnodes;
        for ( size_t j = 0; j < nnodes; j++ )
        {
            *(pcoords++) = grid.nodes[j].x;
            *(pcoords++) = grid.nodes[j].y;
            *(pcoords++) = grid.nodes[j].z;

            for ( size_t k = 0; k < veclen; k++ )
            {
                *(pvalues++) = grid.variables[ vindex + k ].data[j];
            }
        }

        if ( grid.nelements[etype] != 0 )
        {
            const size_t offset = nnodes * i;
            const size_t ncells = grid.nelements[0];
            for ( size_t j = 0; j < ncells; j++ )
            {
                if ( grid.elements[j].type == etype )
                {
                    if ( etype == 1 ) // tet
                    {
                        *(pconnections++) = offset + grid.elements[j].id[0] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[1] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[2] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[3] - 1;
                    }
                    else if ( etype == 2 ) // hex
                    {
                        *(pconnections++) = offset + grid.elements[j].id[0] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[1] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[3] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[2] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[4] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[5] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[7] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[6] - 1;
                    }
                    else if ( etype == 3 ) // prism
                    {
                        *(pconnections++) = offset + grid.elements[j].id[0] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[3] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[5] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[1] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[2] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[4] - 1;
                    }
                    else if ( etype == 4 ) // pyramid
                    {
                        *(pconnections++) = offset + grid.elements[j].id[4] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[0] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[1] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[2] - 1;
                        *(pconnections++) = offset + grid.elements[j].id[3] - 1;
                    }
                }
            }
        }
    }

    Object* object = new Object();
    object->setCellType( ::CellType[ etype ] );
    object->setVeclen( veclen );
    object->setNumberOfNodes( total_nnodes );
    object->setNumberOfCells( total_ncells );
    object->setCoords( coords );
    object->setConnections( connections );
    object->setValues( values );
    object->updateMinMaxValues();
    object->updateMinMaxCoords();

    return object;
}

} // end of namespace ReadFieldView
