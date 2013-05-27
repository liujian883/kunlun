#ifndef KL_STORAGE_PROTOCOL_H_
#define KL_STORAGE_PROTOCOL_H_
#include "common_types.h"
#include "common_protocol.h"
/*
 * @brief: storagenode-storagenode protocol or storagenode-client protocol
 */
#define KL_STORAGE_CMD_FILE_CHECK	1 //check whether files in the vnode need to sync
#define KL_STORAGE_CMD_FILE_SYNC	2 //sync files in the vnode
#define KL_STORAGE_CMD_CREATE_ACCOUNT	3
#define KL_STORAGE_CMD_CREATE_CONTAINER	4
#define KL_STORAGE_CMD_LIST_CONTAINER	5
#define KL_STORAGE_CMD_DELTE_CONTAINER	6
#define KL_STORAGE_CMD_UPLOAD_FILE	7
#define KL_STORAGE_CMD_DOWNLOAD_FILE	8
#define KL_STORAGE_CMD_UPDATE_FILE	9
#define KL_STORAGE_CMD_REMOVE_FILE	10
#define KL_STORAGE_CMD_DELETE_FILE	11
#define KL_STORAGE_CMD_LIST_FILE	12
#define KL_STORAGE_CMD_SERVER_RESP	13
/*
 * @brief: define communicate msg pkg
 */
typedef struct _sync_vnode_info
{
	byte vnode_id[4];
	byte vnode_version[8];
}sync_vnode_info, *psync_vnode_info;

typedef struct _check_file_info
{
	byte file_path[KL_COMMON_PATH_LEN];
	byte hash_code[KL_COMMON_MD5_HASH_LEN];
}check_file_info, *pcheck_file_info;

typedef struct _check_file_resp
{
	byte file_path[KL_COMMON_PATH_LEN]; //file need to sync
}check_file_resp, *pcheck_file_resp;

typedef struct _sync_file_info
{
	byte file_path[KL_COMMON_PATH_LEN];
	byte file_length[8];
}sync_file_info, *psync_file_info;

#endif //KL_STORAGE_PROTOCOL_H_