/*
 * Copyright 2010 Rockchip Electronics S.LSI Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MPP_BUFFER_IMPL_H__
#define __MPP_BUFFER_IMPL_H__

#include "mpp_list.h"
#include "mpp_common.h"
#include "mpp_allocator.h"

#define MPP_BUF_DBG_FUNCTION            (0x00000001)

#define mpp_buf_dbg(flag, fmt, ...)     _mpp_dbg(mpp_buffer_debug, flag, fmt, ## __VA_ARGS__)
#define mpp_buf_dbg_f(flag, fmt, ...)   _mpp_dbg(mpp_buffer_debug, flag, fmt, ## __VA_ARGS__)

#define MPP_BUF_FUNCTION_ENTER()        mpp_buf_dbg_f(MPP_BUF_DBG_FUNCTION, "enter\n")
#define MPP_BUF_FUNCTION_LEAVE()        mpp_buf_dbg_f(MPP_BUF_DBG_FUNCTION, "leave\n")
#define MPP_BUF_FUNCTION_LEAVE_OK()     mpp_buf_dbg_f(MPP_BUF_DBG_FUNCTION, "success\n")
#define MPP_BUF_FUNCTION_LEAVE_FAIL()   mpp_buf_dbg_f(MPP_BUF_DBG_FUNCTION, "failed\n")

typedef struct MppBufferImpl_t          MppBufferImpl;
typedef struct MppBufferGroupImpl_t     MppBufferGroupImpl;

// use index instead of pointer to avoid invalid pointer
struct MppBufferImpl_t {
    char                tag[MPP_TAG_SIZE];
    const char          *caller;
    RK_U32              group_id;
    MppBufferMode       mode;

    MppBufferInfo       info;

    /* used for buf on group reset mode
       set disard value to 1 when frame refcount no zero ,
       we will delay relesase buffer after refcount to zero,
       not put this buf to unused list
     */
    RK_S32              discard;
    // used flag is for used/unused list detection
    RK_U32              used;
    RK_U32              internal;
    RK_S32              ref_count;
    struct list_head    list_status;
};

struct MppBufferGroupImpl_t {
    char                tag[MPP_TAG_SIZE];
    const char          *caller;
    RK_U32              group_id;
    MppBufferMode       mode;
    MppBufferType       type;
    // used in limit mode only
    size_t              limit_size;
    RK_S32              limit_count;
    // status record
    size_t              limit;
    size_t              usage;
    RK_S32              count;

    MppAllocator        allocator;
    MppAllocatorApi     *alloc_api;

    RK_S32              count_used;
    RK_S32              count_unused;

    // link to the other MppBufferGroupImpl
    struct list_head    list_group;

    // link to list_status in MppBufferImpl
    struct list_head    list_used;
    struct list_head    list_unused;
};

#ifdef __cplusplus
extern "C" {
#endif

extern RK_U32 mpp_buffer_debug;

/*
 *  mpp_buffer_create       : create a unused buffer with parameter tag/size/data
 *                            buffer will be register to unused list
 *
 *  mpp_buffer_destroy      : destroy a buffer, it must be on unused status
 *
 *  mpp_buffer_get_unused   : get unused buffer with size. it will first search
 *                            the unused list. if failed it will create on from
 *                            group allocator.
 *
 *  mpp_buffer_ref_inc      : increase buffer's reference counter. if it is unused
 *                            then it will be moved to used list.
 *
 *  mpp_buffer_ref_dec      : decrease buffer's reference counter. if the reference
 *                            reduce to zero buffer will be moved to unused list.
 *
 * normal call flow will be like this:
 *
 * mpp_buffer_create        - create a unused buffer
 * mpp_buffer_get_unused    - get the unused buffer
 * mpp_buffer_ref_inc/dec   - use the buffer
 * mpp_buffer_destory       - destroy the buffer
 */
MPP_RET mpp_buffer_create(const char *tag, const char *caller, RK_U32 group_id, MppBufferInfo *info);
MPP_RET mpp_buffer_destroy(MppBufferImpl *buffer);
MPP_RET mpp_buffer_ref_inc(MppBufferImpl *buffer);
MPP_RET mpp_buffer_ref_dec(MppBufferImpl *buffer);
MppBufferImpl *mpp_buffer_get_unused(MppBufferGroupImpl *p, size_t size);

MPP_RET mpp_buffer_group_init(MppBufferGroupImpl **group, const char *tag, const char *caller, MppBufferMode mode, MppBufferType type);
MPP_RET mpp_buffer_group_deinit(MppBufferGroupImpl *p);
MPP_RET mpp_buffer_group_reset(MppBufferGroupImpl *p);
void mpp_buffer_group_dump(MppBufferGroupImpl *p);
MppBufferGroupImpl *mpp_buffer_legacy_group();

#ifdef __cplusplus
}
#endif

#endif /*__MPP_BUFFER_IMPL_H__*/
