#ifndef CONFIG_H_
#define CONFIG_H_

/*
 * A RTP receiver 
 *
 *  receives opus encoded RTP audio on port 5002, RTCP is received on  port 5003.
 *  the receiver RTCP reports are sent to port 5007
 *
 *             .-------.      .-------------.     .---------.   .-------.   .--------.
 *  RTP        |udpsrc |      | rtpbin      |     |opusdepay|   |opusdec|   |alsasink|
 *  port=5002  |      src->recv_rtp recv_rtp->sink     src->sink   src->sink         |
 *             '-------'      |             |     '---------'   '-------'   '--------'
 *                            |             |      
 *                            |             |       .-------.
 *                            |             |       |udpsink|   RTCP
 *                            |         send_rtcp->sink     |   port=5007
 *             .-------.      |             |       '-------'   sync=false
 *  RTCP       |udpsrc |      |             |                   async=false
 *  port=5003  |     src->recv_rtcp         |                       
 *             '-------'      '-------------'              
 */

/* the caps of the sender RTP stream. This is usually negotiated out of band with
 * SDP or RTSP. */

#define AUDIO_CAPS "application/x-rtp,media=(string)audio,clock-rate=(int)48000,encoding-name=(string)OPUS"

#define AUDIO_DEPAY "rtpopusdepay"
#define AUDIO_DEC   "opusdec"
#define AUDIO_SINK  "autoaudiosink"

/* the destination machine to send RTCP to. This is the address of the sender and
 * is used to send back the RTCP reports of this receiver. If the data is sent
 * from another machine, change this address. */
#define DEST_HOST "127.0.0.1"

#define RTP_PORT_IDX 0
#define RTCP_SEND_PORT_IDX 1
#define RTCP_RCV_PORT_IDX 2

int FIRST_CLIENT_PORTS[] = {5002, 5003, 5007};
int SECOND_CLIENT_PORTS[] = {5012, 5013, 5017};
int* CLIENTS_PORTS[] = {FIRST_CLIENT_PORTS, SECOND_CLIENT_PORTS};

#endif