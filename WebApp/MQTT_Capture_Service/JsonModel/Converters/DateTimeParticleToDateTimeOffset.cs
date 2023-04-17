
using System.Text.Json;
using System.Text.Json.Serialization;

public class DateTimeParticleToDateTimeOffset : JsonConverter<DateTimeOffset> {
  public override bool CanConvert(Type typeToConvert) =>
    typeof(DateTimeOffset).IsAssignableFrom(typeToConvert);

  public override DateTimeOffset Read(ref Utf8JsonReader reader, Type typeToConvert, JsonSerializerOptions options) {
    uint unixtime = reader.GetUInt32();
    DateTimeOffset datetime = unixtime == 0 ? DateTimeOffset.Now : new DateTimeOffset(1970, 1, 1, 0, 0, 0, 0, 0, TimeSpan.Zero);
    return datetime.AddSeconds(unixtime);
  }

  public override void Write(Utf8JsonWriter writer, DateTimeOffset value, JsonSerializerOptions options)
  {
    throw new NotImplementedException();
  }
}
