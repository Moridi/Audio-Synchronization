#ifndef LOGGER_H_
#define LOGGER_H_

#include <gst/gst.h>

void print_source_stats(GObject * source);
void on_ssrc_active_cb(GstElement * rtpbin, guint sessid, guint ssrc,
    GstElement * depay);
void pad_added_cb(GstElement * rtpbin, GstPad * new_pad, GstElement * depay);

#endif