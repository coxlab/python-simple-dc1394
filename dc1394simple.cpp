#include "dc1394simple.h"

//#include <iostream.h>

PyObject * enumerate_cameras(){
    PyObject *guidList = PyList_New(0);
    //std::vector<uint64_t> guids;
    
    dc1394error_t err;
    dc1394camera_list_t * list;
    
    dc1394_t * d_;
    
    d_ = dc1394_new ();
    err= dc1394_camera_enumerate (d_, &list);
    //printf("error: %d", err);
    //DC1394_ERR_RTN(err,"Failed to enumerate cameras");
    
    if (list->num == 0) {
        dc1394_log_error("No cameras found");
        //return guids;
        return guidList;
    }
    
    for(int i = 0; i < list->num; i++){
        PyList_Append(guidList, Py_BuildValue("l", list->ids[i].guid));
        //guids.push_back(list->ids[i].guid);
    }
    
    dc1394_camera_free_list (list);
    
    
    //return guids;
    return guidList;
}
