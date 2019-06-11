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
// #define DEST_HOST "127.0.0.1"

char FIRST_CLIENT_IP[] = "172.30.48.122";
char SECOND_CLINET_IP[] = "172.30.49.65";

char* DEST_HOST[] = {FIRST_CLIENT_IP, SECOND_CLINET_IP};


/* #define AUDIO_SRC  "alsasrc" */
// #define AUDIO_SRC  "jackaudiosrc"
#define AUDIO_SRC  "audiotestsrc"

/* the encoder and payloader elements */
#define AUDIO_ENC  "opusenc"
#define AUDIO_PAY  "rtpopuspay"

#define RTP_PORT_IDX 0
#define RTCP_SEND_PORT_IDX 1
#define RTCP_RCV_PORT_IDX 2

#define NUMBER_OF_CLIENTS 2

int FIRST_CLIENT_PORTS[] = {5002, 5003, 5007};
int SECOND_CLIENT_PORTS[] = {5012, 5013, 5017};
int* CLIENTS_PORTS[] = {FIRST_CLIENT_PORTS, SECOND_CLIENT_PORTS};
#endif