#include "dc1394simple.h"

std::vector<uint64_t> enumerate_cameras(){
    std::vector<uint64_t> guids;
    
    dc1394error_t err;
    dc1394camera_list_t * list;
    
    dc1394_t * d_;
    
    d_ = dc1394_new ();
    err= dc1394_camera_enumerate (d_, &list);
    //printf("error: %d", err);
    //DC1394_ERR_RTN(err,"Failed to enumerate cameras");
    
    if (list->num == 0) {
        dc1394_log_error("No cameras found");
        return guids;
    }
    
    for(int i = 0; i < list->num; i++){
        guids.push_back(list->ids[0].guid);
    }
    
    dc1394_camera_free_list (list);
    
    
    return guids;
}
