# Audio-Synchronization

When designing and implementing a prototype supporting inter-destination media synchronization – synchronized playback between multiple devices receiving the same stream – there are a lot of aspects that need to be considered, especially when working with unmanaged networks. Not only is a proper streaming protocol essential, but also a way to obtain and maintain the synchronization of the clocks of the devices.

The server producing the stream and the clients receiving it should be using the media framework GStreamer. This framework provides methods for both achieving synchronization as well as resynchronization. As the provided resynchronization methods introduced distortions in the audio, an alternative method was implemented. This method focused on minimizing the distortions, thus maintaining a smooth playback.

After the prototype had been implemented, it was tested to see how well it performed under the influence of packet loss and delay.

Here is some videos of the prototype in operation:
https://www.aparat.com/S3Reuis
