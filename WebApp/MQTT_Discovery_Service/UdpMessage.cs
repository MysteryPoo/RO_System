
using System.Net;

public struct UdpMessage {
  public IPEndPoint Sender { get; set; }
  public string Payload { get; set; }
}
