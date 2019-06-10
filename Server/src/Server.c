#define GLIB_DISABLE_DEPRECATION_WARNINGS

#include "Server.h"

#include <string.h>
#include <math.h>

#include <gst/gst.h>
#include "Config.h"
#include "Logger.h"

GstElement *audiosrc, *audioconv, *audiores, *audioenc, *audiopay;
GstElement *rtpbin, *rtpsink, *rtcpsink, *rtcpsrc;
GstElement *pipeline;
GMainLoop *loop;
GstPad *srcpad, *sinkpad;

void setup_client(int argc, char *argv[], int client_id)
{
  initialize_pipeline(argc, argv);
  add_rtpbin();
  add_udp_terminals(CLIENTS_PORTS[client_id][RTP_PORT_IDX],
      CLIENTS_PORTS[client_id][RTCP_SEND_PORT_IDX],
      CLIENTS_PORTS[client_id][RTCP_RCV_PORT_IDX]);
  link_to_rtpbin();
  get_rtp_source_pad();
  get_rtcp_source_pad();
  receive_rtcp();
  setup_pipeline();
}

void initialize_pipeline(int argc, char *argv[])
{
  /* always init first */
  gst_init(&argc, &argv);

  /* the pipeline to hold everything */
  pipeline = gst_pipeline_new(NULL);
  g_assert(pipeline);

  /* the audio capture and format conversion */
  audiosrc = gst_element_factory_make(AUDIO_SRC, "audiosrc");
  g_assert(audiosrc);
  audioconv = gst_element_factory_make("audioconvert", "audioconv");
  g_assert(audioconv);
  audiores = gst_element_factory_make("audioresample", "audiores");
  g_assert(audiores);
  /* the encoding and payloading */
  audioenc = gst_element_factory_make(AUDIO_ENC, "audioenc");
  g_assert(audioenc);
  audiopay = gst_element_factory_make(AUDIO_PAY, "audiopay");
  g_assert(audiopay);

  /* add capture and payloading to the pipeline and link */
  gst_bin_add_many(GST_BIN(pipeline), audiosrc, audioconv, audiores,
      audioenc, audiopay, NULL);

  if(!gst_element_link_many(audiosrc, audioconv, audiores, audioenc,
          audiopay, NULL)) {
    g_error("Failed to link audiosrc, audioconv, audioresample, "
        "audio encoder and audio payloader");
  }
}

void add_rtpbin()
{
  /* the rtpbin element */
  rtpbin = gst_element_factory_make("rtpbin", "rtpbin");
  g_assert(rtpbin);

  gst_bin_add(GST_BIN(pipeline), rtpbin);
}

void add_udp_terminals(int rtp_sink_port,
    int rtcp_sink_port, int rtcp_src_port)
{
  // Periodic ticks waveform
  g_object_set (audiosrc, "wave", 8, NULL);
  
  // Make the server act as a live source
  g_object_set (audiosrc, "is-live", TRUE, NULL);

  /* the udp sinks and source we will use for RTP and RTCP */
  rtpsink = gst_element_factory_make("udpsink", "rtpsink");
  g_assert(rtpsink);
  g_object_set(rtpsink, "port", 5002, "host", DEST_HOST, NULL);

  rtcpsink = gst_element_factory_make("udpsink", "rtcpsink");
  g_assert(rtcpsink);
  g_object_set(rtcpsink, "port", 5003, "host", DEST_HOST, NULL);
  /* no need for synchronisation or preroll on the RTCP sink */
  g_object_set(rtcpsink, "async", FALSE, "sync", FALSE, NULL);

  rtcpsrc = gst_element_factory_make("udpsrc", "rtcpsrc");
  g_assert(rtcpsrc);
  g_object_set(rtcpsrc, "port", 5007, NULL);

  gst_bin_add_many(GST_BIN(pipeline), rtpsink, rtcpsink, rtcpsrc, NULL);
}

void link_to_rtpbin()
{
  /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
  sinkpad = gst_element_get_request_pad(rtpbin, "send_rtp_sink_0");
  srcpad = gst_element_get_static_pad(audiopay, "src");
  if(gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK)
    g_error("Failed to link audio payloader to rtpbin");
  gst_object_unref(srcpad);
}

void get_rtp_source_pad()
{
  /* get the RTP srcpad that was created when we requested the sinkpad above and
   * link it to the rtpsink sinkpad*/
  srcpad = gst_element_get_static_pad(rtpbin, "send_rtp_src_0");
  sinkpad = gst_element_get_static_pad(rtpsink, "sink");
  if(gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK)
    g_error("Failed to link rtpbin to rtpsink");
  gst_object_unref(srcpad);
  gst_object_unref(sinkpad);
}

void get_rtcp_source_pad()
{
  /* get an RTCP srcpad for sending RTCP to the receiver */
  srcpad = gst_element_get_request_pad(rtpbin, "send_rtcp_src_0");
  sinkpad = gst_element_get_static_pad(rtcpsink, "sink");
  if(gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK)
    g_error("Failed to link rtpbin to rtcpsink");
  gst_object_unref(sinkpad);
}

void receive_rtcp()
{
  /* we also want to receive RTCP, request an RTCP sinkpad for session 0 and
   * link it to the srcpad of the udpsrc for RTCP */
  srcpad = gst_element_get_static_pad(rtcpsrc, "src");
  sinkpad = gst_element_get_request_pad(rtpbin, "recv_rtcp_sink_0");
  if(gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK)
    g_error("Failed to link rtcpsrc to rtpbin");
  gst_object_unref(srcpad);
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