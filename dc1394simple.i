%module dc1394simple

%{
	#include "dc1394simple.h"
	#include "arrayobject.h"
%}

%include "typemaps.i"

%{
#define SWIG_FILE_WITH_INIT
#define PY_ARRAY_UNIQUE_SYMBOL PyArray_API
%}


%include "std_vector.i"
namespace std {
   %template(vector_uint64) vector<uint64_t>;
};


%include <dc1394/dc1394.h>
%include <dc1394/video.h>
%include "dc1394simple.h"


%extend SimpleCamera {
    
    %pythoncode{
        def capture_frame(self):
            raw = self.capture_raw_frame()
            
            if raw is None:
                return None
            
            return raw.array
        
    }
}

%extend dc1394video_frame_t {
    
    int get_width(){
        return self->size[0];
    }
    
    int get_height(){
        return self->size[1];
    }
    
    PyObject * to_1D_list(){
        int width = self->size[0];
        int height = self->size[1];
        
        PyObject *list = PyList_New(width*height);
        
        for(int i = 0; i < width*height; i++){
            PyList_SetItem(list, i, PyInt_FromLong((long)self->image[i]));
        } 
        
        return list;
    }
    
    %pythoncode{
        height = property(get_height)
        width = property(get_width)

        def to_array(self):
            import numpy
            one_d_list = self.to_1D_list()
            arr = numpy.array(one_d_list)            
            arr.shape = (self.get_height(), self.get_width())
            return arr
        
        array = property(to_array)
    }
}


    
//    
//    
//    
//	PyObject* __get_array_struct(){
//        PyArrayInterface *inter;
//        
//		inter = (PyArrayInterface *)malloc(sizeof(PyArrayInterface));
//        
//		inter->two = 2;
//		inter->nd = 2;
//		inter->typekind = 'u';
//		inter->itemsize = sizeof(unsigned char);
//		inter->flags = (NPY_CONTIGUOUS | NPY_OWNDATA | NPY_ALIGNED);// |  NPY_NOTSWAPPED);
//        
//		Py_intptr_t *shape = (Py_intptr_t *)malloc(2*sizeof(Py_intptr_t));
//		shape[1] = self->size[1];
//		shape[0] = self->size[0];
//		inter->shape = shape;
//        
//		inter->strides = NULL;
//		
//		inter->data = (void *)(self->image);
//		inter->descr = 0;
//        
//		return PyCObject_FromVoidPtr(inter,0);
//	}
//    
//	
//	%pythoncode{
//#__array_struct__ = property(__get_array_struct, doc="Array Protocol")
//#array_struct = property(__get_array_struct, doc="Array Protocol")
//        list = property(__to_list, doc="Convert to a python list")
//	}
//}

