
using System.Net;
using System.Net.Sockets;
using System.Text;

public class UdpServer : UdpBase {
  public int Port {
    get => ((IPEndPoint)(_client.Client.LocalEndPoint ?? throw new Exception("No port"))).Port;
  }
  public IPAddress Address {
    get => ((IPEndPoint)(_client.Client.LocalEndPoint ?? throw new Exception("No address"))).Address;
  }
  public string Family {
    get {
      switch (_client.Client.AddressFamily) {
        case AddressFamily.InterNetwork:
          return "IPv4";
        case AddressFamily.InterNetworkV6:
          return "IPv6";
        default:
          return _client.Client.AddressFamily.ToString();
      }
    }
  }
  public UdpServer() : this(new IPEndPoint(IPAddress.Any, 1882)) {}
  public UdpServer(IPEndPoint endPoint) {
    const string multicastIP = "224.0.0.116";
    _client = new UdpClient(endPoint);
    try {
      MulticastOption option = new MulticastOption(IPAddress.Parse(multicastIP));
      _client.Client.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership, option);
      Console.WriteLine($"Joining multicast group: {multicastIP}");
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
  public int Reply(string payload, IPEndPoint endPoint) {
    var datagram = Encoding.ASCII.GetBytes(payload);
    return _client.Send(datagram, datagram.Length, endPoint);
  }
}
