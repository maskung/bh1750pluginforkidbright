Blockly.JavaScript['bh1750.get_illuminance'] = function(block) {
	return [
		'DEV_I2C1.BH1750(' + block.getFieldValue('CHANNEL') + ', ' + block.getFieldValue('ADDRESS') + ').get_illuminance()',
		Blockly.JavaScript.ORDER_ATOMIC
	];
};
