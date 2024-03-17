#include <cstdio>
#include <gst/gst.h>

int main(int argc, char *argv[]) {

    gst_init(&argc, &argv);

    GstElement *pipeline = gst_pipeline_new("image-display-pipeline");
    GstElement *source = gst_element_factory_make("v4l2src", "source");
    GstElement *decoder = gst_element_factory_make("jpegdec", "decoder"); // Using jpegdec for MJPG format
    GstElement *sink = gst_element_factory_make("xvimagesink", "sink");

    if (!pipeline || !source || !decoder || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    g_object_set(G_OBJECT(source), "device", "/dev/video0", NULL);
    g_object_set(G_OBJECT(source), "do-timestamp", TRUE, NULL); // Added for timestamps

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
    gst_element_link_many(source, decoder, sink, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR);
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
        g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_error_free(err);
        g_free(debug_info);
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
