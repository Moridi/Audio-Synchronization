#define GLIB_DISABLE_DEPRECATION_WARNINGS

#include <string.h>
#include <math.h>

#include <gst/gst.h>

/* print the stats of a source */
void print_source_stats(GObject * source)
{
  GstStructure *stats;
  gchar *str;

  /* get the source stats */
  g_object_get(source, "stats", &stats, NULL);

  /* simply dump the stats structure */
  str = gst_structure_to_string(stats);
  g_print("source stats: %s\n", str);

  gst_structure_free(stats);
  g_free(str);
}

/* this function is called every second and dumps the RTP manager stats */
gboolean print_stats(GstElement * rtpbin)
{
  GObject *session;
  GValueArray *arr;
  GValue *val;
  guint i;

  g_print("***********************************\n");

  /* get session 0 */
  g_signal_emit_by_name(rtpbin, "get-internal-session", 0, &session);

  /* print all the sources in the session, this includes the internal source */
  g_object_get(session, "sources", &arr, NULL);

  for(i = 0; i < arr->n_values; i++) {
    GObject *source;

    val = g_value_array_get_nth(arr, i);
    source = g_value_get_object(val);

    print_source_stats(source);
  }
  g_value_array_free(arr);

  g_object_unref(session);

  return TRUE;
}
