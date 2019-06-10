#define GLIB_DISABLE_DEPRECATION_WARNINGS

#include "Server.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <gst/gst.h>

#include "Config.h"
#include "Logger.h"

GstElement *audiosrc[NUMBER_OF_CLIENTS];
GstElement *audioconv[NUMBER_OF_CLIENTS];
GstElement *audiores[NUMBER_OF_CLIENTS];
GstElement *audioenc[NUMBER_OF_CLIENTS];
GstElement *audiopay[NUMBER_OF_CLIENTS];

GstElement *rtpbin;
GstElement *rtpsink[NUMBER_OF_CLIENTS];
GstElement *rtcpsink[NUMBER_OF_CLIENTS];
GstElement *rtcpsrc[NUMBER_OF_CLIENTS];
GstElement *pipeline;
GMainLoop *loop;
GstPad *srcpad[NUMBER_OF_CLIENTS];
GstPad *sinkpad[NUMBER_OF_CLIENTS];

char* append_integer(char* string, int integer, char** result)
{
  char* integer_string = (char*)malloc(sizeof(char) * 32);
  sprintf(integer_string, "%d", integer);

  (*result) = (char*)malloc(sizeof(char) * 64);
  int i = 0;

  while(string[i])
  {
    (*result)[i] = string[i];
    i++;
  }

  strcat((*result), integer_string);

  return (*result);
}

void setup_client(int argc, char *argv[], int client_id)
{  
  char* message;

  add_audio_elements(client_id);

  add_udp_terminals(CLIENTS_PORTS[client_id][RTP_PORT_IDX],
      CLIENTS_PORTS[client_id][RTCP_SEND_PORT_IDX],
      CLIENTS_PORTS[client_id][RTCP_RCV_PORT_IDX],
      client_id);
    
  link_to_rtpbin(append_integer("send_rtp_sink_", 
      client_id, &message), client_id);
  get_rtp_source_pad(append_integer("send_rtp_src_", 
      client_id, &message), client_id);
  get_rtcp_source_pad(append_integer("send_rtcp_src_", 
      client_id, &message), client_id);
  receive_rtcp(append_integer("recv_rtcp_sink_", 
      client_id, &message), client_id);
}


void add_audio_elements(int client_id)
{
  /* the audio capture and format conversion */
  audiosrc[client_id] = gst_element_factory_make(AUDIO_SRC, "audiosrc");
  g_assert(audiosrc[client_id]);
  audioconv[client_id] = gst_element_factory_make("audioconvert", "audioconv");
  g_assert(audioconv[client_id]);
  audiores[client_id] = gst_element_factory_make("audioresample", "audiores");
  g_assert(audiores[client_id]);
  /* the encoding and payloading */
  audioenc[client_id] = gst_element_factory_make(AUDIO_ENC, "audioenc");
  g_assert(audioenc[client_id]);
  audiopay[client_id] = gst_element_factory_make(AUDIO_PAY, "audiopay");
  g_assert(audiopay[client_id]);

  /* add capture and payloading to the pipeline and link */
  gst_bin_add_many(GST_BIN(pipeline), audiosrc[client_id], audioconv[client_id], audiores[client_id],
      audioenc[client_id], audiopay[client_id], NULL);

  if(!gst_element_link_many(audiosrc[client_id], audioconv[client_id], audiores[client_id], audioenc[client_id],
          audiopay[client_id], NULL)) {
    g_error("Failed to link audiosrc, audioconv, audioresample, "
        "audio encoder and audio payloader");
  }
}

void initialize_pipeline(int argc, char *argv[])
{
  /* always init first */
  gst_init(&argc, &argv);

  /* the pipeline to hold everything */
  pipeline = gst_pipeline_new(NULL);
  g_assert(pipeline);
}

void add_rtpbin()
{
  /* the rtpbin element */
  rtpbin = gst_element_factory_make("rtpbin", "rtpbin");
  g_assert(rtpbin);

  gst_bin_add(GST_BIN(pipeline), rtpbin);
}

