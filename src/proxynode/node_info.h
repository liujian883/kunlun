#ifndef KL_PROXY_NODE_INFO_H_
#define KL_PROXY_NODE_INFO_H_
/*
 * @description: in the kunlun destributed file system, the real storage node
                 is named with device, and the virtual storage node is named
				 with vnode. what's more, kunlun also use zone to limit
				 different replicas of a file being save to the same zone
 */
#include <stdint.h>
#include <sys/types.h>
#include "common_types.h"

#define KL_PROXY_CONTAINER_SIZE 100 /* define device node container size */
typedef struct _device_info
{
	int device_id; //sign the device in the storage node cluster(online)
	int64_t key_hash; //key_hash divide the name space of kunlun to equivalent parts
	int zone_id;
	int vnode_count;
	int weight;
	char bind_ip[KL_COMMON_IP_ADDR_LEN];
	int bind_port;
	time_t last_beat_time;
}device_info, *device_info_ptr;

class CDeviceContainer
{
public:
	CDeviceContainer();
	~CDeviceContainer();
	
	/*
	 * @param: pdevice_info, must be created on heap(use operator new)
	           and deleted by CDeviceContainer
	 */
	int add_node(device_info_ptr pdevice_info);
	/*
	 * @param: pdevice_info, CDeviceContainer isn't sure to release the
	           memory hold by pdevice_info
	 * @description: the function merge the node info to node container, 
	                 but it can't create a node in node container, if the
					 merged node isn't in container, the function will return
					 -1, if merge node successfully, the function will return 0
	 */
	int merge_node(device_info_ptr pdevice_info);
	/*
	 * @param: pdevice_info, the function will use the ip info and port info in
	           pdevice_info to make sure the node that will be deleted in node 
			   container, CDeviceContainer isn't sure to release the memory holding
			   by pdevice_info
	 */
	int delete_node(device_info_ptr pdevice_info);
	/*
	 * @description: the function will return the node info in the head node container,
	                 the function will sort the node container with qsort to make sure
					 return a node that it's volume is the least
	 */
	device_info_ptr get_node();
private:
	/*
	 * @description: compare the two nodes by their volume, if ptr1's volume
	                 is larger than ptr2, the function will return 1, if ptr1's
					 volume equal to ptr2, the function will return 0, if ptr1's
					 volume is smaller than ptr2, the function will return -1
	 */
	int node_vol_cmp(device_info_ptr ptr1, device_info_ptr ptr2);
	/*
	 * @description: compare the two nodes by their ip address and port address,
	                 if they have the same ip and port, the function will return
					 0, otherwise, the function will return -1
	 */
	int node_diff_cmp(device_info_ptr ptr1, device_info_ptr ptr2);

	device_info_ptr *m_pnode_container;
	int m_node_count;
	int m_container_size;
};
#endif //KL_PROXY_NODE_INFO_H_