#ifndef CONFIG_H_
#define CONFIG_H_

/*
 * A RTP server 
 *  sends the output of alsasrc as opus encoded RTP on port 5002, RTCP is sent on
 *  port 5003. The destination is 127.0.0.1.
 *  the receiver RTCP reports are received on port 5007
 *
 * .------------.   .-------.   .-------.      .-----------------.     .-------.
 * |audiotestsrc|   |opusenc|   |opuspay|      | rtpbin          |     |udpsink|  RTP
 * |         src->sink   src->sink    src->send_rtp   send_rtp->sink   |    port=5002
 * '------------'   '-------'   '-------'      |                 |     '-------'
 *                                             |                 |      
 *                                             |                 |     .-------.
 *                                             |                 |     |udpsink|  RTCP
 *                                             |        send_rtcp->sink        | port=5003
 *                              .-------.      |                 |     '-------' sync=false
 *                      RTCP    |udpsrc |      |                 |               async=false
 *                  port=5007   |    src->recv_rtcp              |                       
 *                              '-------'      '-----------------'              
 */

/* change this to send the RTP data and RTCP to another host */
#define DEST_HOST "127.0.0.1"

/* #define AUDIO_SRC  "alsasrc" */
// #define AUDIO_SRC  "jackaudiosrc"
#define AUDIO_SRC  "audiotestsrc"

/* the encoder and payloader elements */
#define AUDIO_ENC  "opusenc"
#define AUDIO_PAY  "rtpopuspay"

#endif