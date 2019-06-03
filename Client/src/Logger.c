#include <string.h>
#include <math.h>

#include <gst/gst.h>

/* print the stats of a source */
void print_source_stats(GObject * source)
{
  GstStructure *stats;
  gchar *str;

  g_return_if_fail(source != NULL);

  /* get the source stats */
  g_object_get(source, "stats", &stats, NULL);

  /* simply dump the stats structure */
  str = gst_structure_to_string(stats);
  g_print("source stats: %s\n", str);

  gst_structure_free(stats);
  g_free(str);
}

/* will be called when rtpbin signals on-ssrc-active. It means that an RTCP
 * packet was received from another source. */
void on_ssrc_active_cb(GstElement * rtpbin, guint sessid, guint ssrc,
    GstElement * depay)
{
  GObject *session, *osrc;

  g_print("got RTCP from session %u, SSRC %u\n", sessid, ssrc);

  /* get the right session */
  g_signal_emit_by_name(rtpbin, "get-internal-session", sessid, &session);

#if 0
  /* FIXME: This is broken in rtpbin */
  /* get the internal source(the SSRC allocated to us, the receiver */
  g_object_get(session, "internal-source", &isrc, NULL);
  print_source_stats(isrc);
  g_object_unref(isrc);
#endif

  /* get the remote source that sent us RTCP */
  g_signal_emit_by_name(session, "get-source-by-ssrc", ssrc, &osrc);
  print_source_stats(osrc);
  g_object_unref(osrc);
  g_object_unref(session);
}

/* will be called when rtpbin has validated a payload that we can depayload */
void pad_added_cb(GstElement * rtpbin, GstPad * new_pad, GstElement * depay)
{
  GstPad *sinkpad;
  GstPadLinkReturn lres;

  g_print("new payload on pad: %s\n", GST_PAD_NAME(new_pad));

  sinkpad = gst_element_get_static_pad(depay, "sink");
  g_assert(sinkpad);

  lres = gst_pad_link(new_pad, sinkpad);
  g_assert(lres == GST_PAD_LINK_OK);
  gst_object_unref(sinkpad);
}