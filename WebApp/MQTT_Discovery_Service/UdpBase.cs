
using System.Net.Sockets;
using System.Text;

public interface IUdp {
  Task<UdpMessage> ReceiveAsync();
  void Close();
}

public abstract class UdpBase : IUdp {
  protected UdpClient _client = new UdpClient();
  public async Task<UdpMessage> ReceiveAsync() {
    var result = await _client.ReceiveAsync();
    return new UdpMessage {
      Sender = result.RemoteEndPoint,
      Payload = Encoding.ASCII.GetString(result.Buffer, 0, result.Buffer.Length)
    };
  }
  public void Close() {
    _client.Close();
  }
}