void add_udp_terminals(int rtp_sink_port,
    int rtcp_sink_port, int rtcp_src_port, int client_id)
{
  // Periodic ticks waveform
  g_object_set (audiosrc[client_id], "wave", 8, NULL);
  
  // Make the server act as a live source
  g_object_set (audiosrc[client_id], "is-live", TRUE, NULL);

  /* the udp sinks and source we will use for RTP and RTCP */
  rtpsink[client_id] = gst_element_factory_make("udpsink", "rtpsink");
  g_assert(rtpsink[client_id]);
  g_object_set(rtpsink[client_id], "port", rtp_sink_port, "host", DEST_HOST, NULL);

  rtcpsink[client_id] = gst_element_factory_make("udpsink", "rtcpsink");
  g_assert(rtcpsink[client_id]);
  g_object_set(rtcpsink[client_id], "port", rtcp_sink_port, "host", DEST_HOST, NULL);
  /* no need for synchronisation or preroll on the RTCP sink */
  g_object_set(rtcpsink[client_id], "async", FALSE, "sync", FALSE, NULL);

  rtcpsrc[client_id] = gst_element_factory_make("udpsrc", "rtcpsrc");
  g_assert(rtcpsrc[client_id]);
  g_object_set(rtcpsrc[client_id], "port", rtcp_src_port, NULL);

  gst_bin_add_many(GST_BIN(pipeline), rtpsink[client_id],
      rtcpsink[client_id], rtcpsrc[client_id], NULL);
}

void link_to_rtpbin(const char* request_pad, int client_id)
{
  /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
  sinkpad[client_id] = gst_element_get_request_pad(rtpbin, request_pad);
  srcpad[client_id] = gst_element_get_static_pad(audiopay[client_id], "src");
  if(gst_pad_link(srcpad[client_id], sinkpad[client_id]) != GST_PAD_LINK_OK)
    g_error("Failed to link audio payloader to rtpbin");
  gst_object_unref(srcpad[client_id]);
}

void get_rtp_source_pad(const char* static_pad, int client_id)
{
  /* get the RTP srcpad that was created when we requested the sinkpad above and
   * link it to the rtpsink sinkpad*/
  srcpad[client_id] = gst_element_get_static_pad(rtpbin, "send_rtp_src_0");
  sinkpad[client_id] = gst_element_get_static_pad(rtpsink[client_id], "sink");
  if(gst_pad_link(srcpad[client_id], sinkpad[client_id]) != GST_PAD_LINK_OK)
    g_error("Failed to link rtpbin to rtpsink");
  gst_object_unref(srcpad[client_id]);
  gst_object_unref(sinkpad[client_id]);
}

void get_rtcp_source_pad(const char* request_pad, int client_id)
{
  /* get an RTCP srcpad for sending RTCP to the receiver */
  srcpad[client_id] = gst_element_get_request_pad(rtpbin, request_pad);
  sinkpad[client_id] = gst_element_get_static_pad(rtcpsink[client_id], "sink");
  if(gst_pad_link(srcpad[client_id], sinkpad[client_id]) != GST_PAD_LINK_OK)
    g_error("Failed to link rtpbin to rtcpsink");
  gst_object_unref(sinkpad[client_id]);
}

void receive_rtcp(const char* request_pad, int client_id)
{
  /* we also want to receive RTCP, request an RTCP sinkpad for session 0 and
   * link it to the srcpad of the udpsrc for RTCP */
  srcpad[client_id] = gst_element_get_static_pad(rtcpsrc[client_id], "src");
  sinkpad[client_id] = gst_element_get_request_pad(rtpbin, request_pad);
  if(gst_pad_link(srcpad[client_id], sinkpad[client_id]) != GST_PAD_LINK_OK)
    g_error("Failed to link rtcpsrc to rtpbin");
  gst_object_unref(srcpad[client_id]);
}

void setup_pipeline()
{
  /* set the pipeline to playing */
  g_print("starting sender pipeline\n");
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  /* print stats every second */
  g_timeout_add_seconds(1,(GSourceFunc) print_stats, rtpbin);

  /* we need to run a GLib main loop to get the messages */
  loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);

  g_print("stopping sender pipeline\n");
  gst_element_set_state(pipeline, GST_STATE_NULL);
}