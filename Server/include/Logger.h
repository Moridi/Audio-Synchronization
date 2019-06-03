#ifndef LOGGER_H_
#define LOGGER_H_

#include <gst/gst.h>

void print_source_stats(GObject * source);
gboolean print_stats(GstElement * rtpbin);

#endif