/*      -*- c++ -*-
 *
 * (C) Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#ifndef __OH_RPC_PARAMS_H
#define __OH_RPC_PARAMS_H


/**********************************************************************
 * struct RpcParams
 * Just a convenient container to pointers array.
 * Used in Marshall Layer.
 *********************************************************************/
struct RpcParams
{            
    explicit RpcParams( void * p1 = 0, void * p2 = 0, void * p3 = 0,
                        void * p4 = 0, void * p5 = 0, void * p6 = 0 )
    {  
        array[0] = p1;
        array[1] = p2;
        array[2] = p3;
        array[3] = p4;
        array[4] = p5;
        array[5] = p6;
    }

    union {
        void * array[6];
        const void * const_array[6];
    };
};


/**********************************************************************
 * struct ClientRpcParams
 * Just a convenient container to pointers array.
 * Allows to set in CTOR only p2-p6
 * For p1 there is a separate function
 * Used in Marshall Layer.
 *********************************************************************/
struct ClientRpcParams : public RpcParams
{            
    explicit ClientRpcParams( void * p2 = 0, void * p3 = 0, void * p4 = 0,
                              void * p5 = 0, void * p6 = 0 )
        : RpcParams( 0, p2, p3, p4, p5, p6 )
    {  
        // empty
    }

    void SetFirst( void * p1 )
    {
        array[0] = p1;
    }
};


#endif /* __OH_RPC_PARAMS_H */

