
using Capture.DbRow;

public partial class SupabaseService {
  private async Task CreateOrUpdateOptionForComponent(long componentId, OptionJson option) {
    OptionDbRow? existingOption = (await Client.From<OptionDbRow>().Where(o => o.ComponentId == componentId && o.Name == option.Name).Get()).Models.FirstOrDefault();
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
    return (await Client.From<OptionDbRow>().Insert(optionDb)).Models.First();
  }

  private async Task<OptionDbRow> UpdateOptionForComponent(OptionDbRow optionDb, OptionJson optionJson) {
    optionDb.Type = optionJson.Type;
    return (await Client.From<OptionDbRow>().Update(optionDb)).Models.First();
  }

  private async Task CreateOrUpdateNumberOptionForOption(long optionId, NumberOption option) {
    OptionNumberDbRow? existingOption = (await Client.From<OptionNumberDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
    OptionNumberDbRow optionDb = existingOption is null ? await CreateNumberOptionForOption(optionId, option) : await UpdateNumberOptionForOption(existingOption, option);
  }

  private async Task<OptionNumberDbRow> CreateNumberOptionForOption(long optionId, NumberOption option) {
    var optionNumberDb = new OptionNumberDbRow() {
      OptionId = optionId,
      Units = option.Units,
      Default = option.Default,
    };
    return (await Client.From<OptionNumberDbRow>().Insert(optionNumberDb)).Models.First();
  }

  private async Task<OptionNumberDbRow> UpdateNumberOptionForOption(OptionNumberDbRow optionDb, NumberOption optionJson) {
    optionDb.Units = optionJson.Units;
    optionDb.Default = optionJson.Default;
    return (await Client.From<OptionNumberDbRow>().Update(optionDb)).Models.First();
  }

  private async Task CreateOrUpdateBooleanOptionForOption(long optionId, BooleanOption option) {
    OptionBooleanDbRow? existingOption = (await Client.From<OptionBooleanDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
    OptionBooleanDbRow optionDb = existingOption is null ? await CreateBooleanOptionForOption(optionId, option) : await UpdateBooleanOptionForOption(existingOption, option);
  }

  private async Task<OptionBooleanDbRow> CreateBooleanOptionForOption(long optionId, BooleanOption option) {
    var optionBooleanDb = new OptionBooleanDbRow() {
      OptionId = optionId,
      Default = option.Default,
    };
    return (await Client.From<OptionBooleanDbRow>().Insert(optionBooleanDb)).Models.First();
  }

  private async Task<OptionBooleanDbRow> UpdateBooleanOptionForOption(OptionBooleanDbRow optionDb, BooleanOption optionJson) {
    optionDb.Default = optionJson.Default;
    return (await Client.From<OptionBooleanDbRow>().Update(optionDb)).Models.First();
  }
}
