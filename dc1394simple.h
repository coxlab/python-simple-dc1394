/*
 * Get one gray image using libdc1394 and store it as portable gray map
 *    (pgm). Based on 'samplegrab' from Chris Urmson
 *
 * Written by Damien Douxchamps <ddouxchamps@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdint.h>
#include <dc1394/dc1394.h>
#include <python.h>
#include <vector>
//#include <stdlib.h>
//#include <inttypes.h>

std::vector<uint64_t> enumerate_cameras();


class SimpleCamera {
  
protected:
     
    bool dummy_flag;
    dc1394camera_t *camera;
    int i;
    dc1394featureset_t features;
    dc1394framerates_t framerates;
    dc1394video_modes_t video_modes;
    dc1394framerate_t framerate;
    dc1394video_mode_t video_mode;
    dc1394color_coding_t coding;
    unsigned int width, height;
    dc1394video_frame_t *frame;
    dc1394_t * d;
    
public:
  
    // Dummy constructor, for testing
    SimpleCamera(){
        dummy_flag = 1;
        camera = NULL;
    }
        
    SimpleCamera(uint64_t guid){
        
        dummy_flag = 0;
        dc1394error_t err;
        
        d = dc1394_new ();
        
        camera = dc1394_camera_new (d, guid);
        err=dc1394_video_get_supported_modes(camera,&video_modes);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Can't get video modes");

        // select highest res mode:
        for (i=video_modes.num-1;i>=0;i--) {
          if (!dc1394_is_video_mode_scalable(video_modes.modes[i])) {
              dc1394_get_color_coding_from_video_mode(camera,video_modes.modes[i], &coding);
              if (coding==DC1394_COLOR_CODING_MONO8) {
                  video_mode=video_modes.modes[i];
                  break;
              }
          }
        }
        if (i < 0) {
          dc1394_log_error("Could not get a valid MONO8 mode");
          cleanup_and_exit(camera);
        }

        err=dc1394_get_color_coding_from_video_mode(camera, video_mode,&coding);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not get color coding");

        // get highest framerate
        err=dc1394_video_get_supported_framerates(camera,video_mode,&framerates);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not get framrates");
        framerate=framerates.framerates[framerates.num-1];
        
        err=dc1394_video_set_iso_speed(camera, DC1394_ISO_SPEED_400);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set iso speed");

        err=dc1394_video_set_mode(camera, video_mode);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set video mode");

        err=dc1394_video_set_framerate(camera, framerate);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set framerate");

        err=dc1394_capture_setup(camera,4, DC1394_CAPTURE_FLAGS_DEFAULT);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera");

        /*-----------------------------------------------------------------------
         *  report camera's features
         *-----------------------------------------------------------------------*/
        err=dc1394_feature_get_all(camera,&features);
        if (err!=DC1394_SUCCESS) {
            dc1394_log_warning("Could not get feature set");
        } else {
            dc1394_feature_print_all(&features, stdout);
        }

    }  
    
    
    ~SimpleCamera(){
        
        if(dummy_flag) return;
        /*-----------------------------------------------------------------------
         *  close camera
         *-----------------------------------------------------------------------*/
        dc1394_video_set_transmission(camera, DC1394_OFF);
        dc1394_capture_stop(camera);
        dc1394_camera_free(camera);
        dc1394_free (d);
    }
    
    dc1394video_frame_t *capture_raw_frame(){
        
        dc1394error_t err;
        err=dc1394_video_set_transmission(camera, DC1394_ON);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not start camera iso transmission");

        /*-----------------------------------------------------------------------
         *  capture one frame
         *-----------------------------------------------------------------------*/
        err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not capture a frame");

        /*-----------------------------------------------------------------------
         *  stop data transmission
         *-----------------------------------------------------------------------*/
        err=dc1394_video_set_transmission(camera,DC1394_OFF);
        //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not stop the camera");
        
        return frame;
    }
    

    void cleanup_and_exit(dc1394camera_t *camera)
    {
        dc1394_video_set_transmission(camera, DC1394_OFF);
        dc1394_capture_stop(camera);
        dc1394_camera_free(camera);
        camera = NULL;
    }
    
    
    dc1394video_frame_t *capture_fake_frame(){
        dc1394video_frame_t *frame = new dc1394video_frame_t();
        
        #define FAKE_SIZE   100
        frame->size[0] = FAKE_SIZE;
        frame->size[1] = FAKE_SIZE;
        frame->image = (unsigned char *)calloc(FAKE_SIZE*FAKE_SIZE, sizeof(unsigned char));
        return frame;
    }
        
};
