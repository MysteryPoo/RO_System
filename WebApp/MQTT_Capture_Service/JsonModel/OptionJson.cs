
using System.Text.Json;
using System.Text.Json.Serialization;

public class OptionJson {
  
  [JsonPropertyName("name")]
  public string Name { get; set; } = string.Empty;
  [JsonPropertyName("type")]
  public string Type { get; set; } = string.Empty;
}

public class BooleanOption : OptionJson {
  [JsonPropertyName("default")]
  public bool Default { get; set; }
}

public class NumberOption : OptionJson {
  [JsonPropertyName("units")]
  public string Units { get; set; } = string.Empty;
  [JsonPropertyName("default")]
  public int Default { get; set; }
}

public class OptionConverterWithTypeDiscriminator : JsonConverter<OptionJson> {
  public override bool CanConvert(Type typeToConvert) =>
    typeof(OptionJson).IsAssignableFrom(typeToConvert);

  public override OptionJson Read(ref Utf8JsonReader reader, Type typeToConvert, JsonSerializerOptions options) {
    if (reader.TokenType != JsonTokenType.StartObject) {
      throw new JsonException();
    }

    reader.Read();
    if (reader.TokenType != JsonTokenType.PropertyName) {
      throw new JsonException();
    }
    string? propertyName = reader.GetString();
    if (propertyName != "name") {
      throw new JsonException();
    }
    reader.Read();
    if (reader.TokenType != JsonTokenType.String) {
      throw new JsonException();
    }
    string optionName = reader.GetString() ?? string.Empty;

    reader.Read();
    if (reader.TokenType != JsonTokenType.PropertyName) {
      throw new JsonException();
    }
    propertyName = reader.GetString();
    if (propertyName != "type") {
      throw new JsonException();
    }

    reader.Read();
    if (reader.TokenType != JsonTokenType.String) {
      throw new JsonException();
    }

    string typeDiscriminator = reader.GetString() ?? "unknown";
    OptionJson option = typeDiscriminator switch {
      "number" => new NumberOption(),
      "boolean" => new BooleanOption(),
      "trigger" => new OptionJson(),
      _ => throw new JsonException()
    };
    option.Name = optionName;
    option.Type = typeDiscriminator;

    while (reader.Read()) {
      if (reader.TokenType == JsonTokenType.EndObject) {
        return option;
      }

      if (reader.TokenType == JsonTokenType.PropertyName) {
        propertyName = reader.GetString();
        reader.Read();
        switch (propertyName) {
          case "name":
            option.Name = reader.GetString() ?? string.Empty;
            break;
          case "units":
            string? units = reader.GetString();
            ((NumberOption)option).Units = units ?? string.Empty;
            break;
          case "default":
            if (typeDiscriminator.Equals("number")) {
              ((NumberOption)option).Default = reader.GetInt32();
            }
            if (typeDiscriminator.Equals("boolean")) {
              ((BooleanOption)option).Default = reader.GetBoolean();
            }
            break;
        }
      }
    }

    throw new JsonException();
  }

  public override void Write(Utf8JsonWriter writer, OptionJson value, JsonSerializerOptions options)
  {
    throw new NotImplementedException();
  }
}
