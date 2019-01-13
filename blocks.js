Blockly.Blocks["bh1750.get_illuminance"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.BH1750_GET_LUX_TITLE);

		// i2c channel 0 to 64
		var channel_array = [];
		for (var i = 0;i <= 64; i++) {
			channel_array.push([String(i), String(i)]);
		}
		this.appendDummyInput()
			.appendField(Blockly.Msg.CHANNEL)
			.appendField(new Blockly.FieldDropdown(channel_array), 'CHANNEL');

		// device addess
		this.appendDummyInput()
			.appendField(Blockly.Msg.ADDRESS)
			.appendField(new Blockly.FieldDropdown([
				["0x23", "35"],
				["0x5c", "92"],
			]), 'ADDRESS');

		this.setOutput(true, 'Number');
		this.setInputsInline(true);
		this.setPreviousStatement(false);
		this.setNextStatement(false);
		this.setColour(58);
		this.setTooltip(Blockly.Msg.BH1750_GET_LUX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.BH1750_GET_LUX_HELPURL);
	}
};
