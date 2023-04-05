interface IStatus {
  [JsonPropertyName("online")]
  bool isOnline;
  [JsonPropertyName("last_heard")]
  DateTimeOffset lastHeard;
}