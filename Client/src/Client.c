#include <string.h>
#include <math.h>

#include <gst/gst.h>
#include "Config.h"
#include "Logger.h"

GstElement *rtpbin, *rtpsrc, *rtcpsrc, *rtcpsink;
GstElement *audiodepay, *audiodec, *audiores, *audioconv, *audiosink;
GstElement *pipeline;
GMainLoop *loop;
GstCaps *caps;
gboolean res;
GstPadLinkReturn lres;
GstPad *srcpad, *sinkpad;

void initialize_pipeline(int argc, char *argv[])
{
    /* always init first */
    gst_init(&argc, &argv);

    /* the pipeline to hold everything */
    pipeline = gst_pipeline_new(NULL);
    g_assert(pipeline);

    /* the udp src and source we will use for RTP and RTCP */
    rtpsrc = gst_element_factory_make("udpsrc", "rtpsrc");
    g_assert(rtpsrc);
    g_object_set(rtpsrc, "port", 5002, NULL);
    /* we need to set caps on the udpsrc for the RTP data */
    caps = gst_caps_from_string(AUDIO_CAPS);
    g_object_set(rtpsrc, "caps", caps, NULL);
    gst_caps_unref(caps);

    rtcpsrc = gst_element_factory_make("udpsrc", "rtcpsrc");
    g_assert(rtcpsrc);
    g_object_set(rtcpsrc, "port", 5003, NULL);

    rtcpsink = gst_element_factory_make("udpsink", "rtcpsink");
    g_assert(rtcpsink);
    g_object_set(rtcpsink, "port", 5007, "host", DEST_HOST, NULL);
    /* no need for synchronisation or preroll on the RTCP sink */
    g_object_set(rtcpsink, "async", FALSE, "sync", FALSE, NULL);

    gst_bin_add_many(GST_BIN(pipeline), rtpsrc, rtcpsrc, rtcpsink, NULL);
}

void add_decoder()
{
    /* the depayloading and decoding */
    audiodepay = gst_element_factory_make(AUDIO_DEPAY, "audiodepay");
    g_assert(audiodepay);

    audiodec = gst_element_factory_make(AUDIO_DEC, "audiodec");
    g_assert(audiodec);
    g_object_set(audiodec, "plc", TRUE, NULL);
    
    /* the audio playback and format conversion */
    audioconv = gst_element_factory_make("audioconvert", "audioconv");
    g_assert(audioconv);
    audiores = gst_element_factory_make("audioresample", "audiores");
    g_assert(audiores);

    // @TODO: drift-tolerance, sync, slave-method
    audiosink = gst_element_factory_make(AUDIO_SINK, "audiosink");
    g_assert(audiosink);
    // g_object_set(audiodec, "plc", TRUE, NULL);

    /* add depayloading and playback to the pipeline and link */
    gst_bin_add_many(GST_BIN(pipeline), audiodepay, audiodec, audioconv,
        audiores, audiosink, NULL);

    res = gst_element_link_many(audiodepay, audiodec, audioconv, audiores,
        audiosink, NULL);
    g_assert(res == TRUE);
}

void add_rtpbin()
{
    /* the rtpbin element */
    rtpbin = gst_element_factory_make("rtpbin", "rtpbin");
    g_assert(rtpbin);

    // @TODO: Uncomment it.
    // BUFFER_MODE_NONE = 0
    // g_object_set(rtpbin, "buffer-mode", 0, NULL);
    g_object_set(rtpbin, "ntp-sync", TRUE, NULL);

    gst_bin_add(GST_BIN(pipeline), rtpbin);
}

void link_all_to_rtpbin()
{
    /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
    srcpad = gst_element_get_static_pad(rtpsrc, "src");
    sinkpad = gst_element_get_request_pad(rtpbin, "recv_rtp_sink_0");
    lres = gst_pad_link(srcpad, sinkpad);
    g_assert(lres == GST_PAD_LINK_OK);
    gst_object_unref(srcpad);

}

void get_rtcp_sink_pad()
{
    /* get an RTCP sinkpad in session 0 */
    srcpad = gst_element_get_static_pad(rtcpsrc, "src");
    sinkpad = gst_element_get_request_pad(rtpbin, "recv_rtcp_sink_0");
    lres = gst_pad_link(srcpad, sinkpad);
    g_assert(lres == GST_PAD_LINK_OK);
    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);
}

void get_rtcp_source_pad()
{
    /* get an RTCP srcpad for sending RTCP back to the sender */
    srcpad = gst_element_get_request_pad(rtpbin, "send_rtcp_src_0");
    sinkpad = gst_element_get_static_pad(rtcpsink, "sink");
    lres = gst_pad_link(srcpad, sinkpad);
    g_assert(lres == GST_PAD_LINK_OK);
    gst_object_unref(sinkpad);
}

void connect_signals()
{
    /* the RTP pad that we have to connect to the depayloader will be created
    * dynamically so we connect to the pad-added signal, pass the depayloader as
    * user_data so that we can link to it. */
    g_signal_connect(rtpbin, "pad-added", G_CALLBACK(pad_added_cb), audiodepay);

    /* give some stats when we receive RTCP */
    g_signal_connect(rtpbin, "on-ssrc-active", G_CALLBACK(on_ssrc_active_cb),
        audiodepay);
}

void setup_pipeline()
{
    /* set the pipeline to playing */
    g_print("starting receiver pipeline\n");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* we need to run a GLib main loop to get the messages */
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_print("stopping receiver pipeline\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);

    gst_object_unref(pipeline);
}
