/*****************************************************************************/
/**
 *  @file   ImportFieldViewData.h
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
#pragma once

#include "FieldViewData.h"
#include <string>
#include <kvs/UnstructuredVolumeObject>


namespace ReadFieldView
{

kvs::UnstructuredVolumeObject* ImportFieldViewData(
    const FieldViewData& data,
    const int etype,
    const size_t gindex,
    const size_t vindex );

kvs::UnstructuredVolumeObject* ImportFieldViewData(
    const FieldViewData& data,
    const int etype,
    const size_t vindex );

} // end of namespace ReadFieldView
