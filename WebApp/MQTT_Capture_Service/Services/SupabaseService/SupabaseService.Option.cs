
using Capture.DbRow;

public partial class SupabaseService {

  public async Task<IEnumerable<OptionDbRow>> GetOptionListForComponent(long componentId) {
    return (await _client.From<OptionDbRow>().Where(o => o.ComponentId == componentId).Get()).Models;
  }

  public async Task<OptionNumberDbRow?> GetNumberOptionFromOption(long optionId) {
    return (await _client.From<OptionNumberDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
  }

  public async Task<OptionBooleanDbRow?> GetBooleanOptionFromOption(long optionId) {
    return (await _client.From<OptionBooleanDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
  }

  private async Task CreateOrUpdateOptionForComponent(long componentId, OptionJson option) {
    OptionDbRow? existingOption = (await _client.From<OptionDbRow>().Where(o => o.ComponentId == componentId && o.Name == option.Name).Get()).Models.FirstOrDefault();
    var optionDb = existingOption is null ? await CreateOptionForComponent(componentId, option) : await UpdateOptionForComponent(existingOption, option);
    if (optionDb is null) throw new Exception("Failed to insert Option.");
    switch(optionDb.Type) {
      case "number": {
        await CreateOrUpdateNumberOptionForOption(optionDb.Id, (NumberOption)option);
        break;
      }
      case "boolean": {
        await CreateOrUpdateBooleanOptionForOption(optionDb.Id, (BooleanOption)option);
        break;
      }
    }
  }

  private async Task<OptionDbRow> CreateOptionForComponent(long componentId, OptionJson option) {
    var optionDb = new OptionDbRow() {
      Name = option.Name,
      ComponentId = componentId,
      Type = option.Type,
    };
    return (await _client.From<OptionDbRow>().Insert(optionDb)).Models.First();
  }

  private async Task<OptionDbRow> UpdateOptionForComponent(OptionDbRow optionDb, OptionJson optionJson) {
    optionDb.Type = optionJson.Type;
    return (await _client.From<OptionDbRow>().Update(optionDb)).Models.First();
  }

  private async Task CreateOrUpdateNumberOptionForOption(long optionId, NumberOption option) {
    OptionNumberDbRow? existingOption = (await _client.From<OptionNumberDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
    OptionNumberDbRow optionDb = existingOption is null ? await CreateNumberOptionForOption(optionId, option) : await UpdateNumberOptionForOption(existingOption, option);
  }

  private async Task<OptionNumberDbRow> CreateNumberOptionForOption(long optionId, NumberOption option) {
    var optionNumberDb = new OptionNumberDbRow() {
      OptionId = optionId,
      Units = option.Units,
      Default = option.Default,
    };
    return (await _client.From<OptionNumberDbRow>().Insert(optionNumberDb)).Models.First();
  }

  private async Task<OptionNumberDbRow> UpdateNumberOptionForOption(OptionNumberDbRow optionDb, NumberOption optionJson) {
    optionDb.Units = optionJson.Units;
    optionDb.Default = optionJson.Default;
    return (await _client.From<OptionNumberDbRow>().Update(optionDb)).Models.First();
  }

  private async Task CreateOrUpdateBooleanOptionForOption(long optionId, BooleanOption option) {
    OptionBooleanDbRow? existingOption = (await _client.From<OptionBooleanDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
    OptionBooleanDbRow optionDb = existingOption is null ? await CreateBooleanOptionForOption(optionId, option) : await UpdateBooleanOptionForOption(existingOption, option);
  }

  private async Task<OptionBooleanDbRow> CreateBooleanOptionForOption(long optionId, BooleanOption option) {
    var optionBooleanDb = new OptionBooleanDbRow() {
      OptionId = optionId,
      Default = option.Default,
    };
    return (await _client.From<OptionBooleanDbRow>().Insert(optionBooleanDb)).Models.First();
  }

  private async Task<OptionBooleanDbRow> UpdateBooleanOptionForOption(OptionBooleanDbRow optionDb, BooleanOption optionJson) {
    optionDb.Default = optionJson.Default;
    return (await _client.From<OptionBooleanDbRow>().Update(optionDb)).Models.First();
  }
}
